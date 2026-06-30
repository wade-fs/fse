// /area/newbie/room_6_6.c
#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "$GRN$Green Meadow$NOR$",
        "zh-TW": "$GRN$翠綠草原$NOR$",
        "zh-CN": "$GRN$翠绿草原$NOR$"
    ]));
    set_long(([
        "en": "This is a vast meadow. The breeze blows, and the grass waves. In the distance, several wild horses can be seen grazing leisurely.\n",
        "zh-TW": "這裡是一片廣闊的草原，微風拂過，草浪起伏。\n遠處可以看到幾匹野馬正在悠閒地吃草。\n",
        "zh-CN": "这里是一片广阔的草原，微风拂过，草浪起伏。\n远处可以看到几匹野马正在悠闲地吃草。\n"
    ]));
    set_coordinate(6, 6, 0);
    add_exit("west", "./room_5_6.c");
    add_exit("south", "./room_6_5.c");

    spawn_npc("/npc/horse.c");
}
