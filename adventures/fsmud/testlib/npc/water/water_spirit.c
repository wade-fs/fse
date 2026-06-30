// npc/water/water_spirit.c - 水精靈
#include "/include/config.h"
inherit "/std/npc.c";

void create() {
    ::create();
    set_name(([ "en": "Water Spirit", "zh-TW": "水精靈", "zh-CN": "水精灵" ]));
    set_short(([ "en": "a water spirit", "zh-TW": "一隻水精靈", "zh-CN": "一只水精灵" ]));
    set_long(([
        "en": "A translucent spirit formed by water.
",
        "zh-TW": "一隻由水構成的半透明精靈。
",
        "zh-CN": "一只由水构成的半透明精灵。
"
    ]));
    set_id(({"spirit","water spirit","水精靈"}));

    level    = 3;
    stat_str = 4;
    stat_dex = 10;
    stat_con = 8;
    recalc_stats();
    hp = max_hp;

    set_exp_reward(60);
    set_gold_reward(10);
    set_respawn(40);
    set_aggro_msg("水精靈噴出一道水柱擊向你！
");
}
