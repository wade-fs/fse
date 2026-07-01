// /tests/test_so_loop.c (蜀山遊記 SO 最小整合測試套件)
#include "/runtime/include/ansi.h"

int test_initial_bootstrap();
int test_meditation_flow();

int run_all_tests() {
    write(HIW "\n============================================\n" NOR);
    write(HIW "   開始執行 尋仙問道 (SO) 核心 Reality 測試  \n" NOR);
    write(HIW "============================================\n" NOR);

    int failures = 0;
    if (test_initial_bootstrap() != 0) failures++;
    if (test_meditation_flow()   != 0) failures++;

    if (failures == 0) {
        write(HIG "\n🎉 [SUCCESS] 尋仙問道 (SO) 測試全部通過！\n" NOR);
        return 0;
    } else {
        write(HIR "\n❌ [FAILURE] 尋仙問道 (SO) 測試有 " + failures + " 個失敗項目。\n" NOR);
        return 1;
    }
}

int test_initial_bootstrap() {
    write("🧪 測試 1: 驗證玩家載體建立與修仙屬性初始值...\n");
    object actor = clone_object("/std/user.c");
    if (!actor) {
        write(HIR "  ❌ 失敗: 無法建立玩家物件。\n" NOR);
        return 1;
    }
    actor->set_id("tester");
    actor->restore_state();

    if (actor->query_karma() != 0) {
        write(HIR "  ❌ 失敗: 初始業力不為 0。\n" NOR);
        destruct(actor); return 1;
    }
    if (actor->query_spiritual_energy() != 100) {
        write(HIR "  ❌ 失敗: 初始靈力不為 100。\n" NOR);
        destruct(actor); return 1;
    }
    write(HIG "  ✓ 通過: 玩家物件與初始修仙屬性正常。\n" NOR);
    destruct(actor);
    return 0;
}

int test_meditation_flow() {
    write("🧪 測試 2: 驗證靜慮崖節點載入...\n");
    object actor = clone_object("/std/user.c");
    actor->set_id("tester");

    object node = load_object("/nodes/meditation_cliff/node");
    if (!node) {
        write(HIR "  ❌ 失敗: 無法載入節點 /nodes/meditation_cliff/node。\n" NOR);
        destruct(actor); return 1;
    }

    // 將 actor 移入節點，觸發 on_enter
    move_object(actor, node);
    node->on_enter(actor);

    mapping state = node->query_node_state(actor);
    if (!state || !state["short"]) {
        write(HIR "  ❌ 失敗: query_node_state 回傳無效。\n" NOR);
        node->on_leave(actor);
        destruct(actor); return 1;
    }
    write(HIG "  ✓ 通過: 靜慮崖節點載入，query_node_state 正常。\n" NOR);
    node->on_leave(actor);
    destruct(actor);
    return 0;
}
