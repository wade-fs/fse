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
        "en": "A wide street lined with various small shops.\n",
        "zh-TW": "寬闊的街道兩旁開滿了各式小店。\n",
        "zh-CN": "宽阔的街道两旁开满了各式小店。\n"
    ]));
    set_coordinate(6, 4, 0);
    add_exit("west", "./room_5_4.c");
    add_exit("east", "./room_7_4.c");
    add_exit("south", "./room_6_3.c");
    add_exit("north", "./room_6_5.c");
}
