// /rooms/triassic_plains.c  (史前文明 PC)
// 三疊紀荒原 — 唯一的房間，史前叢林邊緣
#include "/include/ansi.h"
inherit "/std/room";

private object proto_chicken;  // 場內的小型鳥類生物

void create() {
    ::create();
    set_short(YEL + "三疊紀荒原" + NOR);
    set_long(
        "你置身在一片詭異而炎熱的史前荒原。\n"
        "天空是橘紅色的，空氣中瀰漫著硫磺的氣味。\n"
        "腳下是紅棕色的砂岩地，偶有低矮的蕨類植物在石縫間掙扎求存。\n"
        "遠方可以看見巨大的蕨類森林輪廓，而你的周圍並不安全——\n"
        "這是三疊紀，人類根本不應該存在於此。\n"
    );
    // 目前無出口（極簡一房間）

    // 延遲初始化怪物，等 create 完成後再 clone
    call_out("init_monsters", 1);
}

void init_monsters() {
    proto_chicken = clone_object("/monsters/proto_chicken.c");
    if (proto_chicken) {
        proto_chicken->set_respawn_room("/rooms/triassic_plains.c");
        enter(proto_chicken);
    }
}

// 覆寫 describe，顯示怪物提示
string describe(object looker) {
    string base = ::describe(looker);
    return base;
}
