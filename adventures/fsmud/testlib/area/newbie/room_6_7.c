#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "Back Garden",
        "zh-TW": "後花園",
        "zh-CN": "后花园"
    ]));
    set_long(([
        "en": "A quiet corner at the edge of the town, filled with strange flowers and plants.\n",
        "zh-TW": "城鎮邊緣安靜的角落，種滿了奇異的花草。\n",
        "zh-CN": "城镇边缘安静的角落，种满了奇异的花草。\n"
    ]));
    set_coordinate(6, 7, 0);
    add_exit("west", "./room_5_7.c");
    add_exit("east", "./room_7_7.c");
    add_exit("south", "./room_6_6.c");
}
