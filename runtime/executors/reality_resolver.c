// /runtime/executors/reality_resolver.c
// FSE 第二代認識論執行判定器 (Reality Resolver)
// 職責：
//   - 支援「並行 Reality 評估」（Natural + Social + Spiritual 等分支並行評估）
//   - 讀取挑戰引用的 Realities 與 Knowledges，並比對玩家 Evidence (observations)
//   - 判定認知狀態常數：UNDERSTANDING / MISUNDERSTANDING / MISCONCEPTION
//   - 獨立且並行驅動各 Reality 的世界因果演化 (Evolve)，更新感知 (Reveal) 與新訊號
#include "/runtime/include/ansi.h"

#define UNDERSTANDING    0
#define MISUNDERSTANDING 1
#define MISCONCEPTION    2

// 前置宣告內部評估函式
mixed evaluate_knowledge_branch(object node_obj, object actor, mapping act, string *knowledges, mapping player_obs);

// 執行判定入口
int execute(object node_obj, object actor, mapping act, mapping chal_data, string cid) {
    if (!node_obj || !actor || !chal_data) return 0;

    object i18n = load_object("/runtime/services/i18n_service.c");

    // 取得玩家當前的所有觀察 Evidence (observations)
    mapping player_obs;
    if (function_exists("query_observations", actor)) {
        player_obs = actor->query_observations();
    }
    if (!player_obs) player_obs = ([]);

    // 檢測是否為並行 Reality 配置 (YAML 宣告 realities 映射)
    mapping realities = chal_data["realities"];
    
    // 如果是舊的扁平格式，我們構造一個單一的 realities 分支來進行 Fallback 相容
    if (!realities || !mapp(realities)) {
        string *flat_kns = chal_data["knowledges"];
        if (!flat_kns || sizeof(flat_kns) == 0) return 0;
        
        realities = ([
            "spiritual": ([
                "knowledges": flat_kns
            ])
        ]);
    }

    int total_branches = sizeof(realities);
    int understood_branches = 0;
    string *all_active_confusions = ({});

    // 遍歷並行評估每一個 Reality 分支 (e.g. natural, social, spiritual)
    foreach (string r_type, mixed r_cfg in realities) {
        if (!mapp(r_cfg)) continue;
        
        string *kns = r_cfg["knowledges"];
        if (!kns || sizeof(kns) == 0) continue;

        int branch_state = MISCONCEPTION;
        mixed res = evaluate_knowledge_branch(node_obj, actor, act, kns, player_obs);
        if (mapp(res)) {
            branch_state = res["state"];
            if (branch_state == UNDERSTANDING) {
                understood_branches++;
            } else {
                string *b_confs = res["confusions"];
                if (b_confs) all_active_confusions += b_confs;
            }
        }

        // 獨立驅動此 Reality 分支專屬的世界演化 (Evolve)
        mapping evolve_cfg = chal_data["evolve"];
        if (evolve_cfg && node_obj) {
            string state_key = (branch_state == UNDERSTANDING) ? "understanding" : 
                               ((branch_state == MISUNDERSTANDING) ? "misunderstanding" : "misconception");
            
            // 優先讀取分支底下的獨立 Evolve 設定，例如 evolve["spiritual"]["understanding"]
            mapping branch_evolve = evolve_cfg[r_type];
            if (branch_evolve && mapp(branch_evolve)) {
                mapping effect = branch_evolve[state_key];
                if (effect) {
                    // 執行 side effects (如 HP/靈力/業力增減)
                    if (function_exists("apply_adventure_side_effects", node_obj)) {
                        node_obj->apply_adventure_side_effects(actor, effect, (branch_state == UNDERSTANDING));
                    }

                    // 世界演化產生新訊號，餵入當前 session
                    string *new_signals = effect["new_signals"];
                    if (new_signals && function_exists("add_observation", actor)) {
                        foreach (string sig in new_signals) {
                            actor->add_observation(sig);
                        }
                    }
                }
            } else {
                // 若無分支 Evolve，Fallback 讀取頂層 Evolve (相容舊挑戰格式)
                mapping effect = evolve_cfg[state_key];
                if (effect) {
                    if (function_exists("apply_adventure_side_effects", node_obj)) {
                        node_obj->apply_adventure_side_effects(actor, effect, (branch_state == UNDERSTANDING));
                    }
                    string *new_signals = effect["new_signals"];
                    if (new_signals && function_exists("add_observation", actor)) {
                        foreach (string sig in new_signals) {
                            actor->add_observation(sig);
                        }
                    }
                }
            }
        }
    }

    // ─── 5. 整合評估結論 ───
    int final_success = (understood_branches == total_branches);

    // 🚀 前端即時 Reality 雷達圖數據推播
    if (actor && function_exists("send_json_event", actor)) {
        actor->send_json_event("reality_evaluation", ([
            "challenge_id": cid,
            "final_success": final_success,
            "branches": realities, // 包含了 natural, spiritual 等對應的分支
            "all_confusions": all_active_confusions
        ]));
    }

    if (final_success) {
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

        foreach (string conf in all_active_confusions) {
            actor->set_temp("failure_history/" + conf, 1);
            if (i18n) {
                string t_msg = i18n->translate("so.confusion." + conf);
                if (t_msg && t_msg != "so.confusion." + conf) {
                    tell_object(actor, HIR + t_msg + "\n" NOR);
                } else {
                    // 體面 Fallback 預設提示
                    tell_object(actor, HIR + "【 🌀 心魔糾纏 】你心中產生了一股無形阻礙 (" + conf + ")，干擾著你的理解。\n" NOR);
                }
            }
        }

        string fail_warning = chal_data["failure_warning"] || "【 🌀 產生困惑 】你當前的理解與天地法則產生了斷裂。\n";
        tell_object(actor, RED + fail_warning + NOR);

        // 🚀 蜀山奧德賽天劫失敗：境界受損與修為倒退懲罰
        if (strsrch(cid, "tribulation") != -1) {
            string realm = function_exists("query_realm", actor) ? actor->query_realm() : "golden_core";
            if (realm == "great_ascension") {
                tell_object(actor, HIR + "\n💀 【 💥 飛昇失敗 】 狂暴雷光轟散了你的道基！你從大乘境界跌落回元嬰期！\n\n" + NOR);
                if (function_exists("set_realm", actor)) actor->set_realm("nascent_soul");
                actor->add_hp(-80);
            } else if (realm == "nascent_soul") {
                tell_object(actor, HIR + "\n💀 【 💥 元嬰崩碎 】 天雷將你強求的元嬰撕裂！你跌落回金丹期！\n\n" + NOR);
                if (function_exists("set_realm", actor)) actor->set_realm("golden_core");
                actor->add_hp(-50);
            } else {
                tell_object(actor, HIR + "\n💀 【 💥 金丹受損 】 蠻力抗劫導致金丹出現蛛網般的裂紋，境界倒退！\n\n" + NOR);
                if (function_exists("set_realm", actor)) actor->set_realm("initiation");
                actor->add_hp(-30);
            }
        }
    return 1; // 成功評估並演化，接管該行動
}
}

