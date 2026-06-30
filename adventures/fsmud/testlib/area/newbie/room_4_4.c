#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/room.c";

void create() {
    ::create();
    set_short(([
        "en": "Central Square",
        "zh-TW": "$CYN$中央廣場$NOR$",
        "zh-CN": "$CYN$中央广场$NOR$"
    ]));
    set_long(([
        "en": "This is the center of the Newbie Village. The spacious square is paved with clean flagstones. A giant fountain stands in the middle, its water glistening in the sunlight.",
        "zh-TW": "這裡是新手村的正中央，寬闊的廣場由潔淨的石板鋪成。廣場中央矗立著一座巨大的噴泉，水花在陽光下閃爍著七彩的光芒。噴泉旁有幾隻野鴿子在踱步，偶爾有哥布林的叫聲從東邊的林子傳來。這裡也是許多冒險者出發與休憩的起點。\n",
        "zh-CN": "这里是新手村的正中央，宽阔的广场由洁净的石板铺成。广场中央矗立著一座巨大的喷泉，水花在阳光下闪烁著七彩的光芒。喷泉旁有几隻野鸽子在踱步，偶尔有哥布林的叫声从东边的林子传来。这里也是许多冒险者出发与休憩的起点。\n"
    ]));
    set_no_combat(1);
    set_coordinate(4, 4, 0);
    add_exit("north", "./room_4_5.c");
    add_exit("south", "./room_4_3.c");
    add_exit("west",  "./room_3_4.c");
    add_exit("east",  "./room_5_4.c");
    add_exit("lm",    "/area/lm/center_world.c");

    // 廣場守衛
    spawn_npc("/npc/guard.c");

    // 🚀 新增：裝飾性雕像
    spawn_npc("/item/newbie_statue.c");

    // 廣場上的野生風精靈（白天出現，被動）
    spawn_npc("/npc/creature/wind_sprite.c");
}
