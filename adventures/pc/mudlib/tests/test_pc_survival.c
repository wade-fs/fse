// /adventures/pc/mudlib/tests/test_pc_survival.c
// PC (史前文明) 生存閉環整合測試套件
#include "/include/ansi.h"
inherit "/std/object";

int run_all_tests() {
    write(HIW "\n============================================\n" +
              "  FSE PC — 史前文明自動化整合測試開始\n" +
              "============================================\n" +
              "  [新手村安全區：4房間空間相連與闖關機制驗證]\n" +
              "============================================\n" NOR);

    object player = clone_object("/std/user.c");
    player->set_id("test_survivor");
    
    // 取得初始房間 (三疊紀荒原 - 新手出生點)
    object plains = load_object("/rooms/triassic_plains/room");
    if (!plains || !player) {
        write(HIR "❌ 測試失敗: 無法初始化玩家或荒原房間物件。\n" NOR);
        return 1;
    }

    // 讓玩家進入荒原
    move_object(player, plains);
    plains->enter(player);

    object pm = load_object("/runtime/services/progress_manager.c");

    // ----------------------------------------------------
    // 🧪 測試 1：驗證新手村不致死安全保護機制
    // ----------------------------------------------------
    write(CYN "🧪 [測試 1] 驗證新手村不致死安全保護機制...\n" NOR);
    player->set_hp(5);
    player->set_temp("found_roots", 0); // 確保 kick rocks 失敗
    player->force_me("kick rocks");
    
    if (player->query_hp() != 1) {
        write(HIR "❌ 測試 1 失敗: 在新手村安全區內扣血居然致死或未降到 1！目前 HP: " + player->query_hp() + "\n" NOR);
        return 1;
    }
    string env_name_1 = environment(player) ? base_name(environment(player)) : "none";
    if (strsrch(env_name_1, "triassic_plains") == -1) {
        write(HIR "❌ 測試 1 失敗: 安全區內力竭居然被傳送了！實際位置: " + env_name_1 + "\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 安全區不致死機制生效，HP 降為 1 但未死亡且留在原地。\n" NOR);

    // 恢復 HP 繼續測試
    player->set_hp(100);

    // ----------------------------------------------------
    // 🧪 測試 2：在荒原 focus ground 並生火解鎖 thermodynamics
    // ----------------------------------------------------
    write(CYN "🧪 [測試 2] 荒原生火 (novice 首要任務)...\n" NOR);
    player->force_me("focus ground");
    player->force_me("rub branches roots");
    if (!player->has_factor("thermodynamics")) {
        write(HIR "❌ 測試 2 失敗: rub 未能獲得 thermodynamics factor！\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功引火並獲得 thermodynamics。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 3：前往巨型蕨類森林並打製石器解鎖 flint_knapping
    // ----------------------------------------------------
    write(CYN "🧪 [測試 3] 前往巨型蕨類森林 (go forest) 打製石器...\n" NOR);
    player->force_me("go forest");
    object forest = environment(player);
    if (!forest || strsrch(base_name(forest), "fern_forest") == -1) {
        write(HIR "❌ 測試 3 失敗: 未能進入巨型蕨類森林。\n" NOR);
        return 1;
    }
    player->force_me("focus ground");
    player->force_me("strike obsidian obsidian");
    if (!player->has_factor("flint_knapping")) {
        write(HIR "❌ 測試 3 失敗: 未能打製石刃解鎖 flint_knapping！\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功打製石刀並解鎖 flint_knapping。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 4：進入黑暗溶洞 burn wood 觀察 painting 解鎖 wind_direction
    // ----------------------------------------------------
    write(CYN "🧪 [測試 4] 進入黑暗溶洞 (go cave) 解讀史前壁畫...\n" NOR);
    player->force_me("go cave");
    object cave = environment(player);
    if (!cave || strsrch(base_name(cave), "dark_cave") == -1) {
        write(HIR "❌ 測試 4 失敗: 未能進入黑暗溶洞。\n" NOR);
        return 1;
    }

    player->force_me("burn wood");
    write("  DEBUG: has cave_painting = " + player->has_factor("cave_painting") + "\n");
    player->force_me("look painting");
    write("  DEBUG: has wind_direction = " + player->has_factor("wind_direction") + "\n");
    if (!player->has_factor("wind_direction")) {
        write(HIR "❌ 測試 4 失敗: 未能通過壁畫解鎖 wind_direction！\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功利用火種點燃柴火，解讀壁畫並領悟 wind_direction。\n" NOR);

    // 回到森林
    player->force_me("go back");

    // ----------------------------------------------------
    // 🧪 測試 5：進入捕食者峽谷利用逆風隱蔽完成新手逃脫
    // ----------------------------------------------------
    write(CYN "🧪 [測試 5] 進入捕食者峽谷 (go canyon) 潛伏避開 Herrerasaurus...\n" NOR);
    player->force_me("go canyon");
    object canyon = environment(player);
    if (!canyon || strsrch(base_name(canyon), "predator_canyon") == -1) {
        write(HIR "❌ 測試 5 失敗: 未能進入捕食者峽谷。\n" NOR);
        return 1;
    }
    player->force_me("focus wind");
    player->force_me("hide downwind");
    
    if (!player->has_factor("stealth_camouflage")) {
        write(HIR "❌ 測試 5 失敗: 潛伏逃脫後未獲得 stealth_camouflage factor！\n" NOR);
        return 1;
    }
    if (pm->query_current_stage(player, "main") != "day_zero") {
        write(HIR "❌ 測試 5 失敗: 逃脫成功後，境界未正確推進至 day_zero 階段！目前 Stage: " + pm->query_current_stage(player, "main") + "\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功潛伏避開巨獸，解鎖 stealth_camouflage，並晉升至 day_zero 階段！\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 6：新手任務完成，前往 day_zero 起點乾燥峽谷
    // ----------------------------------------------------
    write(CYN "🧪 [測試 6] 跨入非安全區：前往乾燥峽谷 (go desert)...\n" NOR);
    player->force_me("go desert");
    object dcanyon = environment(player);
    if (!dcanyon || strsrch(base_name(dcanyon), "desert_canyon") == -1) {
        write(HIR "❌ 測試 6 失敗: 未能穿越進入乾燥峽谷區域！\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功透過相連地圖移動至乾燥峽谷。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 7：驗證非安全區的死亡與墓地起點轉移機制 (day_zero 墓地)
    // ----------------------------------------------------
    write(CYN "🧪 [測試 7] 驗證非安全區缺氧奔跑致死與墓地轉移...\n" NOR);
    player->force_me("run");
    if (!player->has_factor("oxygen_scarcity")) {
        write(HIR "❌ 測試 7 失敗: 奔跑後未獲得 oxygen_scarcity factor！\n" NOR);
        return 1;
    }
    
    // 驗證死亡後是否被送回 day_zero 的 spawn_node (desert_canyon)
    string env_name = environment(player) ? base_name(environment(player)) : "none";
    if (strsrch(env_name, "desert_canyon") == -1) {
        write(HIR "❌ 測試 7 失敗: 死亡後未正確復活於當前 stage (day_zero) 的起點 (desert_canyon)！實際位置: " + env_name + "\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 非安全區死亡機制成立，且復活點已切換至 day_zero 起點 (乾燥峽谷)。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 8：高溫灼傷致死
    // ----------------------------------------------------
    write(CYN "🧪 [測試 8] 觸碰地表岩石測試高溫灼傷致死...\n" NOR);
    player->force_me("touch ground");
    if (!player->has_factor("heat_regulation")) {
        write(HIR "❌ 測試 8 失敗: 未獲得 heat_regulation factor！\n" NOR);
        return 1;
    }
    env_name = environment(player) ? base_name(environment(player)) : "none";
    if (strsrch(env_name, "desert_canyon") == -1) {
        write(HIR "❌ 測試 8 失敗: 死亡後未正確復活於當前 stage 的起點！實際位置: " + env_name + "\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 高溫灼傷致死並成功領悟 heat_regulation。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 9：前往 shade 避難並完成 survive_day_zero
    // ----------------------------------------------------
    write(CYN "🧪 [測試 9] 尋找背光陰影避難並完成第零天任務...\n" NOR);
    player->force_me("go shade");
    object shade = environment(player);
    if (!shade || strsrch(base_name(shade), "triassic_shade") == -1) {
        write(HIR "❌ 測試 9 失敗: 未能進入背光陰影。\n" NOR);
        return 1;
    }
    player->force_me("rest");
    if (pm->query_current_stage(player, "main") != "first_night") {
        write(HIR "❌ 測試 9 失敗: 避難後未正確晉升至 first_night 階段！\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功進入陰影避難，完成 survive_day_zero 並晉升至 first_night。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 10：前往史前河床品嚐水質解鎖 water_boiling
    // ----------------------------------------------------
    write(CYN "🧪 [測試 10] 前往史前河床 (go riverbed) 發現水質問題...\n" NOR);
    player->force_me("go riverbed");
    player->set_thirst(50);
    player->force_me("lick water");
    if (!player->has_factor("water_boiling")) {
        write(HIR "❌ 測試 10 失敗: 嚐水後未獲得 water_boiling factor！\n" NOR);
        return 1;
    }
    if (player->query_thirst() != 40) {
        write(HIR "❌ 測試 10 失敗: 舔水後口渴度未扣減！目前口渴度: " + player->query_thirst() + "\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功發現水質有毒，解鎖 water_boiling 且降低口渴度。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 11：驗證非安全區背景心跳消耗 (飢餓、口渴與發呆扣 HP)
    // ----------------------------------------------------
    write(CYN "🧪 [測試 11] 驗證非安全區背景心跳消耗機制...\n" NOR);
    // 移至非安全區乾燥峽谷
    move_object(player, dcanyon);
    player->set_hunger(99);
    player->set_thirst(99);
    player->set_temp("hb_count", 14); // 模擬已累積 14 次心跳
    player->set_hp(100);
    
    // 觸發第 15 次心跳
    player->heart_beat();
    
    if (player->query_hunger() != 100 || player->query_thirst() != 100) {
        write(HIR "❌ 測試 11 失敗: 心跳消耗未能正確累加！飢餓: " + player->query_hunger() + "，口渴: " + player->query_thirst() + "\n" NOR);
        return 1;
    }
    if (player->query_hp() != 90) {
        write(HIR "❌ 測試 11 失敗: 極度飢渴狀態下心跳未扣減生命值！目前 HP: " + player->query_hp() + "\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 背景心跳消耗正常累加，且極度飢渴扣 HP 傷害成立。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 12：驗證動態難度切換與 Nightmare 傷害加成乘數
    // ----------------------------------------------------
    write(CYN "🧪 [測試 12] 測試 Wizard 動態切換難度為 Nightmare 與傷害放大...\n" NOR);
    move_object(player, plains);
    player->set_temp("found_roots", 0); // 確保失敗
    player->force_me("god difficulty nightmare");
    player->set_hp(100);
    player->force_me("kick rocks");
    
    int dmg_taken = 100 - player->query_hp();
    // 5 * 2.5 = 12.5，LPC 轉 int 預期為 12 點傷害
    if (dmg_taken != 12) {
        write(HIR "❌ 測試 12 失敗: 難度加成未生效，預期扣減 12 HP，實際扣減了 " + dmg_taken + " HP！\n" NOR);
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
