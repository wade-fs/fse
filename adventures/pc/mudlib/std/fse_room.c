// /std/fse_room.c  (FSE Engine Core Room Base Class)
// 通用 FSE 遊戲引擎房間基座：管理感知、物理交互、Reveal機制與基礎描述
#include "/runtime/include/ansi.h"
inherit "/std/object";
inherit "/runtime/core/virtual_object";

protected string short_desc;
protected string long_desc;
protected mapping exits;      // 方向 -> 目標房間路徑
protected object *occupants;  // 所有在此房間的物件
private nosave mapping presence_configs;

void create() {
    ::create();
    short_desc = "一個地點";
    long_desc  = "這是一個地點。";
    exits      = ([]);
    occupants  = ({});
    
    // 初始化虛擬路徑與 YAML 設定
    setup_virtual("rooms", "room.yaml");
    
    mapping config = query_virtual_config();
    if (config) {
        if (config["short"]) set_short(config["short"]);
        if (config["long"]) set_long(config["long"]);

        // 解析出口
        mapping ex = config["exits"];
        if (ex) {
            foreach (string dir, string dest in ex) {
                add_exit(dir, dest);
            }
        }

        // 解析存在實體 (presence) 配置
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

void set_short(string s)  { short_desc = s; }
string query_short()      { return short_desc; }
void set_long(string l)   { long_desc = l; }
string query_long()       { return long_desc; }

void spawn_presence_from_config() {
    if (!presence_configs) return;
    foreach (string pid, int count in presence_configs) {
        for (int i = 0; i < count; i++) {
            object ob = clone_object(pid);
            if (ob) {
                ob->set_respawn_room(base_name(this_object()));
                move_object(ob, this_object());
                enter(ob);
            }
        }
    }
}

void add_exit(string dir, string dest) {
    exits[dir] = dest;
}

private mapping _query_reveal_exits() {
    mapping config = query_virtual_config();
    if (config) return config["reveal_exits"];
    return 0;
}

mapping query_exits(object player) {
    mapping actual_exits = copy(exits);
    
    // 🚀 蜀山奧德賽：高業力鬼打牆 (Karma Loop)
    if (player && player->query_karma() > 80) {
        mapping config = query_virtual_config();
        if (config && config["karma_loop"]) {
            // 所有方向的出口，都有 70% 機率直接將目的地扭曲回自己（形成鬼打牆）
            foreach (string dir, string dest in actual_exits) {
                if (random(100) < 70) {
                    actual_exits[dir] = base_name(this_object());
                }
            }
            tell_object(player, YEL + "🌀 【業障迷局】 你四下張望，只覺得周圍景色萬般熟悉，似乎落入了無休止的因果循環中...\n" + NOR);
        }
    }
    
    mapping reveal_exits = _query_reveal_exits();
    if (reveal_exits && player) {
        int karma = player->query_karma();
        foreach (string dir, mapping data in reveal_exits) {
            // 🚀 蜀山奧德賽：高業力有 50% 機率遮蔽需要 Reveal 的隱密路徑
            if (karma > 60 && random(100) < 50) {
                continue;
            }
            string req_factor = data["requires_factor"];
            if (req_factor && player->has_factor(req_factor)) {
                actual_exits[dir] = data["dest"];
            }
        }
    }
    return actual_exits;
}

void enter(object ob) {
    if (member_array(ob, occupants) == -1) {
        occupants += ({ ob });
    }
    
    if (ob && function_exists("query_role", ob) && ob->query_role() == "player") {
        foreach (object presence in query_occupants()) {
            if (presence && presence != ob && function_exists("check_detection", presence)) {
                presence->check_detection(ob);
            }
        }
    }
}

void leave(object ob) {
    occupants -= ({ ob });
}

object *query_occupants() {
    occupants = filter(occupants, (: objectp($1) :));
    return occupants;
}

string describe(object looker) {
    string result = HIG + short_desc + NOR + "\n";
    
    // 🚀 蜀山奧德賽：業力感知扭曲
    if (looker) {
        int karma = looker->query_karma();
        if (karma > 70) {
            result += RED + "【業障障目】 你眼前一片朦朧，四周的景物似乎在扭曲、顫抖，隱約有哀嚎聲在耳畔迴盪...\n" + NOR;
        } else if (karma > 40) {
            result += YEL + "【心念浮躁】 你的道心隱隱不穩，這裡的氣息讓你感到一絲煩悶與不安。\n" + NOR;
        }
    }
    
    result += long_desc + "\n";

    mapping actual_exits = query_exits(looker);
    string *dirs = keys(actual_exits);
    if (sizeof(dirs) > 0) {
        result += GRN + "出口：" + NOR + implode(dirs, "  ") + "\n";
    }

    foreach (object ob in query_occupants()) {
        if (ob == looker) continue;
        if (function_exists("query_name", ob)) {
            if (function_exists("query_identifiable_by_factors", ob)) {
                string *reqs = ob->query_identifiable_by_factors();
                int known = 1;
                if (reqs && sizeof(reqs) > 0) {
                    known = 0;
                    foreach (string f in reqs) {
                        if (looker->has_factor(f)) {
                            known = 1;
                            break;
                        }
                    }
                }
                if (known) {
                    result += CYN + ob->query_name() + NOR + " 在這裡。\n";
                } else {
                    string unknown_desc = ob->query_unknown_name() || "一個隱約的生物身影";
                    result += YEL + unknown_desc + " 在這裡。\n" + NOR;
                }
            } else {
                result += CYN + ob->query_name() + NOR + " 在這裡。\n";
            }
        }
    }
    return result;
}

string query_sensory_signal(object player, string sense) {
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
        display_msg = sense_data["default_msg"] || "周圍沒有什麼特別的。";
        mapping disc = sense_data["discovery"];
        if (disc && player) {
            int can_discover = 1;
            
            string no_factor = disc["requires_no_factor"];
            if (no_factor && player->has_factor(no_factor)) {
                can_discover = 0;
            }
            
            string req_factor = disc["requires_factor"];
            if (req_factor && !player->has_factor(req_factor)) {
                can_discover = 0;
                string fail_conf = disc["failure_confusion"];
                if (fail_conf) player->player_confused(fail_conf);
            }

            if (can_discover) {
                string set_temp = disc["set_temp"];
                if (set_temp) player->set_temp(set_temp, 1);
                
                string disc_factor = disc["discover_factor"];
                if (disc_factor) {
                    object factor_svc = load_object("/runtime/services/factor_service.c");
                    if (factor_svc) factor_svc->discover_factor(player, disc_factor);
                }

                string disc_msg = disc["msg"] || "";
                if (disc_msg != "") {
                    display_msg += "\n" + YEL + disc_msg + NOR;
                }
            }
        }
    } else if (stringp(sense_data)) {
        display_msg = sense_data;
    }

    string sense_label = "感知";
    if (sense == "smell") sense_label = "感知 - 氣味";
    if (sense == "sound") sense_label = "感知 - 聲音";
    if (sense == "ground") sense_label = "感知 - 地面";
    if (sense == "wind") sense_label = "感知 - 風向";
    
    return HIG + "[ 👁️ " + sense_label + " ] " + NOR + display_msg;
}

string *check_new_reveals(object player, string newly_discovered_factor) {
    string *msgs = ({});
    mapping reveal_exits = _query_reveal_exits();
    if (reveal_exits) {
        foreach (string dir, mapping data in reveal_exits) {
            if (data["requires_factor"] == newly_discovered_factor) {
                string msg = data["reveal_msg"] || ("通往「" + dir + "」的路徑顯現了出來！");
                msgs += ({ msg });
            }
        }
    }
    return msgs;
}

void apply_adventure_side_effects(object player, mapping act, int passed) {
    if (!player || !act) return;
    
    if (passed) {
        // 🚀 蜀山奧德賽：讀取 YAML 中的 karma 屬性並套用業力增減
        int act_karma = act["karma"];
        if (act_karma != 0) {
            player->add_karma(act_karma);
            if (act_karma > 0) {
                tell_object(player, YEL + "⚠️ 此舉引發了因果餘響，業力增加 " + act_karma + " 點。\n" + NOR);
            } else {
                tell_object(player, HIC + "✨ 此舉化解了心中執著，業力消減 " + (-act_karma) + " 點。\n" + NOR);
            }
        }
    }
}
void on_item_spawned(object ob, mapping give_item) {}

int resolve_interaction(object player, string action, string target) {
    mapping config = query_virtual_config();
    if (!config || !player) return 0;

    mixed inters = config["interactions"];
    if (!inters || !arrayp(inters) || sizeof(inters) == 0) return 0;

    foreach (mapping act in inters) {
        int target_matched = 0;
        mixed t_cfg = act["target"];

        if (act["action"] == action) {
            if (undefinedp(t_cfg) || !t_cfg || t_cfg == "") {
                if (undefinedp(target) || !target || target == "") {
                    target_matched = 1;
                }
            } else if (stringp(t_cfg) && t_cfg == target) {
                target_matched = 1;
            } else if (arrayp(t_cfg) && member_array(target, t_cfg) != -1) {
                target_matched = 1;
            }
        }

        if (target_matched) {
            string success_factor = act["discover_factor"];
            if (success_factor && player->has_factor(success_factor)) {
                string repeat_msg = act["repeat_msg"] || "你已經熟練地掌握了這個動作的要領，不需要再重試。";
                tell_object(player, YEL + repeat_msg + "\n" + NOR);
                return 1;
            }

            mapping prereqs = act["prerequisites"];
            int passed = 1;
            if (prereqs) {
                string req_temp = prereqs["temp_state"];
                if (req_temp && !player->query_temp(req_temp)) passed = 0;

                string req_factor = prereqs["factor"];
                if (req_factor && !player->has_factor(req_factor)) passed = 0;

                string req_item = prereqs["item"];
                if (req_item) {
                    object *inv = all_inventory(player);
                    int has_it = 0;
                    foreach (object ob in inv) {
                        if (ob->query_item_id() == req_item) {
                            has_it = 1;
                            break;
                        }
                    }
                    if (!has_it) passed = 0;
                }
            }

            if (passed) {
                if (act["success_msg"]) {
                    tell_object(player, HIG + act["success_msg"] + "\n" + NOR);
                }

                string set_temp = act["set_temp"];
                if (set_temp) player->set_temp(set_temp, 1);

                if (success_factor) {
                    object factor_svc = load_object("/runtime/services/factor_service.c");
                    if (factor_svc) factor_svc->discover_factor(player, success_factor);
                }

                string comp_quest = act["complete_quest"];
                if (comp_quest) {
                    object pm = load_object("/runtime/services/progress_manager.c");
                    if (pm) pm->complete_player_quest(player, comp_quest, "main", 100);
                }

                // 呼叫特定冒險自訂後置修飾副作用 (如體力、飢渴度增減)
                this_object()->apply_adventure_side_effects(player, act, 1);

                mapping give_item = act["give_item"];
                if (give_item) {
                    object ob = clone_object("/std/item.c");
                    if (ob) {
                        ob->set_item_id(give_item["id"] || "item");
                        ob->set_name(give_item["name"] || "未知物品");
                        ob->set_long(give_item["desc"] || "無詳細描述。");
                        int base_dur = give_item["durability"];
                        if (base_dur > 0) {
                            ob->set_durability(base_dur);
                        }
                        this_object()->on_item_spawned(ob, give_item);
                        move_object(ob, player);
                        tell_object(player, HIY + "🎁 你獲得了物品：[" + ob->query_name() + "]，已放入背包(i)。\n" + NOR);
                    }
                }

            } else {
                if (act["failure_msg"]) {
                    tell_object(player, RED + act["failure_msg"] + "\n" + NOR);
                }

                string conf = act["trigger_confusion"];
                if (conf) player->player_confused(conf);

                // 呼叫特定冒險自訂失敗副作用
                this_object()->apply_adventure_side_effects(player, act, 0);
            }
            return 1;
        }
    }
    return 0;
}
