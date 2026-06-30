#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/npc.c";

void create() {
    ::create();
    set_name(([ "en": "Aela the Herbalist", "zh-TW": "藥劑師 艾拉", "zh-CN": "药剂师 艾拉" ]));
    set_short(([ "en": "$GRN$Elegant Elf Herbalist$NOR$" + " Aela", "zh-TW": "$GRN$優雅的精靈藥劑師$NOR$" + " 艾拉", "zh-CN": "$GRN$优雅的精灵药剂师$NOR$" + " 艾拉" ]));
    set_long(([
        "en": "This elf woman has long golden hair, and every move she makes is full of elegance.\nShe is carefully concocting a potion that emits a faint glow.\n",
        "zh-TW": "這位精靈女性有著一頭金色的長髮，舉手投足間充滿了優雅。\n她正在細心地調配一種散發著微光的藥劑。\n",
        "zh-CN": "这位精灵女性有着一头金色的长发，举手投足间充满了优雅。\n她正在细心地调配一种散发着微光的药剂。\n"
    ]));
    set_id(({"aela", "herbalist", "elf", "藥劑師"}));
    set_race("elf");
    set_level(20);
    set_stat(STAT_INT, 25);
    set_stat(STAT_WIS, 25);
    recalc_stats();
    hp = max_hp;

    // 🚀 新增：重生與走動
    set_respawn(60);      // 60秒重生
    set_wander_chance(3); // 3% 機率走動
    set_move_range(2);    // 最多離家 2 格

    // 🚀 新增：傳授技能
    set_skill_to_teach("alchemy", 30);

    add_response(({ "藥水", "potion", "藥劑" }), ({
        "如果你需要治療，我的藥水是全村最好的。",
        "調配藥劑需要極大的耐心與專注。",
        "雖然味道可能不太好，但效果絕對有保證。"
    }));
    add_response(({ "藥草", "herb" }), ({
        "我這裡有一些常見的藥草，如果你能幫我採些新鮮的野花回來就太好了。",
        "藥草的力量源自大自然的恩賜。"
    }));

    // 🚀 新增：任務互動
    add_response(({ "quest", "任務" }), (:
        object tp = this_player();
        if (tp->query_quest("slime_medicine")) {
            if (tp->query_quest("slime_medicine")["status"] == "active") {
                return "煉金術需要精確的配比，我還需要 5 團史萊姆黏液。";
            } else {
                return "感謝你的幫助，那些黏液幫了大忙。";
            }
        }
        load_object("/secure/quest_d.c")->accept_quest(tp, "slime_medicine");
        return "冒險者，你能幫我收集 5 團『史萊姆黏液』嗎？這對製作恢復藥水很重要。";
    :));

    add_response(({ "report", "回報" }), (:
        object tp = this_player();
        if (tp->query_quest("slime_medicine") && tp->query_quest("slime_medicine")["status"] == "active") {
            if (load_object("/secure/quest_d.c")->complete_quest(tp, "slime_medicine")) {
                return "非常感謝！這些黏液看起來品質很不錯。";
            }
        }
        return "等你收集到 5 團史萊姆黏液後再回來找我吧。";
    :));
}
