#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "At the Foot of the City Wall",
        "zh-TW": "城牆腳下",
        "zh-CN": "城墙脚下"
    ]));
    set_long(([
        "en": "At the foot of the old stone wall on the west side of town, the ground has many bulges and cracks, and the soil here has been overturned by something from underground. Stepping on the soft soil, you can occasionally feel the ground vibrating slightly.\n",
        "zh-TW": "城鎮西側古老石牆的腳下，地面有多處隆起和裂縫，這裡的土壤被什麼東西從地底掀翻了。踩在鬆軟的泥土上，偶爾能感受到地面微微震動。\n",
        "zh-CN": "城镇西侧古老石墙的脚下，地面有多处隆起和裂缝，这里的土壤被什么东西从地底掀翻了。踩在松软的泥土上，偶尔能感受到地面微微震动。\n"
    ]));
    set_coordinate(0, 2, 0);
    add_exit("east",  "./room_1_2.c");
    add_exit("north", "./room_0_3.c");
    add_exit("south", "./room_0_1.c");

    spawn_npc("/npc/creature/mole_beast.c");
}
