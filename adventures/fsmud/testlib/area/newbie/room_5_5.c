#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "$HIM$Town General Store$NOR$",
        "zh-TW": "$HIM$城鎮雜貨鋪$NOR$",
        "zh-CN": "$HIM$城镇杂货铺$NOR$"
    ]));
    set_long(([
        "en": "This is a small shop filled with a dazzling array of items. The shelves are piled with all kinds of sundries: torches, ropes, rations, and some unknown small parts. It sells various essentials for travel and adventure.\n",
        "zh-TW": "這是一間琳瑯滿目的小店，貨架上堆滿了各式各樣的雜物：火把、繩索、乾糧、還有一些不知名的小零件。這裡販賣著各種旅行冒險必備的小東西。\n",
        "zh-CN": "这是一间琳琅满目的小店，货架上堆满了各式各样的杂物：火把、绳索、干粮、还有一些不知名的小零件。这里贩卖着各种旅行冒险必备的小东西。\n"
    ]));
    set_no_combat(1);
    set_coordinate(5, 5, 0);
    add_exit("west", "./room_4_5.c");
    add_exit("south", "./room_5_4.c");
}
