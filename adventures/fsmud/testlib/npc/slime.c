// npc/slime.c - 史萊姆（新手怪）
#include "/include/config.h"
inherit "/std/npc.c";

void create() {
    ::create();
    set_name(([ "en": "Slime", "zh-TW": "史萊姆", "zh-CN": "史莱姆" ]));
    set_short(([ "en": "a blue slime", "zh-TW": "一隻藍色史萊姆", "zh-CN": "一只蓝色史莱姆" ]));
    set_long(([
        "en": "This is a round blue slime. It looks weak but a bit disgusting.\n",
        "zh-TW": "這是一隻圓滾滾的藍色史萊姆，看起來弱不禁風，卻讓人有點噁心。\n",
        "zh-CN": "这是一只圆滚滚的蓝色史莱姆，看起来弱不禁风，却让人有点恶心。\n"
    ]));
    set_id(({"slime","史萊姆","藍色史萊姆"}));

    level    = 1;
    stat_str = 5;
    stat_dex = 4;
    stat_con = 6;
    recalc_stats();
    hp = max_hp;

    set_exp_reward(30);
    set_gold_reward(5);
    set_drop_list(({"/item/consumable/health_potion.c"}));
    set_harvest_data(([
        "file": "/item/material/slime_jelly.c",
        "chance": 90
    ]));
    set_respawn(30);
    set_aggro_msg("史萊姆發出「咕嚕咕嚕」的聲音，彈向你！\n");
}
