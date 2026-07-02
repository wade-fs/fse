// /tests/test_so_loop.c (蜀山遊記 SO 最小整合測試套件)
#include "/runtime/include/ansi.h"

int test_initial_bootstrap();
int test_meditation_flow();
int test_social_commerce_flow();

int run_all_tests() {
    write(HIW "\n============================================\n" NOR);
    write(HIW "   開始執行 尋仙問道 (SO) 核心 Reality 測試  \n" NOR);
    write(HIW "============================================\n" NOR);

    int failures = 0;
    if (test_initial_bootstrap() != 0) failures++;
    if (test_meditation_flow()   != 0) failures++;
    if (test_social_commerce_flow() != 0) failures++;

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

int test_social_commerce_flow() {
    write("🧪 測試 3: 驗證紅塵商貿雙重證據鏈評估與 WebSocket 推播...\n");
    object actor = clone_object("/std/user.c");
    actor->set_id("tester");
    actor->set_name("測試行商");
    actor->clear_observations();

    object node = load_object("/nodes/town_center/node");
    if (!node) {
        write(HIR "  ❌ 失敗: 無法載入節點 /nodes/town_center/node。\n" NOR);
        destruct(actor); return 1;
    }

    if (getenv("MUD_TEST_MODE") || this_player()) {
        write(HIK "  [Test Debug] loaded node=" + file_name(node) + 
              ", entity_id=" + node->query_entity_id() + 
              ", short=" + node->query_short() + "\n" NOR);
    }

    // 1. 將玩家移入紅塵集市，驗證 node_state 推播
    write("  👉 [Step 1] 移入集市並觸發 on_enter，驗證推播...\n");
    move_object(actor, node);
    node->on_enter(actor); // 內部會觸發 send_json_event "node_state"

    // 2. 測試無證據時的 haggle_by_force 偏誤 (Misconception)
    write("  👉 [Step 2] 在無 Observation 證據下進行交易，預期判定為成見 (haggle_by_force)...\n");
    // 設定需要定心 Factor 以渡過 prerequisites
    actor->discover_factor("stillness_resonance", ([]));
    
    int result = node->resolve_interaction(actor, "trade_fair", "trader");
    if (!result) {
        write(HIR "  ❌ 失敗: 交互未被 ActionExecutor 接管。\n" NOR);
        node->on_leave(actor); destruct(actor); return 1;
    }

    // 預期業力暴漲 +10 (從初始 0 變為 10)
    if (actor->query_karma() != 10) {
        write(HIR "  ❌ 失敗: 業力應增加為 10，但目前為: " + actor->query_karma() + "\n" NOR);
        node->on_leave(actor); destruct(actor); return 1;
    }
    write(HIG "  ✓ 通過: 無證據交易判定正確 (karma: 0 -> 10)。\n" NOR);

    // 3. 收集雙重證據 (market_price + trader_greed) 後交易，預期 Understanding 了結因果
    write("  👉 [Step 3] 模擬收集雙重證據 [market_price, trader_greed] 再次交易...\n");
    actor->add_observation("market_price");
    actor->add_observation("trader_greed");

    result = node->resolve_interaction(actor, "trade_fair", "trader");
    if (!result) {
        write(HIR "  ❌ 失敗: 收集證據後交易未能被 Resolver 處理。\n" NOR);
        node->on_leave(actor); destruct(actor); return 1;
    }

    // 預期領悟了結塵緣，業力削減 10 點 (從 10 變回 0)
    if (actor->query_karma() != 0) {
        write(HIR "  ❌ 失敗: 業力應削減回 0，但目前為: " + actor->query_karma() + "\n" NOR);
        node->on_leave(actor); destruct(actor); return 1;
    }
    
    // 驗證是否獲得 commerce_master 知識 Factor
    if (!actor->has_factor("commerce_master")) {
        write(HIR "  ❌ 失敗: 玩家未解鎖 'commerce_master' 知識點！\n" NOR);
        node->on_leave(actor); destruct(actor); return 1;
    }

    write(HIG "  ✓ 通過: 雙重證據鏈對齊交易領悟成功，了結塵緣 (karma: 10 -> 0) 且解鎖 commerce_master！\n" NOR);

    node->on_leave(actor);
    destruct(actor);
    return 0;
}
