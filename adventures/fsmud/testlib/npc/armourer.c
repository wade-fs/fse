#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/npc.c";

void create() {
    ::create();
    set_name(([ "en": "Shopkeeper Kevin", "zh-TW": "老闆 凱文", "zh-CN": "老板 凯文" ]));
    set_short(([ "en": "$HIB$Shrewd Armour Shop Owner$NOR$" + " Kevin", "zh-TW": "$HIB$精明的防具店老闆$NOR$" + " 凱文", "zh-CN": "$HIB$精明的防具店老板$NOR$" + " 凯文" ]));
    set_long(([
        "en": "Kevin is a shrewd businessman, always wearing a professional smile.\nHe knows the structure of armor inside out and can recommend the most suitable equipment based on your body type.\n",
        "zh-TW": "凱文是個精明的生意人，總是帶著一副職業性的微笑。\n他對防具的構造瞭如指掌，能根據你的體型推薦最適合的裝備。\n",
        "zh-CN": "凯文是个精明的生意人，总是带着一副职业性的微笑。\n他对防具的构造了如指掌，能根据你的体型推荐最适合的装备。\n"
    ]));
    set_id(({"kevin", "armourer", "owner", "老闆"}));
    set_race("human");
    set_level(12);
    set_stat(STAT_INT, 18);
    set_stat(STAT_DEX, 15);
    recalc_stats();
    hp = max_hp;

    // 🚀 重生與走動
    set_respawn(60);
    set_wander_chance(2);
    set_move_range(1);

    add_response(({ "防具", "armour", "shield" }), ({
        "如果你想活得久一點，就得穿好一點的甲。",
        "我的皮甲都是用上等的魔獸皮製作的。",
        "盾牌在關鍵時刻能救你一命。"
    }));

    // 🚀 新增：任務互動
    add_response(({ "quest", "任務" }), (:
        object tp = this_player();
        if (tp->query_quest("collect_fur")) {
            if (tp->query_quest("collect_fur")["status"] == "active") {
                return "我還在等那 3 張狼皮來趕製這批訂單呢。";
            } else {
                return "上次那些狼皮幫了大忙，現在我的存貨很充足。";
            }
        }
        load_object("/secure/quest_d.c")->accept_quest(tp, "collect_fur");
        return "我想做一件暖和的斗篷，你能幫我找 3 張『狼皮』來嗎？我會給你一條不錯的腰帶作為報酬。";
    :));

    add_response(({ "report", "回報" }), (:
        object tp = this_player();
        if (tp->query_quest("collect_fur") && tp->query_quest("collect_fur")["status"] == "active") {
            if (load_object("/secure/quest_d.c")->complete_quest(tp, "collect_fur")) {
                return "喔！好漂亮的毛皮。這是答應給你的腰帶。";
            }
        }
        return "沒有 3 張狼皮的話，我可沒辦法完成工作。";
    :));
}
