#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "$HIC$Recruit Training Ground$NOR$",
        "zh-TW": "$HIC$新兵訓練場$NOR$",
        "zh-CN": "$HIC$新兵训练场$NOR$"
    ]));
    set_long(([
        "en": "This is a level open space with the ground trodden firm. Some wooden dummies and practice wooden swords are placed around.\nThe training instructor is here guiding recruits on how to perform basic combat.\n",
        "zh-TW": "這是一塊平整的空地，地面被踩得很實。四周擺放著一些木質的人偶和練習用的木劍。\n訓練教官正在這裡指導新兵們如何進行基礎的戰鬥。\n",
        "zh-CN": "这是一块平整的空地，地面被踩得很实。四周摆放着一些木质的人偶和练习用的木剑。\n训练教官正在这里指导新兵们如何进行基础的战斗。\n"
    ]));
    set_no_combat(1);
    set_is_outdoor(0);
    set_coordinate(1, 1, 0);
    add_exit("east",  "./room_2_1.c");
    add_exit("west",  "./room_0_1.c");
    add_exit("north", "./room_1_2.c");
    add_exit("south", "./room_1_0.c");

    // 產生老闆
    spawn_npc("/npc/trainer.c");
}
