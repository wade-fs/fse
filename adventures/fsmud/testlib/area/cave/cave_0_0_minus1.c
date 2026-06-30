// mudlib/area/cave/cave_0_0_minus1.c - 洞穴入口
#include "/include/config.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "Cave Entrance",
        "zh-TW": "洞穴入口",
        "zh-CN": "洞穴入口"
    ]));
    set_long(([
        "en": "It is dark and damp here. You hear sounds of fluttering.
",
        "zh-TW": "這裡陰暗潮濕，你聽到了拍動翅膀的聲音。
",
        "zh-CN": "这里阴暗潮湿，你听到了拍动翅膀的声音。
"
    ]));
    set_coordinate(0, 0, -1);
    set_is_outdoor(0);
    
    // 這裡設一個出口到地面（z=0）
    add_exit("up", "../newbie/room_0_0.c");
    add_exit("north", "./cave_0_1_minus1.c");
}

void reset() {
    ::reset();
    spawn_npc("/npc/creature/bat.c");
}
