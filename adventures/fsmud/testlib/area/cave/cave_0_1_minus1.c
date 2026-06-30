// mudlib/area/cave/cave_0_1_minus1.c - 洞穴內部
#include "/include/config.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "Deep Cave",
        "zh-TW": "洞穴深處",
        "zh-CN": "洞穴深处"
    ]));
    set_long(([
        "en": "The cave gets darker. Many bats are hanging on the ceiling.
",
        "zh-TW": "洞穴越來越深，天花板上掛著許多蝙蝠。
",
        "zh-CN": "洞穴越来越深，天花板上挂着许多蝙蝠。
"
    ]));
    set_coordinate(0, 1, -1);
    set_is_outdoor(0);
    add_exit("south", "./cave_0_0_minus1.c");
}

void reset() {
    ::reset();
    spawn_npc("/npc/creature/bat.c");
}
