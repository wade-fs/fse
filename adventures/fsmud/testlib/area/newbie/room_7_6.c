#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "Abandoned Plaza",
        "zh-TW": "廢棄廣場",
        "zh-CN": "废弃广场"
    ]));
    set_long(([
        "en": "This is an abandoned open space in the northeast corner of the town. The ground is uneven, and several broken stone pillars are scattered around. There are traces of ancient runes on the pillars, and there is an indescribable sense of pressure in the air.\n",
        "zh-TW": "這是城鎮東北角的一塊廢棄空地，地面凹凸不平，幾根斷裂的石柱東倒西歪。石柱上有古老符文的痕跡，空氣中有股說不出來的壓迫感。\n",
        "zh-CN": "这是城镇东北角的一块废弃空地，地面凹凸不平，几根断裂的石柱东倒西歪。石柱上有古老符文的痕迹，空气中有股说不出来的压迫感。\n"
    ]));
    set_coordinate(7, 6, 0);
    add_exit("north", "./room_7_7.c");
    add_exit("west",  "./room_6_6.c");
    add_exit("south", "./room_7_5.c");

    spawn_npc("/npc/creature/stone_golem.c");
}
