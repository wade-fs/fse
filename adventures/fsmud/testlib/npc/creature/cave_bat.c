// npc/creature/cave_bat.c - 洞窟蝙蝠（洞穴，Lv1）
#include "/include/config.h"
inherit "/std/npc.c";

void create() {
    ::create();
    set_name(([ "en": "Cave Bat", "zh-TW": "洞窟蝙蝠", "zh-CN": "洞窟蝙蝠" ]));
    set_short(([ "en": "A black-winged bat", "zh-TW": "一隻黑翼蝙蝠", "zh-CN": "一只黑翼蝙蝠" ]));
    set_long(([ "en": "This bat hangs in dark corners. Once disturbed, it darts wildly through the air, emitting an uncomfortable screech.\n", "zh-TW": "這隻蝙蝠掛在陰暗角落，一被驚動就在空中亂竄，發出令人不舒服的尖嘯。\n", "zh-CN": "这只蝙蝠挂在阴暗角落，一被惊动就在空中乱窜，发出令人不舒服的尖啸。\n" ]));
    set_id(({"cave_bat", "蝙蝠", "洞窟蝙蝠"}));

    level    = 1;
    stat_str = 4;
    stat_dex = 16;
    stat_con = 5;
    recalc_stats();
    hp = max_hp;

    set_habitat(HABITAT_CAVE);
    set_behaviour(BEHAV_FLEE);
    set_flee_hp_pct(50);
    set_aggro_msg("蝙蝠被驚動，朝你猛衝！\n");

    set_exp_reward(25);
    set_gold_reward(3);
    set_respawn(30);
}
