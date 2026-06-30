// /npc/mage_teacher.c
#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/npc.c";

void create() {
    ::create();
    set_name(([ "en": "Saruman", "zh-TW": "導師 薩魯曼", "zh-CN": "导师 萨鲁曼" ]));
    set_short(([
        "en": "$HIC$Senior Magic Mentor$NOR$" + " Saruman",
        "zh-TW": "$HIC$資深魔法導師$NOR$" + " 薩魯曼",
        "zh-CN": "$HIC$资深魔法导师$NOR$" + " 萨鲁曼"
    ]));
    set_long(([
        "en": "He is an old man with a white beard reaching down to his waist. His eyes are deep, as if they can insight into all the truths of the world.\n",
        "zh-TW": "他是一位白鬍子垂到腰間的老者，雙眼深邃，彷彿能洞察世間一切真理。\n",
        "zh-CN": "他是一位白胡子垂到腰间的老者，双眼深邃，仿佛能洞察世间一切真理。\n"
    ]));
    set_id(({"saruman", "mage", "teacher", "導師"}));
    
    set_level(40);
    set_stat("int", 35);
    set_stat("wis", 30);
    recalc_stats();
    hp = max_hp;

    set_skill_to_teach("magic", 50);
    set_skill_to_teach("fireball", 40);

    add_response(({ "join", "加入" }), (:
        object tp = this_player();
        if (load_object("/secure/guild_d.c")->join_guild(tp, "mage")) {
            return "明智的選擇。魔法的奧秘正等待著你去發掘。";
        }
        return "如果你還沒準備好，就別來浪費我的時間。";
    :));
    
    add_response(({ "magic", "魔法" }), "魔法不是戲法，它是對宇宙規律的解構與重組。");
}
