#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "Residential Area",
        "zh-TW": "平民區",
        "zh-CN": "平民区"
    ]));
    set_long(([
        "en": "Rows of neat stone houses stand here.\n",
        "zh-TW": "這裡是一排排整齊的石造民居。\n",
        "zh-CN": "这里是一排排整齐的石造民居。\n"
    ]));
    set_coordinate(2, 0, 0);
    add_exit("west", "./room_1_0.c");
    add_exit("east", "./room_3_0.c");
    add_exit("north", "./room_2_1.c");
}
