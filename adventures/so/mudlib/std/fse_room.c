// /std/fse_room.c  (FSE 節點基底類別)
// 職責：管理 Actor 進出、Path 可見性、感知訊號、Reveal 機制與基礎狀態查詢。
// 命名原則：
//   Node（節點）取代 room；Actor（行動者）取代 occupant；
//   Path（路徑）取代 exit；on_enter/on_leave 取代 enter/leave。
#include "/runtime/include/ansi.h"
inherit "/std/object";
inherit "/runtime/core/virtual_object";

protected string short_desc;
protected string long_desc;
protected mapping paths;       // 方向 -> 目標節點路徑
protected object *actors;      // 目前在此節點的所有物件
private nosave mapping presence_configs;

void create() {
    ::create();
    short_desc = "一個地點";
    long_desc  = "這是一個地點。";
    paths      = ([]);
    actors     = ({});

    // 初始化虛擬路徑與 YAML 設定
    setup_virtual("nodes", "node.yaml");

    mapping config = query_virtual_config();
    if (config) {
        if (config["short"]) set_short(config["short"]);
        if (config["desc"])  set_long(config["desc"]); // description -> desc

        // 解析路徑 (paths)
        mapping ex = config["paths"];
        if (ex) {
            foreach (string dir, string dest in ex) {
                add_path(dir, dest);
            }
        }

        // 解析 presence 配置
        mixed presence = config["presence"];
        if (arrayp(presence) && sizeof(presence) > 0) {
            presence_configs = ([]);
            foreach (mapping p in presence) {
                presence_configs[p["id"]] = p["count"];
            }
            call_out("spawn_presence_from_config", 0);
        }
    }
}

void set_short(string s) { short_desc = s; }
string query_short()     { return short_desc; }
void set_long(string l)  { long_desc = l; }
string query_long()      { return long_desc; }

string query_node_dir() {
    string bn = base_name(this_object());
    int idx = rindex(bn, "/");
    if (idx == -1) return "/";
    return bn[0..idx];
}

void spawn_presence_from_config() {
    if (!presence_configs) return;
    foreach (string pid, int count in presence_configs) {
        for (int i = 0; i < count; i++) {
            object ob = clone_object(pid);
            if (ob) {
                ob->set_respawn_node(base_name(this_object()));
                move_object(ob, this_object());
                on_enter(ob);
            }
        }
    }
}

void add_path(string dir, string dest) { paths[dir] = dest; }

private mapping _query_reveal_paths() {
    mapping config = query_virtual_config();
    if (config) return config["reveal_paths"];
    return 0;
}

// 回傳對特定 actor 可見的路徑（考量 reveal 條件與業力鬼打牆）
mapping query_paths(object actor) {
    mapping visible = copy(paths);

    // Karma Loop：若業力過重且節點設定了業力限制，或者業力大於 80 時，出口全部重定向回自身
    int karma_threshold = 80;
    mapping config = query_virtual_config();
    if (config && !undefinedp(config["karma_loop_threshold"])) {
        karma_threshold = config["karma_loop_threshold"];
    }

    if (actor && function_exists("query_karma", actor) && actor->query_karma() >= karma_threshold) {
        foreach (string dir, string dest in visible) {
            visible[dir] = base_name(this_object()); // 鬼打牆，原地打轉
        }
        tell_object(actor, HIR "【心魔】四方皆路，卻條條歸原。\n" NOR);
        return visible;
    }

    mapping reveal  = _query_reveal_paths();
    if (reveal && actor) {
        foreach (string dir, mapping data in reveal) {
            string req_factor = data["requires_factor"];
            if (req_factor && actor->has_factor(req_factor)) {
                visible[dir] = data["dest"];
            }
        }
    }
    return visible;
}

// ── Actor 進出 ──────────────────────────────────────
void on_enter(object actor) {
    if (member_array(actor, actors) == -1) {
        actors += ({ actor });
    }
    if (actor && function_exists("query_role", actor) && actor->query_role() == "player") {
        foreach (object presence in query_actors()) {
            if (presence && presence != actor && function_exists("check_detection", presence)) {
                presence->check_detection(actor);
            }
        }
        
        // 預留：同門共鳴勾子（實作時由 social_service 計算）
        // this_object()->check_sect_resonance(actor, query_actors());
    }
}

void on_leave(object actor) {
    actors -= ({ actor });
}

object *query_actors() {
    actors = filter(actors, (: objectp($1) :));
    return actors;
}

