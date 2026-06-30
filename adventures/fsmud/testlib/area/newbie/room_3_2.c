#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "Town Street",
        "zh-TW": "城鎮街道",
        "zh-CN": "城镇街道"
    ]));
    set_long(([
        "en": "This is a quiet street, with occasional residents passing by.\n",
        "zh-TW": "這裡是一條安靜的街道，偶爾有居民走過。\n",
        "zh-CN": "这里是一条安静的街道，偶尔有居民走过。\n"
    ]));
    set_coordinate(3, 2, 0);
    add_exit("west", "./room_2_2.c");
    add_exit("east", "./room_4_2.c");
    add_exit("south", "./room_3_1.c");
    add_exit("north", "./room_3_3.c");
}
