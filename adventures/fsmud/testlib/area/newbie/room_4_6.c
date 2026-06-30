// /area/newbie/room_4_6.c
#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "$HIC$Magic Lab$NOR$",
        "zh-TW": "$HIC$魔法研究室$NOR$",
        "zh-CN": "$HIC$魔法研究室$NOR$"
    ]));
    set_long(([
        "en": "The air here is filled with a faint aura of arcane energy. The bookshelves around are piled high with thick magic books.\nMaster Saruman is sitting quietly behind the desk, studying an ancient scroll.\n",
        "zh-TW": "這裡瀰漫著淡淡的奧法能量氣息。四周的書架上堆滿了厚重的魔法書籍。\n導師薩魯曼正靜靜地坐在桌後，研究著一份古老的捲軸。\n",
        "zh-CN": "这里弥漫着淡淡的奥法能量气息。四周的书架上堆满了厚重的魔法书籍。\n导师萨鲁曼正静静地坐在桌后，研究着一份古老的卷轴。\n"
    ]));
    set_no_combat(1);
    set_has_lab(1);
    set_is_outdoor(0);
    set_coordinate(4, 6, 0);
    add_exit("south", "./room_4_5.c");
    add_exit("north", "./room_4_7.c");
	add_exit("lm",    "/area/lm/world.c");

    spawn_npc("/npc/mage_teacher.c");
}
