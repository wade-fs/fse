#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "North Square Clearing",
        "zh-TW": "廣場北空地",
        "zh-CN": "广场北空地"
    ]));
    set_long(([
        "en": "An open space to the north of the square, where several old trees have branches slanting across. Looking up, a massive raptor circles in the sky, its shadow gliding across the ground — one shouldn't be careless.\n",
        "zh-TW": "廣場北方一塊開闊的空地，幾棵老樹枝幹橫斜。仰頭望去，空中盤旋著一隻碩大的猛禽，牠的影子在地面掠過——讓人不敢大意。\n",
        "zh-CN": "广场北方一块开阔的空地，几棵老树枝干横斜。仰头望去，空中盘旋着一只硕大的猛禽，它的影子在地面掠过——让人不敢大意。\n"
    ]));
    set_coordinate(2, 6, 0);
    add_exit("west",  "./room_1_6.c");
    add_exit("east",  "./room_3_6.c");
    add_exit("north", "./room_2_7.c");
    add_exit("south", "./room_2_5.c");

    spawn_npc("/npc/creature/sky_hawk.c");
}
