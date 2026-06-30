#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/npc.c";

void create() {
    ::create();
    set_name(([ "en": "Thorin the Blacksmith", "zh-TW": "鐵匠 索林", "zh-CN": "铁匠 索林" ]));
    set_short(([ "en": "$YEL$Stout Dwarf Blacksmith$NOR$" + " Thorin", "zh-TW": "$YEL$粗壯的矮人鐵匠$NOR$" + " 索林", "zh-CN": "$YEL$粗壮的矮人铁匠$NOR$" + " 索林" ]));
    set_long(([
        "en": "This is a typical dwarf, with a red beard tied in braids and bulging arm muscles.\nHe is sweating profusely while hammering a piece of glowing red pig iron.\n",
        "zh-TW": "這是一位典型的矮人，紅色的鬍鬚紮成辮子，手臂肌肉隆起。\n他正滿頭大汗地敲打著一塊通紅的生鐵。\n",
        "zh-CN": "这是一位典型的矮人，红色的胡须扎成辫子，手臂肌肉隆起。\n他正满头大汗地敲打着一块通红的生铁。\n"
    ]));
    set_id(({"thorin", "blacksmith", "dwarf", "鐵匠"}));
    set_race("dwarf");
    set_level(25);
    set_stat(STAT_STR, 30);
    set_stat(STAT_CON, 30);
    recalc_stats();
    hp = max_hp;

    // 🚀 新增：重生與走動
    set_respawn(60);      // 60秒重生
    set_wander_chance(5); // 5% 機率走動
    set_move_range(3);    // 最多離家 3 格

    // 🚀 新增：傳授技能
    set_skill_to_teach("forging", 30);

    add_response(({ "武器", "weapon" }), ({
        "想要好武器？你找對人了！",
        "我打造的劍可以輕易劈開史萊姆的皮。",
        "看看那邊架子上的短劍，那是我的得意之作。"
    }));
    add_response(({ "防具", "armour" }), ({
        "雖然我更擅長打造武器，但防具我也略知一二。",
        "防禦跟進攻一樣重要，別忘了買件皮甲防身。"
    }));
    add_response("索林", "沒錯，我就是索林。想要委託打造什麼嗎？");

    // 🚀 新增：任務互動
    add_response(({ "quest", "任務" }), (:
        object tp = this_player();
        if (tp->query_quest("crab_armour")) {
            if (tp->query_quest("crab_armour")["status"] == "active") {
                return "快去幫我找 2 塊螃蟹殼回來，我好用來加固那些防具。";
            } else {
                return "多虧了你帶回來的螃蟹殼，最近的防具品質提升了不少。";
            }
        }
        load_object("/secure/quest_d.c")->accept_quest(tp, "crab_armour");
        return "如果你想幫忙，去水邊打些泥螃蟹，帶 2 塊『螃蟹殼』回來給我。";
    :));

    add_response(({ "report", "回報" }), (:
        object tp = this_player();
        if (tp->query_quest("crab_armour") && tp->query_quest("crab_armour")["status"] == "active") {
            if (load_object("/secure/quest_d.c")->complete_quest(tp, "crab_armour")) {
                return "太好了，這正是我需要的材料！這是給你的報酬。";
            }
        }
        return "等你湊齊了 2 塊螃蟹殼再來說吧。";
    :));
}
