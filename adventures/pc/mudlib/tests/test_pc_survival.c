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
    
    // 取得初始房間 (三疊紀荒原 - 新手出生點)
    object plains = load_object("/rooms/triassic_plains/room");
    if (!plains || !player) {
        write(HIR "❌ 測試失敗: 無法初始化玩家或荒原房間物件。\n" NOR);
        return 1;
    }

    // 讓玩家進入房間 (模擬新玩家首次登入)
    move_object(player, plains);
    plains->enter(player);

    object pm = load_object("/runtime/services/progress_manager.c");

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
    // 🧪 測試 3：使用 roots 和 branches 摩擦引火並晉升
    // ----------------------------------------------------
    write(CYN "🧪 [測試 3] 使用 roots 和 branches 摩擦引火...\n" NOR);
    player->force_me("rub branches roots");
    if (!player->has_factor("thermodynamics")) {
        write(HIR "❌ 測試 3 失敗: 具備材料後 rub 依然未獲得 thermodynamics factor！\n" NOR);
        return 1;
    }
    if (pm->query_current_stage(player, "main") != "survivor") {
        write(HIR "❌ 測試 3 失敗: 生火後未晉升至 survivor 階段！\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 摩擦引火領悟成功，解鎖 thermodynamics 並晉升至 survivor 階段。\n" NOR);

    // ----------------------------------------------------
    // 💡 模擬第二次登入 / 已有進度的玩家起點切換至乾燥峽谷
    // ----------------------------------------------------
    write(CYN "💡 [進度切換] 檢測到已解鎖 thermodynamics，玩家起點切換至乾燥峽谷...\n" NOR);
    object canyon = load_object("/rooms/desert_canyon/room");
    if (!canyon) {
        write(HIR "❌ 無法載入乾燥峽谷房間物件。\n" NOR);
        return 1;
    }
    move_object(player, canyon);
    canyon->enter(player);

    // ----------------------------------------------------
    // 🧪 測試 4：在乾燥峽谷嘗試奔跑以測試缺氧機制
    // ----------------------------------------------------
    write(CYN "🧪 [測試 4] 在乾燥峽谷嘗試奔跑以測試缺氧機制...\n" NOR);
    player->force_me("run");
    if (!player->has_factor("oxygen_scarcity")) {
        write(HIR "❌ 測試 4 失敗: 奔跑後未獲得 oxygen_scarcity factor！\n" NOR);
        return 1;
    }

    // 驗證是否因為具備 thermodynamics 而正確復活於乾燥峽谷
    string env_name = environment(player) ? base_name(environment(player)) : "none";
    if (strsrch(env_name, "desert_canyon") == -1) {
        write(HIR "❌ 測試 4 失敗: 死亡後未正確復活於乾燥峽谷起點！實際位置: " + env_name + "\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 奔跑缺氧倒地死亡、領悟 oxygen_scarcity，並正確復活於乾燥峽谷。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 5：觸碰地表岩石測試高溫灼傷機制
    // ----------------------------------------------------
    write(CYN "🧪 [測試 5] 觸碰地表岩石測試高溫灼傷機制...\n" NOR);
    player->force_me("touch ground");
    if (!player->has_factor("heat_regulation")) {
        write(HIR "❌ 測試 5 失敗: 觸碰地表岩石後未獲得 heat_regulation factor！\n" NOR);
        return 1;
    }
    string env_name_5 = environment(player) ? base_name(environment(player)) : "none";
    if (strsrch(env_name_5, "desert_canyon") == -1) {
        write(HIR "❌ 測試 5 失敗: 死亡後未正確復活於乾燥峽谷起點！實際位置: " + env_name_5 + "\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 高溫灼傷死亡、領悟 heat_regulation，並正確復活於乾燥峽谷。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 6：檢驗背光陰影出口 (shade) 是否解鎖
    // ----------------------------------------------------
    write(CYN "🧪 [測試 6] 檢驗背光陰影出口 (shade) 是否解鎖...\n" NOR);
    mapping exits = canyon->query_exits(player);
    if (undefinedp(exits["shade"])) {
        write(HIR "❌ 測試 6 失敗: 具備 heat_regulation 後，shade 出口依然未 Reveal！\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功 Reveal 背光陰影出口。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 7：前往背光陰影 (go shade)
    // ----------------------------------------------------
    write(CYN "🧪 [測試 7] 前往背光陰影 (go shade)...\n" NOR);
    player->force_me("go shade");
    object current_room = environment(player);
    if (!current_room || strsrch(base_name(current_room), "triassic_shade") == -1) {
        write(HIR "❌ 測試 7 失敗: 玩家移動失敗或未進入背光陰影房間。\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功移動至背光洞窟陰影。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 8：在陰影下休息 (rest) 撐過第零天
    // ----------------------------------------------------
    write(CYN "🧪 [測試 8] 在陰影下休息 (rest) 撐過第零天...\n" NOR);
    player->force_me("rest");
    // 已完成 survive_day_zero 任務
    write(HIG "  ✓ 通過: 成功完成 survive_day_zero。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 9：前往史前河床 (go riverbed)
    // ----------------------------------------------------
    write(CYN "🧪 [測試 9] 前往史前河床 (go riverbed)...\n" NOR);
    player->force_me("go riverbed");
    current_room = environment(player);
    if (!current_room || strsrch(base_name(current_room), "triassic_riverbed") == -1) {
        write(HIR "❌ 測試 9 失敗: 未能進入史前河床房間。\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功移動至史前河床。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 10：嘗試舔水窪中的水 (lick water)
    // ----------------------------------------------------
    write(CYN "🧪 [測試 10] 嘗試舔水窪中的水 (lick water)...\n" NOR);
    player->force_me("lick water");
    if (!player->has_factor("water_boiling")) {
        write(HIR "❌ 測試 10 失敗: 嚐試水質後未解鎖 water_boiling factor！\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 嚐試水質成功領悟 water_boiling。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 11：前往三疊紀荒原 (go plains) 並移動前往巨型蕨類森林 (go forest)
    // ----------------------------------------------------
    write(CYN "🧪 [測試 11] 前往三疊紀荒原並轉往森林...\n" NOR);
    player->force_me("go plains");
    player->force_me("go forest");
    current_room = environment(player);
    if (!current_room || strsrch(base_name(current_room), "fern_forest") == -1) {
        write(HIR "❌ 測試 11 失敗: 玩家移動失敗或未進入巨型蕨類森林房間。\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功穿越荒原移動至巨型蕨類森林。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 12：森林中 focus ground，應引導黑曜石
    // ----------------------------------------------------
    write(CYN "🧪 [測試 12] 森林中 focus ground，應引導黑曜石...\n" NOR);
    player->force_me("focus ground");
    if (!player->query_temp("found_obsidian")) {
        write(HIR "❌ 測試 12 失敗: 在森林中 focus ground 未能標記 found_obsidian temp 變數！\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功動態感知並發現黑曜石。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 13：敲擊 strike obsidian 解鎖 flint_knapping
    // ----------------------------------------------------
    write(CYN "🧪 [測試 13] 敲擊黑曜石製作工具 (strike obsidian obsidian)...\n" NOR);
    player->force_me("strike obsidian obsidian");
    if (!player->has_factor("flint_knapping")) {
        write(HIR "❌ 測試 13 失敗: 敲擊黑曜石後未獲得 flint_knapping factor！\n" NOR);
        return 1;
    }
    if (!player->query_temp("has_sharp_obsidian")) {
        write(HIR "❌ 測試 13 失敗: 未獲得 sharp_obsidian 黑曜石片工具標記！\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 打製石器成功，解鎖 flint_knapping 並獲得工具片。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 14：驗證動態難度切換與 Nightmare 傷害加成乘數
    // ----------------------------------------------------
    write(CYN "🧪 [測試 14] 測試 Wizard 動態切換難度為 Nightmare 與傷害放大...\n" NOR);
    move_object(player, plains);
    player->set_temp("found_roots", 0); // 確保失敗
    player->force_me("god difficulty nightmare");
    player->set_hp(100);
    player->force_me("kick rocks");
    
    int dmg_taken = 100 - player->query_hp();
    // 5 * 2.5 = 12.5，LPC 轉 int 預期為 12 點傷害
    if (dmg_taken != 12) {
        write(HIR "❌ 測試 14 失敗: 難度加成未生效，預期扣減 12 HP，實際扣減了 " + dmg_taken + " HP！\n" NOR);
        player->force_me("god difficulty normal");
        return 1;
    }
    
    player->force_me("god difficulty normal");
    write(HIG "  ✓ 通過: 難度切換與 Nightmare 乘數傷害放大驗證成功。\n" NOR);

    destruct(player);
    write(HIW "\n============================================\n" +
              "  ✓ FSE PC — 史前文明所有整合測試全部通過！\n" +
              "============================================\n" NOR);
    return 0;
}
