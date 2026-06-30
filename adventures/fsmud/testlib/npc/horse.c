// /npc/horse.c
#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/npc.c";

void create() {
    ::create();
    set_name(([ "en": "Wild Horse", "zh-TW": "野馬", "zh-CN": "野马" ]));
    set_short(([ "en": "a sturdy wild horse", "zh-TW": "一匹健壯的野馬", "zh-CN": "一匹健壮的野马" ]));
    set_long(([
        "en": "This is a wild horse with a bright coat and well-developed muscles. Its eyes reflect a hint of wildness.\n",
        "zh-TW": "這是一匹毛色亮麗的野馬，肌肉發達，眼神中透著一絲野性。\n",
        "zh-CN": "这是一匹毛色亮丽的野马，肌肉发达，眼神中透著一丝野性。\n"
    ]));
    set_id(({"horse", "wild horse", "野馬", "馬"}));

    level    = 5;
    stat_str = 15;
    stat_dex = 20;
    stat_con = 15;
    recalc_stats();
    hp = max_hp;

    set_tamable(1); // 🚀 設定為可馴服
    set_respawn(120);
}
