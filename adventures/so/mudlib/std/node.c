// /std/node.c (蜀山遊記 SO 節點類別)
// 繼承 fse_room，覆寫 SO 專屬的修仙副作用。
#include "/runtime/include/ansi.h"
inherit "/std/fse_room";

void create() {
    ::create();
}

// SO 專屬後置修飾：成功領悟與失敗皆會依據 evolve 配置來變動靈力與業力
void apply_adventure_side_effects(object actor, mapping act, int passed) {
    if (!actor || !act) return;
    
    // 優先讀取嵌套的 adventure_effects，若不存在或不是 mapping 則直接 Fallback 讀取 act 本身
    mapping effects = act["adventure_effects"];
    if (!effects || !mapp(effects)) {
        effects = act;
    }

    int se = effects["spiritual_energy"];
    if (se) actor->add_physical_state("spiritual_energy", se);

    int karma_change = effects["karma_change"];
    if (karma_change) actor->add_physical_state("karma", karma_change);
}
