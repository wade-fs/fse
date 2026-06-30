#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "$HIY$Blacksmith$NOR$",
        "zh-TW": "$HIY$鐵匠鋪$NOR$",
        "zh-CN": "$HIY$铁匠铺$NOR$"
    ]));
    set_long(([
        "en": "This is a steaming workshop with a blazing fire. Various iron-forged swords, axes, and shields are displayed on wall racks. Some discarded metal residues are scattered on the floor.\n",
        "zh-TW": "這是一個熱氣騰騰的作坊，爐火熊熊燃燒。牆上的掛架上展示著各種鋼鐵鑄造的長劍、斧頭與盾牌。地板上散落著一些廢棄的金屬殘渣。\n",
        "zh-CN": "这是一个热气腾腾的作坊，炉火熊熊燃烧。墙上的挂架上展示着各种钢铁铸造的长剑、斧头与盾牌。地板上散落着一些废弃的金属残渣。\n"
    ]));
    set_no_combat(1);
    set_has_forge(1);
    set_is_outdoor(0);
    set_coordinate(3, 4, 0);
    add_exit("east", "./room_4_4.c");
    add_exit("west", "./room_2_4.c");
    
    // 產生老闆
    spawn_npc("/npc/blacksmith.c");
}
