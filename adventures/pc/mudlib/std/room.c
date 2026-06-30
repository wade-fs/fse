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
mapping query_exits() { return exits; }

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
    string *dirs = keys(exits);
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
        player->set_temp("found_roots", 1);
        return HIG + "[ 👁️ 感知 - 地面 ] " + NOR + raw_msg + "\n" +
               YEL + "【 🔍 發現 】在那些碎石縫中，你找到了幾株耐旱蕨類植物乾枯的根部 (roots)。\n" +
               "這些根部極度乾燥且富含纖維。你可以嘗試將它們與這片荒野隨處可見的松科樹枝 (branches) 劇烈摩擦 (rub) 來產生火花！" + NOR;
    }


    return HIG + "[ 👁️ 感知 ] " + NOR + raw_msg;
}


