// npc/creature/stone_golem.c - 石頭傀儡（陸地，Lv5，巡邏）
#include "/include/config.h"
inherit "/std/npc.c";

void create() {
    ::create();
    set_name(([ "en": "Stone Golem", "zh-TW": "石頭傀儡", "zh-CN": "石头傀儡" ]));
    set_short(([ "en": "A slowly moving stone golem", "zh-TW": "一座緩步移動的石頭傀儡", "zh-CN": "一座缓步移动的石头傀儡" ]));
    set_long(([ "en": "Driven by ancient magic, this stone golem silently patrols the ruins. Each of its steps makes the ground tremble slightly.\n", "zh-TW": "這座由古代魔法驅動的石頭傀儡，沉默地在廢墟間巡視，每一步都讓地面微微震動。\n", "zh-CN": "这座由古代魔法驱动的石头傀儡，沉默地在废墟间巡视，每一步都让地面微微震动。\n" ]));
    set_id(({"stone_golem", "石頭傀儡", "傀儡", "石傀儡"}));

    level    = 5;
    stat_str = 20;
    stat_dex = 4;
    stat_con = 20;
    recalc_stats();
    hp = max_hp;

    set_habitat(HABITAT_LAND);
    set_behaviour(BEHAV_PATROL);
    set_aggro_range(1);
    set_patrol_rooms(({
        "/area/newbie/room_6_6.c",
        "/area/newbie/room_6_5.c",
        "/area/newbie/room_7_6.c",
        "/area/newbie/room_7_5.c"
    }));
    set_aggro_msg("石頭傀儡的眼睛猩紅發光，轉向你！\n");
    set_special_atk("石錘重擊", 20);

    set_exp_reward(200);
    set_gold_reward(30);
    set_drop_list(({"/item/armour/iron_helm.c"}));
    set_respawn(180);
}
