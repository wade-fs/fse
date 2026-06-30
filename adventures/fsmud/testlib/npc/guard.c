#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/npc.c";

void create() {
    ::create();
    set_name(([ "en": "Orc Guard Grog", "zh-TW": "守衛 葛羅格", "zh-CN": "守卫 葛罗格" ]));
    set_short(([ "en": "$RED$Strong Orc Guard$NOR$" + " Grog", "zh-TW": "$RED$強壯的獸人守衛$NOR$" + " 葛羅格", "zh-CN": "$RED$强壮的兽人守卫$NOR$" + " 葛罗格" ]));
    set_long(([ 
        "en": "This orc has rough skin, exposed tusks, and holds a heavy giant axe in his hand.\nAlthough he looks fierce, he is faithfully performing his patrol mission.\n", 
        "zh-TW": "這個獸人皮膚粗糙，獠牙外露，手裡握著一柄沉重的巨斧。\n雖然看起來很兇，但他正忠實地執行著巡邏任務。\n",
        "zh-CN": "这个兽人皮肤粗糙，獠牙外露，手里握着一柄沉重的巨斧。\n虽然看起来很凶，但他正忠实地执行着巡逻任务。\n"
    ]));
    set_id(({"grog", "guard", "orc", "守衛"}));
    set_race("orc");
    set_level(15);
    set_stat(STAT_STR, 28);
    set_stat(STAT_CON, 22);
    recalc_stats();
    hp = max_hp;

    // 🚀 新增：重生與走動
    set_respawn(30);      // 30秒重生 (守衛補位要快)
    set_wander_chance(5); // 5% 機率走動
    set_move_range(5);    // 守衛巡邏範圍較大
}
