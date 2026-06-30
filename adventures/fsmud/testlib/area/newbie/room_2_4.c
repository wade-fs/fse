#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "East-West Avenue",
        "zh-TW": "東西大道",
        "zh-CN": "东西大道"
    ]));
    set_long(([
        "en": "The wide street is lined with various small shops.\n",
        "zh-TW": "寬闊的街道兩旁開滿了各式小店。\n",
        "zh-CN": "宽阔的街道两旁开满了各式小店。\n"
    ]));
    set_coordinate(2, 4, 0);
    add_exit("west", "./room_1_4.c");
    add_exit("east", "./room_3_4.c");
    add_exit("south", "./room_2_3.c");
    add_exit("north", "./room_2_5.c");
}
