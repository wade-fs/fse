// mudlib/area/tower/tower_2f.c - 法師塔二樓
#include "/include/config.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "Mage Tower Second Floor",
        "zh-TW": "法師塔二樓",
        "zh-CN": "法师塔二楼"
    ]));
    set_long(([
        "en": "You are on the second floor. It is filled with magical books.
",
        "zh-TW": "這裡是二樓，到處都是魔法書籍。
",
        "zh-CN": "这里是二楼，到处都是魔法书籍。
"
    ]));
    set_coordinate(0, 5, 1);
    set_is_outdoor(0);
    add_exit("down", "./tower_1f.c");
}
