#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "Back Garden",
        "zh-TW": "後花園",
        "zh-CN": "后花园"
    ]));
    set_long(([
        "en": "A quiet corner at the edge of the town, filled with strange flowers and plants. Occasionally, the sound of bat wings can be heard from the stone crevices near the wall, and a low panting sound comes from the bushes. This place is not as calm as it seems.\n",
        "zh-TW": "城鎮邊緣安靜的角落，種滿了奇異的花草。靠近牆角的石縫間偶爾有蝙蝠的翅膀聲，花叢中傳來低沉的喘息聲，這裡並不像表面看起來那麼平靜。\n",
        "zh-CN": "城镇边缘安静的角落，种满了奇异的花草。靠近墙角的石缝间偶尔有蝙蝠的翅膀声，花丛中传来低沉的喘息声，这里并不像表面看起来那么平静。\n"
    ]));
    set_coordinate(7, 7, 0);
    add_exit("west",  "./room_6_7.c");
    add_exit("south", "./room_7_6.c");

    spawn_npc("/npc/creature/cave_bat.c");
}