// ── 節點狀態（回傳 mapping，由 node_executor 決定呈現方式）──
mapping query_node_state(object actor) {
    mapping visible_paths = query_paths(actor);

    // 組裝在場 actors 資訊
    mixed *visible_actors = ({});
    foreach (object ob in query_actors()) {
        if (ob == actor) continue;
        if (!function_exists("query_name", ob)) continue;

        string display_name;
        if (function_exists("query_identifiable_by_factors", ob)) {
            string *reqs = ob->query_identifiable_by_factors();
            int known = 1;
            if (reqs && sizeof(reqs) > 0) {
                known = 0;
                foreach (string f in reqs) {
                    if (actor->has_factor(f)) { known = 1; break; }
                }
            }
            display_name = known ? ob->query_name()
                                 : (ob->query_unknown_name() || "一個隱約的存在");
        } else {
            display_name = ob->query_name();
        }
        visible_actors += ({ ([ "name": display_name ]) });
    }

    return ([
        "short":   short_desc,
        "long":    long_desc,
        "paths":   visible_paths,
        "actors":  visible_actors,
    ]);
}

// ── 感知訊號 ────────────────────────────────────────
string query_sensory_signal(object actor, string sense) {
    mapping config = query_virtual_config();
    if (!config) return "周圍一片死寂。";

    mapping signals = config["sensory_signals"];
    if (!signals) {
        signals = ([
            "smell" : "一陣潮濕的風吹來，夾雜著泥土的氣味。",
            "sound" : "微弱的蟲鳴與遠方低沉的風聲。",
            "wind"  : "東南風徐徐吹過，帶著些許熱氣。",
            "ground": "紅砂岩地表，留有一些破碎的蕨類葉片。"
        ]);
    }

    mixed sense_data = signals[sense];
    if (!sense_data) return HIG + "[ 👁️ 感知 ] " + NOR + "什麼也沒感知到。";

    string display_msg;
    if (mappingp(sense_data)) {
        // 功法過濾：檢查此 signal 是否需要特定 sutra factor 
        string sutra_req = sense_data["requires_sutra"];
        if (sutra_req && actor && !actor->has_factor(sutra_req)) {
            return HIG + "[ 👁️ 感知 ] " + NOR + "你感知到某種細微的精神波動，但法門不對，無法解讀。";
        }

        display_msg = sense_data["default_msg"] || "周圍沒有什麼特別的。";
        mapping disc = sense_data["discovery"];
        if (disc && actor) {
            int can_discover = 1;
            string no_factor = disc["requires_no_factor"];
            if (no_factor && actor->has_factor(no_factor)) can_discover = 0;
            string req_factor = disc["requires_factor"];
            if (req_factor && !actor->has_factor(req_factor)) {
                can_discover = 0;
                string fail_conf = disc["failure_confusion"];
                if (fail_conf) actor->player_confused(fail_conf);
            }
            if (can_discover) {
                string set_temp = disc["set_temp"];
                if (set_temp) actor->set_temp(set_temp, 1);
                string disc_factor = disc["discover_factor"];
                if (disc_factor) {
                    object factor_svc = load_object("/runtime/services/factor_service.c");
                    if (factor_svc) factor_svc->discover_factor(actor, disc_factor);
                }
                string disc_msg = disc["msg"] || "";
                if (disc_msg != "") display_msg += "\n" + YEL + disc_msg + NOR;
            }
        }
    } else if (stringp(sense_data)) {
        display_msg = sense_data;
    }

    mapping sense_labels = ([
        "smell" : "感知 - 氣味",
        "sound" : "感知 - 聲音",
        "ground": "感知 - 地面",
        "wind"  : "感知 - 風向",
    ]);
    string label = sense_labels[sense] || "感知";
    return HIG + "[ 👁️ " + label + " ] " + NOR + display_msg;
}

// ── Reveal 檢查（新探索因素後呼叫）────────────────
string *check_new_reveals(object actor, string newly_discovered_factor) {
    string *msgs = ({});
    mapping reveal = _query_reveal_paths();
    if (reveal) {
        foreach (string dir, mapping data in reveal) {
            if (data["requires_factor"] == newly_discovered_factor) {
                string msg = data["reveal_msg"] || ("通往「" + dir + "」的路徑顯現了出來！");
                msgs += ({ msg });
            }
        }
    }
    return msgs;
}

// ── 供子類覆寫的回調介面 ─────────────────────────
void apply_adventure_side_effects(object actor, mapping act, int passed) {}
void on_item_spawned(object ob, mapping give_item) {}

