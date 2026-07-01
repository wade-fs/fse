// /runtime/executors/reality_resolver.c
// FSE 第二代認識論執行判定器 (Reality Resolver)
// 職責：
//   - 讀取挑戰引用的 Realities 與 Knowledges
//   - 比對玩家的 Evidence (observations)
//   - 判定認知狀態常數：UNDERSTANDING / MISUNDERSTANDING / MISCONCEPTION
//   - 驅動世界因果演化 (Evolve)，更新感知 (Reveal) 與新訊號
#include "/runtime/include/ansi.h"

#define UNDERSTANDING    0
#define MISUNDERSTANDING 1
#define MISCONCEPTION    2

// 執行判定入口
// 參數：
//   - node_obj: 當前節點物件
//   - actor: 行動者玩家物件
//   - act: 前端/行動傳來的預測資料 (例如：([ "action": "meditate", "target": "spiritual_current" ]))
//   - chal_data: 挑戰 YAML 的詳細資料
//   - cid: 挑戰 ID
int execute(object node_obj, object actor, mapping act, mapping chal_data, string cid) {
    if (!node_obj || !actor || !chal_data) return 0;

    object i18n = load_object("/runtime/services/i18n_service.c");
    string *knowledges = chal_data["knowledges"];
    if (!knowledges || sizeof(knowledges) == 0) {
        return 0;
    }

    // 取得玩家當前的所有觀察 Evidence (observations)
    mapping player_obs;
    if (function_exists("query_observations", actor)) {
        player_obs = actor->query_observations();
    }
    if (!player_obs) player_obs = ([]);

    int total_knowledges = sizeof(knowledges);
    int understood_count = 0;
    int misunderstanding_count = 0;
    string *active_confusions = ({});

    // 暫存評估結果
    mapping eval_results = ([]);

    foreach (string kn_ref in knowledges) {
        string *parts = explode(kn_ref, ".");
        if (sizeof(parts) < 3) continue;

        string reality_type = parts[0];
        string law_id       = parts[1];
        string kn_id        = parts[2];

        // 規則存放路徑：/runtime/realities/<reality_type>/<law_id>.yaml 或本地 fallback
        string law_path = sprintf("/runtime/realities/%s/%s.yaml", reality_type, law_id);
        if (file_size(law_path) <= 0) {
            law_path = sprintf("/realities/%s/%s.yaml", reality_type, law_id);
            if (file_size(law_path) <= 0) continue;
        }

        string raw = read_file(law_path);
        if (!raw) continue;
        mapping law_data = yaml_decode(raw);
        if (!law_data || !law_data["knowledges"] || !law_data["knowledges"][kn_id]) continue;

        mapping kn_data = law_data["knowledges"][kn_id];
        mapping evaluate = kn_data["evaluate"];
        if (!evaluate) continue;

        // 比對行動是否對齊 (Aligned Action)
        string aligned_act = evaluate["aligned_action"];
        if (aligned_act && act["action"] != aligned_act) {
            eval_results[kn_ref] = MISCONCEPTION;
            string precon = evaluate["default_misconception"] || "concept_misunderstood";
            active_confusions += ({ precon });
            continue;
        }

        // 比對 required_observations (Evidence)
        string *req_obs = evaluate["required_observations"];
        int has_all_reqs = 1;
        string *missing_reqs = ({});

        if (req_obs) {
            foreach (string req in req_obs) {
                if (undefinedp(player_obs[req])) {
                    has_all_reqs = 0;
                    missing_reqs += ({ req });
                }
            }
        }

        if (has_all_reqs) {
            eval_results[kn_ref] = UNDERSTANDING;
            understood_count++;
        } else {
            // 比對 misunderstanding_patterns
            mixed *patterns = evaluate["misunderstanding_patterns"];
            int pattern_matched = 0;
            string yield_status = "";

            if (patterns && arrayp(patterns)) {
                foreach (mapping pattern in patterns) {
                    string *has_list = pattern["has"];
                    string *miss_list = pattern["missing"];
                    int match = 1;

                    if (has_list) {
                        foreach (string h in has_list) {
                            if (undefinedp(player_obs[h])) { match = 0; break; }
                        }
                    }
                    if (match && miss_list) {
                        foreach (string m in miss_list) {
                            if (!undefinedp(player_obs[m])) { match = 0; break; }
                        }
                    }

                    if (match) {
                        pattern_matched = 1;
                        yield_status = pattern["yields"];
                        break;
                    }
                }
            }

            if (pattern_matched && yield_status != "") {
                eval_results[kn_ref] = MISUNDERSTANDING;
                misunderstanding_count++;
                active_confusions += ({ yield_status });
            } else {
                eval_results[kn_ref] = MISCONCEPTION;
                string default_misc = evaluate["default_misconception"] || "concept_misunderstood";
                active_confusions += ({ default_misc });
            }
        }
    }

    // ─── 4. Understanding Evaluation 評估結論 ───
    int final_state = MISCONCEPTION;
    if (understood_count == total_knowledges) {
        final_state = UNDERSTANDING;
    } else if (understood_count > 0 || misunderstanding_count > 0) {
        final_state = MISUNDERSTANDING;
    }

    // ─── 5. Reveal / Memory / Discovery 認知更新 ───
    if (final_state == UNDERSTANDING) {
        string success_msg = chal_data["success_msg"] || "🎉 成功領悟了天地因果法則！\n";
        tell_object(actor, HIG + success_msg + NOR);

        string disc_factor = chal_data["discover_factor"];
        if (disc_factor) {
            load_object("/runtime/services/factor_service.c")->discover_factor(actor, disc_factor);
        }

        int success_prog = chal_data["success_progress"] || 10;
        load_object("/runtime/services/progress_manager.c")->complete_player_quest(actor, cid, "main", success_prog);
    } else {
        object event_bus = load_object("/runtime/services/event_bus.c");
        if (event_bus) {
            event_bus->publish("PlayerConfused", ([
                "player": actor,
                "challenge_id": cid,
                "node_id": node_obj->query_entity_id()
            ]));
        }
        actor->set_temp("is_confused", 1);

        foreach (string conf in active_confusions) {
            actor->set_temp("failure_history/" + conf, 1);
            if (i18n) {
                string t_msg = i18n->translate("so.confusion." + conf);
                if (t_msg && t_msg != "so.confusion." + conf) {
                    tell_object(actor, HIR + t_msg + "\n" NOR);
                }
            }
        }

        string fail_warning = chal_data["failure_warning"] || "【 🌀 產生困惑 】你當前的理解與天地法則產生了斷裂。\n";
        tell_object(actor, RED + fail_warning + NOR);
    }

    // ─── 6. World Consequence & Evolve 世界演化 ───
    mapping consequence_cfg = chal_data["consequence"];
    if (consequence_cfg && node_obj) {
        string state_key = (final_state == UNDERSTANDING) ? "understanding" : 
                           ((final_state == MISUNDERSTANDING) ? "misunderstanding" : "misconception");
        mapping effect = consequence_cfg[state_key];
        if (effect) {
            if (function_exists("apply_adventure_side_effects", node_obj)) {
                node_obj->apply_adventure_side_effects(actor, effect, (final_state == UNDERSTANDING));
            }

            string *new_signals = effect["new_signals"];
            if (new_signals && function_exists("add_observation", actor)) {
                foreach (string sig in new_signals) {
                    actor->add_observation(sig);
                }
            }
        }
    }

    return (final_state == UNDERSTANDING);
}
