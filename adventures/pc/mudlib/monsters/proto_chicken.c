// /monsters/proto_chicken.c  (史前文明 PC)
// 始祖小雞類 (Eoraptor-like) — 三疊紀最小的雙足肉食性生物
// 牠是玩家從 Level 0 → Level 1 的第一個挑戰
#include "/include/ansi.h"
inherit "/std/monster";

void create() {
    ::create();
    initialize_from_yaml("/content/monsters/proto_chicken.yaml");
}
