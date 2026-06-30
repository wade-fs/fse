// npc/creature/mole_beast.c - 掘土獸（地下，Lv4）
#include "/include/config.h"
inherit "/std/npc.c";

void create() {
    ::create();
    set_name(([ "en": "Mole Beast", "zh-TW": "掘土獸", "zh-CN": "掘土兽" ]));
    set_short(([ "en": "A ferocious mole beast", "zh-TW": "一頭凶猛的掘土獸", "zh-CN": "一头凶猛的掘土兽" ]));
    set_long(([ "en": "This mole beast resembles a giant mole. Its sharp claws leave deep furrows in the ground, and a pair of light-insensitive eyes glows faintly in the dark.\n", "zh-TW": "這頭掘土獸形似巨鼴，鋒利的爪子在地面留下深深的溝痕，一雙失去光感的眼睛在黑暗中發著微光。\n", "zh-CN": "这头掘土兽形似巨鼹，锋利的爪子在地面留下深深的沟痕，一双失去光感的眼睛在黑暗中发著微光。\n" ]));
    set_id(({"mole_beast", "掘土獸", "鼴鼠獸"}));

    level    = 4;
    stat_str = 16;
    stat_dex = 8;
    stat_con = 14;
    recalc_stats();
    hp = max_hp;

    set_habitat(HABITAT_UNDERGROUND);
    set_behaviour(BEHAV_FLEE);
    set_flee_hp_pct(25);
    set_aggro_range(2);
    set_aggro_msg("掘土獸從地裂中暴衝而出，發出震耳的嗥鳴！\n");
    set_special_atk("猛爪撕裂", 25);

    set_exp_reward(120);
    set_gold_reward(20);
    set_drop_list(({"/item/consumable/health_potion.c"}));
    set_respawn(90);
}