// 內部單一 Reality 分支之 Knowledge 評估邏輯
mixed evaluate_knowledge_branch(object node_obj, object actor, mapping act, string *knowledges, mapping player_obs) {
    string *active_confusions_out = ({});
    int total_kns = sizeof(knowledges);
    int understood_count = 0;
    int misunderstanding_count = 0;

    foreach (string kn_ref in knowledges) {
        string *parts = explode(kn_ref, ".");
        if (sizeof(parts) < 3) continue;

        string reality_type = parts[0];
        string law_id       = parts[1];
        string kn_id        = parts[2];

        string law_path = sprintf("/runtime/realities/%s/%s.yaml", reality_type, law_id);
        if (file_size(law_path) <= 0) {
            law_path = sprintf("/realities/%s/%s.yaml", reality_type, law_id);
            if (file_size(law_path) <= 0) {
                if (getenv("MUD_TEST_MODE") || this_player()) {
                    write(HIR "⚠️ [Reality Debug] 找不到 Law 檔案: /runtime/realities/" + reality_type + "/" + law_id + ".yaml\n" NOR);
                }
                continue;
            }
        }

        string raw = read_file(law_path);
        if (!raw) {
            if (getenv("MUD_TEST_MODE") || this_player()) {
                write(HIR "⚠️ [Reality Debug] 無法讀取 Law 檔案: " + law_path + "\n" NOR);
            }
            continue;
        }
        mapping law_data = yaml_decode(raw);
        if (!law_data) {
            if (getenv("MUD_TEST_MODE") || this_player()) {
                write(HIR "⚠️ [Reality Debug] Law YAML 解析失敗: " + law_path + "\n" NOR);
            }
            continue;
        }
        if (!law_data["knowledges"] || !law_data["knowledges"][kn_id]) {
            if (getenv("MUD_TEST_MODE") || this_player()) {
                write(HIR "⚠️ [Reality Debug] Law 檔案中找不到 Knowledge ID: " + kn_id + " (於 " + law_path + ")\n" NOR);
            }
            continue;
        }

        mapping kn_data = law_data["knowledges"][kn_id];
        mapping evaluate = kn_data["evaluate"];
        if (!evaluate) continue;

        // 🚀 蜀山奧德賽：心魔與天劫挑戰的 Resolver 認知對抗邏輯
        if (kn_id == "demon_heart_observation" || kn_id == "ego_dissolution") {
            // 強硬對抗直接判定為 Misconception 失敗，並受自我成見糾纏
            if (act["action"] == "fight" || act["action"] == "resist") {
                if (getenv("MUD_TEST_MODE") || this_player()) {
                    write(HIR "  [Reality Debug] 心魔對抗：執意強求對抗，觸發 Misconception！\n" NOR);
                }
                active_confusions_out += ({ "ego_clinging" });
                misunderstanding_count++; // 歸入未理解
                continue;
            }
            // 觀照與放下，更容易達成 Understanding
            if (act["action"] == "observe" || act["action"] == "let_go") {
                // 即使缺少部分 observations，亦可勉強渡過 (Misunderstanding)
                if (getenv("MUD_TEST_MODE") || this_player()) {
                    write(HIC "  [Reality Debug] 心魔觀照：玩家採取禪修放下行為。\n" NOR);
                }
            }
        }

        if (getenv("MUD_TEST_MODE") || this_player()) {
            write(HIK "  [Reality Debug] 正在比對 Knowledge: " + kn_ref + "\n" NOR);
            write(HIK "  [Reality Debug] 玩家當前 Observations: " + sprintf("%O", keys(player_obs)) + "\n" NOR);
        }

        // 比對行動是否對齊 (Aligned Action)
        string aligned_act = evaluate["aligned_action"];
        if (aligned_act && act["action"] != aligned_act) {
            if (getenv("MUD_TEST_MODE") || this_player()) {
                write(HIR "  [Reality Debug] Aligned Action 不對符！預期 " + aligned_act + "，玩家輸入了 " + act["action"] + " (判定為 Misconception)。\n" NOR);
            }
            string precon = evaluate["default_misconception"] || "concept_misunderstood";
            active_confusions_out += ({ precon });
            continue;
        }

        // 比對 required_observations (Evidence)
        string *req_obs = evaluate["required_observations"];
        int has_all_reqs = 1;
        string *missing_obs = ({});

        if (req_obs) {
            foreach (string req in req_obs) {
                if (undefinedp(player_obs[req])) {
                    has_all_reqs = 0;
                    missing_obs += ({ req });
                }
            }
        }

        if (has_all_reqs) {
            if (getenv("MUD_TEST_MODE") || this_player()) {
                write(HIG "  [Reality Debug] 完美比對！收集齊了所有 Evidence (Observations)。\n" NOR);
            }
            understood_count++;
        } else {
            if (getenv("MUD_TEST_MODE") || this_player()) {
                write(YEL "  [Reality Debug] 缺失證據 (Missing Evidence): " + sprintf("%O", missing_obs) + "\n" NOR);
            }

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
                if (getenv("MUD_TEST_MODE") || this_player()) {
                    write(YEL "  [Reality Debug] 匹配到局部偏誤模式 (Misunderstanding Pattern) -> " + yield_status + "\n" NOR);
                }
                misunderstanding_count++;
                active_confusions_out += ({ yield_status });
            } else {
                if (getenv("MUD_TEST_MODE") || this_player()) {
                    write(RED "  [Reality Debug] 無法匹配任何偏誤，進入預設成見 (Default Misconception) -> " + (evaluate["default_misconception"] || "concept_misunderstood") + "\n" NOR);
                }
                string default_misc = evaluate["default_misconception"] || "concept_misunderstood";
                active_confusions_out += ({ default_misc });
            }
        }
    }

    int state;
    if (understood_count == total_kns) {
        state = UNDERSTANDING;
    } else if (understood_count > 0 || misunderstanding_count > 0) {
        state = MISUNDERSTANDING;
    } else {
        state = MISCONCEPTION;
    }
    return ([ "state": state, "confusions": active_confusions_out ]);
}
