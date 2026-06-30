// /std/room.c  (史前文明 PC)
// 房間基底：描述、出口、物件與生物容器
#include "/include/ansi.h"
inherit "/std/object";
inherit "/runtime/core/virtual_object";

private string short_desc;
private string long_desc;
private mapping exits;      // 方向 -> 目標房間路徑
private object *occupants;  // 所有在此房間的物件 (玩家/怪物)
private nosave mapping monster_configs;

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

        // 解析怪物配置
        mixed monsters = config["monsters"];
        if (arrayp(monsters) && sizeof(monsters) > 0) {
            monster_configs = ([]);
            foreach (mapping m in monsters) {
                monster_configs[m["id"]] = m["count"];
            }
            // 延遲在當前 tick 結束後執行，避免 create 期間移動物件造成的時序問題
            call_out("spawn_monsters_from_config", 0);
        }
    }
}

void set_short(string s)  { short_desc = s; }
string query_short()      { return short_desc; }
void set_long(string l)   { long_desc = l; }
string query_long()       { return long_desc; }

void spawn_monsters_from_config() {
    if (!monster_configs) return;
    foreach (string mid, int count in monster_configs) {
        for (int i = 0; i < count; i++) {
            object ob = clone_object(mid);
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

mapping query_exits(object player) {
    // 複製基礎出口
    mapping actual_exits = copy(exits);
    
    // 從 YAML 讀取動態 Reveal 出口配置
    mapping config = query_virtual_config();
    if (config && player) {
        mapping reveal_exits = config["reveal_exits"];
        if (reveal_exits) {
            foreach (string dir, mapping data in reveal_exits) {
                string req_factor = data["requires_factor"];
                if (req_factor && player->has_factor(req_factor)) {
                    actual_exits[dir] = data["dest"];
                }
            }
        }
    }
    return actual_exits;
}

// 物件進入/離開房間 (由 move_object 底層呼叫，或手動維護)
void enter(object ob) {
    if (member_array(ob, occupants) == -1) {
        occupants += ({ ob });
    }
}

void leave(object ob) {
    occupants -= ({ ob });
}

object *query_occupants() {
    // 過濾掉已被 destruct 的物件
    occupants = filter(occupants, (: objectp($1) :));
    return occupants;
}

// 回傳描述（包含出口與生物清單），供 look 指令使用
string describe(object looker) {
    string result = HIG + short_desc + NOR + "\n";
    result += long_desc + "\n";

    // 出口
    mapping actual_exits = query_exits(looker);
    string *dirs = keys(actual_exits);
    if (sizeof(dirs) > 0) {
        result += GRN + "出口：" + NOR + implode(dirs, "  ") + "\n";
    }

    // 在場生物 (僅在擁有基礎觀測或特定生存技能時，顯示生物名字，否則只給予感官提示)
    foreach (object ob in query_occupants()) {
        if (ob == looker) continue;
        if (function_exists("query_name", ob)) {
            if (looker->has_factor("combat_survival")) {
                result += CYN + ob->query_name() + NOR + " 在這裡。\n";
            } else {
                result += YEL + "某個未知的低矮身影在陰影中移動...\n" + NOR;
            }
        }
    }
    return result;
}

// 供 focus 指令調用的感官聚焦系統
string query_sensory_signal(object player, string sense) {
    mapping config = query_virtual_config();
    if (!config) return "周圍一片死寂。";

    // 自 YAML 中獲取定義的感官訊號 (若無，給予預設值)
    mapping signals = config["sensory_signals"];
    if (!signals) {
        signals = ([
            "smell" : "一陣潮濕的風吹來，夾雜著泥土的氣味。",
            "sound" : "微弱的蟲鳴與遠方低沉的風聲。",
            "wind"  : "東南風徐徐吹過，帶著些許熱氣。",
            "ground": "紅棕色的乾硬砂岩，留有一些破碎的蕨類葉片。"
        ]);
    }

    string raw_msg = signals[sense];
    if (!raw_msg) raw_msg = "什麼也沒感知到。";

    // 🌟 FSE 精神所在：根據玩家擁有的 Factor 對感官訊號進行「翻譯」或「顯現 (Reveal)」
    if (sense == "smell") {
        if (player->has_factor("predator_scent")) {
            return HIG + "[ 👃 生態分析 - 氣味 ] " + NOR + raw_msg + "\n" +
                   RED + "【 ⚠️ 警告 】這種腥臭味特徵符合中大型捕食者（如原雞龍或初獸），距離你非常近！" + NOR;
        } else {
            // 觸發 Confusion 標記（因為玩家聞到了但不懂其危險，為之後被襲擊死亡做鋪墊）
            player->player_confused("identify_scent");
            return HIG + "[ 👃 感知 - 氣味 ] " + NOR + raw_msg + " (你對這種氣味感到些許困惑...)";
        }
    }

    if (sense == "sound") {
        if (player->has_factor("vibration_translation")) {
            return HIG + "[ 👂 生態分析 - 聲音 ] " + NOR + raw_msg + "\n" +
                   YEL + "【 💡 領悟 】低頻的震動聲頻率規律，代表有一隻恐龍正踏著碎步在附近踱步。" + NOR;
        } else {
            return HIG + "[ 👂 感知 - 聲音 ] " + NOR + raw_msg;
        }
    }

    if (sense == "ground") {
        string bname = base_name(this_object());
        // 如果是三疊紀荒野且尚未獲得 thermodynamics (鑽木取火) 概念，則引導玩家尋找 root
        if (strsrch(bname, "triassic_plains") != -1 && !player->has_factor("thermodynamics")) {
            player->set_temp("found_roots", 1);
            return HIG + "[ 👁️ 感知 - 地面 ] " + NOR + raw_msg + "\n" +
                   YEL + "【 🔍 發現 】在那些碎石縫中，你找到了幾株耐旱蕨類植物乾枯的根部 (roots)。\n" +
                   "這些根部極度乾燥且富含纖維。你可以嘗試將它們與這片荒野隨處可見的松科樹枝 (branches) 劇烈摩擦 (rub) 來產生火花！" + NOR;
        }
        
        // 如果是巨型蕨類森林，專注地面則引導發現黑曜石，提示敲擊 strike 指令的物理機制
        if (strsrch(bname, "fern_forest") != -1) {
            player->set_temp("found_obsidian", 1);
            return HIG + "[ 👁️ 感知 - 地面 ] " + NOR + raw_msg + "\n" +
                   YEL + "【 🔍 發現 】在鬆軟的落葉堆下，露出了幾塊質地極為堅硬且邊緣銳利的黑曜石 (obsidian)。\n" +
                   "這似乎是打製石器的絕佳材料。你可以嘗試敲擊 (strike) 它們，看看會發生什麼。" + NOR;
        }

        return HIG + "[ 👁️ 感知 - 地面 ] " + NOR + raw_msg;
    }

    return HIG + "[ 👁️ 感知 ] " + NOR + raw_msg;
}

// 🌟 FSE 核心 Reveal 機制：主動檢索是否有新出口因解鎖 factor 而顯現，並回傳其配置的提示字句
string *check_new_reveals(object player, string newly_discovered_factor) {
    mapping config = query_virtual_config();
    string *msgs = ({});
    if (config) {
        mapping reveal_exits = config["reveal_exits"];
        if (reveal_exits) {
            foreach (string dir, mapping data in reveal_exits) {
                if (data["requires_factor"] == newly_discovered_factor) {
                    string msg = data["reveal_msg"] || ("通往「" + dir + "」的路徑顯現了出來！");
                    msgs += ({ msg });
                }
            }
        }
    }
    return msgs;
}

// 🌟 FSE 萬用實體互動器 (Universal Physical Interactor)
// 根據玩家輸入的動作 (action) 與目標 (target) 查閱 YAML 的宣告並執行後果
int resolve_interaction(object player, string action, string target) {
    mapping config = query_virtual_config();
    if (!config) return 0;

    mixed interactions = config["interactions"];
    if (!interactions || !arrayp(interactions)) return 0;

    foreach (mapping act in interactions) {
        if (act["action"] == action && act["target"] == target) {
            
            // ── 檢查守衛：防重複 ──
            string success_factor = act["discover_factor"];
            if (success_factor && player->has_factor(success_factor)) {
                string repeat_msg = act["repeat_msg"] || "你已經熟練地掌握了這個動作的要領，不需要再重試。";
                tell_object(player, YEL + repeat_msg + "\n" + NOR);
                return 1;
            }

            // ── 檢查前置條件 (Prerequisites) ──
            mapping prereqs = act["prerequisites"];
            int passed = 1;
            if (prereqs) {
                // 1. 檢查暫時狀態 temp_state
                string req_temp = prereqs["temp_state"];
                if (req_temp && !player->query_temp(req_temp)) passed = 0;

                // 2. 檢查必要因素 factor
                string req_factor = prereqs["factor"];
                if (req_factor && !player->has_factor(req_factor)) passed = 0;
            }

            if (passed) {
                // ── 成功路徑 ──
                // 輸出成功描述
                if (act["success_msg"]) {
                    tell_object(player, HIG + act["success_msg"] + "\n" + NOR);
                }

                // 設定暫時狀態
                string set_temp = act["set_temp"];
                if (set_temp) player->set_temp(set_temp, 1);

                // 解鎖 Factor
                if (success_factor) {
                    object factor_svc = load_object("/runtime/services/factor_service.c");
                    if (factor_svc) factor_svc->discover_factor(player, success_factor);
                }

                // 完成 Quest
                string comp_quest = act["complete_quest"];
                if (comp_quest) {
                    object pm = load_object("/runtime/services/progress_manager.c");
                    if (pm) pm->complete_player_quest(player, comp_quest, "main", 100);
                }

                // 疲勞與體力變更
                int fatigue = act["fatigue"];
                if (fatigue) player->add_fatigue(fatigue);

                int hp_change = act["hp_change"];
                if (hp_change) player->add_hp(hp_change);

            } else {
                // ── 失敗路徑 ──
                if (act["failure_msg"]) {
                    tell_object(player, RED + act["failure_msg"] + "\n" + NOR);
                }

                // 觸發困惑
                string conf = act["trigger_confusion"];
                if (conf) player->player_confused(conf);

                // 懲罰
                int fail_fatigue = act["fail_fatigue"] || 5;
                player->add_fatigue(fail_fatigue);

                int fail_hp = act["fail_hp"];
                if (fail_hp) player->add_hp(fail_hp);
            }
            return 1; // 成功解析並處理該互動
        }
    }
    return 0; // 當前房間無此互動定義
}




