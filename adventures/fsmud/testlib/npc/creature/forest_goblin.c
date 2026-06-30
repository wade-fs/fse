// npc/creature/forest_goblin.c - 森林哥布林（陸地，Lv2，主動攻擊）
#include "/include/config.h"
inherit "/std/npc.c";

void create() {
    ::create();
    set_name(([ "en": "Forest Goblin", "zh-TW": "森林哥布林", "zh-CN": "森林哥布林" ]));
    set_short(([ "en": "A chattering goblin", "zh-TW": "一隻嘰嘰喳喳的哥布林", "zh-CN": "一只叽叽喳喳的哥布林" ]));
    set_long(([ "en": "This short, green-skinned goblin brandishes a rusty scimitar, shouting in a language you don't understand with a piercing voice.\n", "zh-TW": "這隻矮小的綠皮哥布林揮舞著生鏽的彎刀，用刺耳的嗓音喊著你聽不懂的語言。\n", "zh-CN": "这只矮小的绿皮哥布林挥舞著生锈的弯刀，用刺耳的嗓音喊著你听不懂的语言。\n" ]));
    set_id(({"goblin", "哥布林", "forest_goblin", "森林哥布林"}));

    level    = 2;
    stat_str = 9;
    stat_dex = 12;
    stat_con = 8;
    recalc_stats();
    hp = max_hp;

    set_habitat(HABITAT_LAND);
    set_behaviour(BEHAV_AGGRESSIVE);
    set_aggro_range(2);
    set_aggro_msg("哥布林尖叫一聲，揮刀衝向你！\n");

    set_exp_reward(65);
    set_gold_reward(12);
    set_drop_list(({"/item/weapon/short_sword.c"}));
    set_respawn(45);
}
