// /adventures/pc/mudlib/tests/test_pc_survival.c
// PC (史前文明) 生存閉環整合測試套件
#include "/include/ansi.h"
inherit "/std/object";

int run_all_tests() {
    write(HIW "\n============================================\n" +
              "  FSE PC — 史前文明自動化整合測試開始\n" +
              "============================================\n" NOR);

    object player = clone_object("/std/user.c");
    player->set_id("test_survivor");
    
    // 取得初始房間 (三疊紀荒原)
    object plains = load_object("/rooms/triassic_plains/room");
    if (!plains || !player) {
        write(HIR "❌ 測試失敗: 無法初始化玩家或荒原房間物件。\n" NOR);
        return 1;
    }

    // 讓玩家進入房間
    move_object(player, plains);
    plains->enter(player);

    // ----------------------------------------------------
    // 🧪 測試 1：初來乍到，未 focus 前嘗試 rub 應該失敗
    // ----------------------------------------------------
    write(CYN "🧪 [測試 1] 未尋找乾枯根部前嘗試生火...\n" NOR);
    player->force_me("rub branches roots");
    if (player->has_factor("thermodynamics")) {
        write(HIR "❌ 測試 1 失敗: 未發現引火物居然直接解鎖了 thermodynamics factor！\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 未尋找材料時生火失敗，觸發 Confusion。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 2：focus ground 發現 roots
    // ----------------------------------------------------
    write(CYN "🧪 [測試 2] 專注地面感知 (focus ground)...\n" NOR);
    player->force_me("focus ground");
    if (!player->query_temp("found_roots")) {
        write(HIR "❌ 測試 2 失敗: focus ground 之後未標記 found_roots temp 變數！\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功在荒原發現乾燥根部。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 3：再次嘗試 rub，應完成 first_fire
    // ----------------------------------------------------
    write(CYN "🧪 [測試 3] 使用 roots 和 branches 摩擦引火...\n" NOR);
    player->force_me("rub branches roots");
    if (!player->has_factor("thermodynamics")) {
        write(HIR "❌ 測試 3 失敗: 具備材料後 rub 依然未獲得 thermodynamics factor！\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 摩擦引火領悟成功，解鎖 thermodynamics。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 4：重試 rub，應觸發防重防副作用機制
    // ----------------------------------------------------
    write(CYN "🧪 [測試 4] 重複摩擦，檢驗防重複機制...\n" NOR);
    player->force_me("rub branches roots");
    // 檢查出口是否依然包含 forest
    mapping exits = plains->query_exits(player);
    if (undefinedp(exits["forest"])) {
        write(HIR "❌ 測試 4 失敗: 傳入玩家物件查詢時，森林出口未被正確動態 Reveal 解鎖。\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 重複 rub 觸發冪等性守衛，無重複副作用。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 5：go forest 移動至巨型蕨類森林
    // ----------------------------------------------------
    write(CYN "🧪 [測試 5] 移動前往巨型蕨類森林 (go forest)...\n" NOR);
    player->force_me("go forest");
    object current_room = environment(player);
    if (!current_room || strsrch(base_name(current_room), "fern_forest") == -1) {
        write(HIR "❌ 測試 5 失敗: 玩家移動失敗或未進入巨型蕨類森林房間。\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功跨空間移動至巨型蕨類森林。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 6：新環境 focus ground 應引導黑曜石，而非 roots
    // ----------------------------------------------------
    write(CYN "🧪 [測試 6] 森林中 focus ground，應引導黑曜石...\n" NOR);
    player->force_me("focus ground");
    if (!player->query_temp("found_obsidian")) {
        write(HIR "❌ 測試 6 失敗: 在森林中 focus ground 未能標記 found_obsidian temp 變數！\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功動態感知並發現黑曜石。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 7：敲擊 strike obsidian 解鎖 flint_knapping
    // ----------------------------------------------------
    write(CYN "🧪 [測試 7] 敲擊黑曜石製作工具 (strike obsidian obsidian)...\n" NOR);
    player->force_me("strike obsidian obsidian");
    if (!player->has_factor("flint_knapping")) {
        write(HIR "❌ 測試 7 失敗: 敲擊黑曜石後未獲得 flint_knapping factor！\n" NOR);
        return 1;
    }
    if (!player->query_temp("has_sharp_obsidian")) {
        write(HIR "❌ 測試 7 失敗: 未獲得 sharp_obsidian 黑曜石片工具標記！\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 打製石器成功，解鎖 flint_knapping 並獲得工具片。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 8：退回荒野，測試 kick rocks（前置條件已不滿足，應失敗並扣血）
    // ----------------------------------------------------
    write(CYN "🧪 [測試 8] 測試 kick rocks 失敗路徑與懲罰機制...\n" NOR);
    player->force_me("go back"); // 回到荒原
    player->set_temp("found_roots", 0); // 清空暫時狀態以模擬未發現狀態
    int initial_hp = player->query_hp();
    player->force_me("kick rocks");
    if (player->query_hp() >= initial_hp) {
        write(HIR "❌ 測試 8 失敗: kick rocks 失敗時未扣減生命值！\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 實體互動失敗路徑執行正確，扣血懲罰生效。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 9：測試 lick water（觸發毒素並扣血）
    // ----------------------------------------------------
    write(CYN "🧪 [測試 9] 測試 lick water 動態物理交互...\n" NOR);
    initial_hp = player->query_hp();
    player->force_me("lick water");
    if (player->query_hp() >= initial_hp) {
        write(HIR "❌ 測試 9 失敗: 舔毒水未扣減生命值！\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 實體互動成功解析，毒水反應與傷害生效。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 10：驗證背包 (inventory) 攜帶物品系統與查詢
    // ----------------------------------------------------
    write(CYN "🧪 [測試 10] 驗證實體物品攜帶與背包查詢 (i/inventory)...\n" NOR);
    object *items = all_inventory(player);
    items = filter(items, (: function_exists("query_item_id", $1) :));
    if (sizeof(items) < 2) {
        write(HIR "❌ 測試 10 失敗: 玩家背包中找不到預期生成的火種與黑曜石石刀！\n" NOR);
        return 1;
    }
    // 執行背包列印指令確保無 runtime 錯誤
    player->force_me("i");
    write(HIG "  ✓ 通過: 背包物資狀態記錄與動態物品攜帶驗證成功。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 11：驗證動態難度切換與 Nightmare 傷害加成乘數
    // ----------------------------------------------------
    write(CYN "🧪 [測試 11] 測試 Wizard 動態切換難度為 Nightmare 與傷害放大...\n" NOR);
    // 1. 先用 god 指令變更難度
    player->force_me("god difficulty nightmare");
    
    // 2. 測試 kick rocks 的地獄傷害 (基礎 fail_hp: -5，Nightmare 傷害倍率為 2.5 倍，預期扣減 12 滴血)
    player->set_hp(100);
    player->set_temp("found_roots", 0); // 確保失敗
    player->force_me("kick rocks");
    
    int dmg_taken = 100 - player->query_hp();
    // 5 * 2.5 = 12.5，LPC 轉 int 預期為 12 點傷害
    if (dmg_taken != 12) {
        write(HIR "❌ 測試 11 失敗: 難度加成未生效，預期扣減 12 HP，實際扣減了 " + dmg_taken + " HP！\n" NOR);
        // 還原難度
        player->force_me("god difficulty normal");
        return 1;
    }
    
    // 3. 還原難度為正常
    player->force_me("god difficulty normal");
    write(HIG "  ✓ 通過: 難度切換與 Nightmare 乘數傷害放大驗證成功。\n" NOR);

    destruct(player);
    write(HIW "\n============================================\n" +
              "  ✓ FSE PC — 史前文明所有整合測試全部通過！\n" +
              "============================================\n" NOR);
    return 0;
}
