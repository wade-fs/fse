#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "$MAG$Drunken Dream Tavern$NOR$",
        "zh-TW": "$MAG$醉夢酒館$NOR$",
        "zh-CN": "$MAG$醉梦酒馆$NOR$"
    ]));
    set_long(([
        "en": "The tavern is bustling with noise and the smell of cheap ale. Several rough wooden tables are filled with boasting adventurers.\nBuck, the owner, is busy wiping glasses at the bar.\n",
        "zh-TW": "酒館內人聲鼎沸，充滿了廉價麥酒的味道。幾張粗糙的木桌旁坐滿了吹牛的冒險者。\n老闆巴克正忙著擦拭吧台上的杯子。\n",
        "zh-CN": "酒馆内人声鼎沸，充满了廉价麦酒的味道。几张粗糙的木桌旁坐满了吹牛的冒险者。\n老板巴克正忙着擦拭吧台上的杯子。\n"
    ]));
    set_no_combat(1);
    set_has_tavern(1);
    set_is_outdoor(0);
    set_coordinate(4, 3, 0);
    add_exit("north", "./room_4_4.c");
    add_exit("south", "./room_4_2.c");
    
    // 產生老闆
    spawn_npc("/npc/bartender.c");
}
