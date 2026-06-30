#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "$HIB$Kevin's Armoury$NOR$",
        "zh-TW": "$HIB$凱文的防具店$NOR$",
        "zh-CN": "$HIB$凯文的防具店$NOR$"
    ]));
    set_long(([
        "en": "Various types of leather and chain mail are on display in the shop, emitting the smell of leather and metal. Several ornately decorated shields hang on the walls.\n",
        "zh-TW": "店內陳列著各種皮甲與鎖子甲，散發著皮革與金屬的味道。牆上掛著幾面裝飾華麗的盾牌。\n",
        "zh-CN": "店内陈列着各种皮甲与锁子甲，散发着皮革与金属的味道。墙上挂着几面装饰华丽的盾牌。\n"
    ]));
    set_no_combat(1);
    set_has_shop(1);
    set_is_outdoor(0);
    set_coordinate(5, 4, 0);
    add_exit("west", "./room_4_4.c");
    add_exit("east", "./room_6_4.c");

    // 產生老闆
    spawn_npc("/npc/armourer.c");
}