// ── 互動解析 ────────────────────────────────────────
int resolve_interaction(object actor, string action, string target) {
    mapping config = query_virtual_config();
    if (!config || !actor) return 0;
    mixed inters = config["interactions"];
    if (!inters || !arrayp(inters) || sizeof(inters) == 0) return 0;

    foreach (mapping act in inters) {
        int target_matched = 0;
        mixed t_cfg = act["target"];
        if (act["action"] == action) {
            if (undefinedp(t_cfg) || !t_cfg || t_cfg == "") {
                if (undefinedp(target) || !target || target == "") target_matched = 1;
            } else if (stringp(t_cfg) && t_cfg == target) {
                target_matched = 1;
            } else if (arrayp(t_cfg) && member_array(target, t_cfg) != -1) {
                target_matched = 1;
            }
        }
        if (!target_matched) continue;

        // ── 檢查感知門檻 (Prerequisites/Gate) ──
        mapping prereqs = act["prerequisites"];
        int passed = 1;
        if (prereqs) {
            string req_temp = prereqs["temp_state"];
            if (req_temp && !actor->query_temp(req_temp)) passed = 0;
            string req_factor = prereqs["factor"];
            if (req_factor && !actor->has_factor(req_factor)) passed = 0;
            string req_item = prereqs["item"];
            if (req_item) {
                object *inv = all_inventory(actor);
                int has_it = 0;
                foreach (object ob in inv) {
                    if (ob->query_item_id() == req_item) { has_it = 1; break; }
                }
                if (!has_it) passed = 0;
            }
        }

        if (!passed) {
            string gate_msg = act["gate_msg"] || "【 🌀 心緒未定 】你正想打坐吐納，但四周的環境與風勢讓你心神不定，無法進入狀態。似乎需要先仔細觀察周遭環境。";
            tell_object(actor, YEL + gate_msg + "\n" + NOR);
            return 1; // 攔截響應，不是沉默
        }

        // 如果互動指定了專屬的 Reality Resolver，我們直接將判定委託給 Reality 引擎，不由互動層寫死成敗
        string res_challenge = act["resolver"];
        if (res_challenge) {
            object node_exec = load_object("/runtime/services/node_executor.c");
            if (node_exec) {
                // 構造偽裝的 AST 代表玩家行動的 Predict 預測
                mapping fake_ast = ([
                    "action": action,
                    "target": target
                ]);
                
                // 從 node 載入挑戰設定
                string node_dir = this_object()->query_node_dir();
                string chal_path = sprintf("%schallenges/%s.yaml", node_dir, res_challenge);
                if (file_size(chal_path) > 0) {
                    string raw = read_file(chal_path);
                    if (raw) {
                        mapping chal_data = yaml_decode(raw);
                        if (chal_data) {
                            // 呼叫 Reality Resolver Executor
                            object resolver_exec = load_object("/runtime/executors/reality_resolver.c");
                            if (resolver_exec) {
                                resolver_exec->execute(this_object(), actor, fake_ast, chal_data, res_challenge);
                                return 1;
                            }
                        }
                    }
                }
            }
        }

        string success_factor = act["discover_factor"];
        if (success_factor && actor->has_factor(success_factor)) {
            string repeat_msg = act["repeat_msg"] || "你已掌握這個動作的要領，無需重試。";
            tell_object(actor, YEL + repeat_msg + "\n" + NOR);
            return 1;
        }

        if (passed) {
            if (act["success_msg"]) tell_object(actor, HIG + act["success_msg"] + "\n" + NOR);
            string set_temp = act["set_temp"];
            if (set_temp) actor->set_temp(set_temp, 1);
            if (success_factor) {
                object factor_svc = load_object("/runtime/services/factor_service.c");
                if (factor_svc) factor_svc->discover_factor(actor, success_factor);
            }
            string comp_quest = act["complete_quest"];
            if (comp_quest) {
                object pm = load_object("/runtime/services/progress_manager.c");
                if (pm) pm->complete_player_quest(actor, comp_quest, "main", 100);
            }
            this_object()->apply_adventure_side_effects(actor, act, 1);
            mapping give_item = act["give_item"];
            if (give_item) {
                object ob = clone_object("/std/item.c");
                if (ob) {
                    ob->set_item_id(give_item["id"] || "item");
                    ob->set_name(give_item["name"] || "未知物品");
                    ob->set_long(give_item["desc"] || "無詳細描述。");
                    int base_dur = give_item["durability"];
                    if (base_dur > 0) ob->set_durability(base_dur);
                    this_object()->on_item_spawned(ob, give_item);
                    move_object(ob, actor);
                    tell_object(actor, HIY + "🎁 你獲得了物品：[" + ob->query_name() + "]，已放入背包(i)。\n" + NOR);
                }
            }
        } else {
            if (act["failure_msg"]) tell_object(actor, RED + act["failure_msg"] + "\n" + NOR);
            string conf = act["trigger_confusion"];
            if (conf) actor->player_confused(conf);
            this_object()->apply_adventure_side_effects(actor, act, 0);
        }
        return 1;
    }
    return 0;
}
