#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "North-South Avenue",
        "zh-TW": "南北大道",
        "zh-CN": "南北大道"
    ]));
    set_long(([
        "en": "A neat stone path that runs through the north and south of the town.\n",
        "zh-TW": "一條整潔的石板路，貫穿城鎮南北。\n",
        "zh-CN": "一条整洁的石板路，贯穿城镇南北。\n"
    ]));
    set_coordinate(4, 1, 0);
    add_exit("west", "./room_3_1.c");
    add_exit("east", "./room_5_1.c");
    add_exit("south", "./room_4_0.c");
    add_exit("north", "./room_4_2.c");
}
