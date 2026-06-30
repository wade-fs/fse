#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "Town West Gate",
        "zh-TW": "城鎮西大門",
        "zh-CN": "城镇西大门"
    ]));
    set_long(([
        "en": "This is the western entrance to the town, with heavy wooden gates that are usually kept open. This is the first line of defense against external beast threats.\n",
        "zh-TW": "這是城鎮的西側入口，厚重的木門平時敞開著。這裡是防禦外部野獸威脅的第一線。\n",
        "zh-CN": "这是城镇的西侧入口，厚重的木门平时敞开着。这里是防御外部野兽威胁的第一线。\n"
    ]));
    set_coordinate(0, 4, 0);
    add_exit("east", "./room_1_4.c");
    
    spawn_npc("/npc/guard.c");
}
