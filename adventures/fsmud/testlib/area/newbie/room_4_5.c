#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "$HIW$Adventurers Guild$NOR$",
        "zh-TW": "$HIW$冒險者公會$NOR$",
        "zh-CN": "$HIW$冒险者公会$NOR$"
    ]));
    set_long(([
        "en": "This is the guild hall of the Newbie Village. The ceiling is high, and the walls are decorated with various trophies and maps.\nSeveral adventurers are resting on the benches, and the atmosphere is both serious and lively.\n",
        "zh-TW": "這裡是新手村的公會大廳，天花板很高，四周掛著各種戰利品與地圖。\n幾名冒險者正坐在長椅上休息，氣氛顯得既嚴肅又熱鬧。\n",
        "zh-CN": "这里是新手村的公会大厅，天花板很高，四周挂着各种战利品与地图。\n几名冒险者正坐在长椅上休息，气氛显得既严肃又热闹。\n"
    ]));
    set_no_combat(1);
    set_has_guild(1);
    set_is_outdoor(0);
    set_coordinate(4, 5, 0);
    add_exit("east",  "./room_5_5.c");
    add_exit("west",  "./room_3_5.c");
    add_exit("north", "./room_4_6.c");
    add_exit("south", "./room_4_4.c"); // 連接中央廣場
    add_exit("lm",    "/area/lm/guild_world.c");

    // 產生老闆
    spawn_npc("/npc/guild_master.c");
}
