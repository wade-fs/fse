// npc/creature/earth_worm.c - 巨型土蚯蚓（地下怪，Lv3）
#include "/include/config.h"
inherit "/std/npc.c";

void create() {
    ::create();
    set_name(([ "en": "Giant Earthworm", "zh-TW": "巨型土蚯蚓", "zh-CN": "巨型土蚯蚓" ]));
    set_short(([ "en": "A giant earthworm emerging from the ground", "zh-TW": "一條從地底鑽出的巨型土蚯蚓", "zh-CN": "一条从地底钻出的巨型土蚯蚓" ]));
    set_long(([ "en": "This earthworm is as thick as an adult's arm, emerging from damp soil with mucus dripping onto the ground.\n", "zh-TW": "這條土蚯蚓足有成人手臂般粗，從潮濕的泥土中鑽出，黏液滴落在地面上。\n", "zh-CN": "这条土蚯蚓足有成人手臂般粗，从潮湿的泥土中钻出，黏液滴落在地面上。\n" ]));
    set_id(({"earth_worm", "土蚯蚓", "蚯蚓", "巨型蚯蚓"}));

    level    = 3;
    stat_str = 13;
    stat_dex = 4;
    stat_con = 16;
    recalc_stats();
    hp = max_hp/2;

    set_habitat(HABITAT_UNDERGROUND);
    set_behaviour(BEHAV_AGGRESSIVE);
    set_aggro_range(1);
    set_aggro_msg("地面突然隆起，一條巨大的土蚯蚓破土而出！\n");
    set_special_atk("地震衝擊", 15);

    set_exp_reward(80);
    set_gold_reward(12);
    set_respawn(60);
}
