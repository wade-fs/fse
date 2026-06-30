// npc/creature/mud_crab.c - 泥蟹（水中/地面，Lv2）
#include "/include/config.h"
inherit "/std/npc.c";

void create() {
    ::create();
    set_name(([ "en": "Mud Crab", "zh-TW": "泥蟹", "zh-CN": "泥蟹" ]));
    set_short(([ "en": "A large scuttling mud crab", "zh-TW": "一隻橫行的大泥蟹", "zh-CN": "一只横行的大泥蟹" ]));
    set_long(([ "en": "This mud crab's shell is covered in dried mud. It waves its two large pincers constantly as it scuttles about recklessly.\n", "zh-TW": "這隻泥蟹外殼覆滿乾泥，兩把大螯不停揮舞，橫衝直撞地爬行著。\n", "zh-CN": "这只泥蟹外壳覆满干泥，两把大螯不停挥舞，横冲直撞地爬行著。\n" ]));
    set_id(({"mud_crab", "泥蟹", "蟹", "螃蟹"}));

    level    = 2;
    stat_str = 12;
    stat_dex = 5;
    stat_con = 14;
    recalc_stats();
    hp = max_hp;

    set_habitat(HABITAT_WATER);
    set_behaviour(BEHAV_AGGRESSIVE);
    set_aggro_range(1);
    set_aggro_msg("泥蟹揮舞著大螯，橫衝過來！\n");

    set_exp_reward(60);
    set_gold_reward(10);
    set_drop_list(({"/item/consumable/health_potion.c"}));
    set_harvest_data(([
        "file": "/item/material/crab_shell.c",
        "chance": 75
    ]));
    set_respawn(50);
}
