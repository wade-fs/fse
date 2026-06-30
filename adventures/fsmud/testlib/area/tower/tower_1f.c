// mudlib/area/tower/tower_1f.c - 法師塔一樓
#include "/include/config.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "Mage Tower Entrance",
        "zh-TW": "法師塔入口",
        "zh-CN": "法师塔入口"
    ]));
    set_long(([
        "en": "You are at the entrance of a mysterious mage tower.
",
        "zh-TW": "你來到了一座神秘法師塔的入口。
",
        "zh-CN": "你来到了一座神秘法师塔的入口。
"
    ]));
    set_coordinate(0, 5, 0);
    set_is_outdoor(0);
    
    add_exit("south", "../newbie/room_0_1.c");
    add_exit("up", "./tower_2f.c");
}
