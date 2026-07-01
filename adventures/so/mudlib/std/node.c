// /std/node.c (蜀山遊記 SO 節點類別)
// 繼承 fse_room，覆寫 SO 專屬的修仙副作用。
#include "/runtime/include/ansi.h"
inherit "/std/fse_room";

void create() {
    ::create();
}

// SO 專屬後置修飾：成功領悟影響靈力與業力，失敗執念帶來業力懲罰
void apply_adventure_side_effects(object actor, mapping act, int passed) {
    if (!actor || !act) return;
    if (passed) {
        int se = act["spiritual_energy"];
        if (se) actor->add_spiritual_energy(se);
        int karma_change = act["karma_change"];
        if (karma_change) actor->add_karma(karma_change);
    } else {
        int fail_karma = act["fail_karma"] || 2;
        actor->add_karma(fail_karma);
    }
}
