// /adventures/pc/mudlib/tests/test_pc_survival.c
// PC (史前文明) 生存閉環整合測試套件
#include "/runtime/include/ansi.h"
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
    player->set_thirst(94);
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

    // ----------------------------------------------------
    // 🧪 測試 13：驗證 bug_eater 階段任務（巨蜈蚣獵捕、生火與蟲肉烹飪食）
    // ----------------------------------------------------
    write(CYN "🧪 [測試 13] 測試 bug_eater 階段 (triassic_shade / triassic_riverbed)...\n" NOR);
    // 確保回到陰影處，且進度正確前進至 first_night
    move_object(player, shade);
    
    // 觸發進度檢查（因為 first_fire 在 novice 已解鎖，這會直接讓玩家晉升至 bug_eater）
    player->force_me("light campfire"); // 觸發 check_player_stage_advancement
    
    if (pm->query_current_stage(player, "main") != "bug_eater") {
        write(HIR "❌ 測試 13 失敗: 玩家未正確晉升至 bug_eater 階段！目前: " + pm->query_current_stage(player, "main") + "\n" NOR);
        return 1;
    }
    
    // 模擬獵捕巨蜈蚣
    player->force_me("hunt megapede");
    if (!player->has_factor("arthropod_warning")) {
        write(HIR "❌ 測試 13 失敗: 獵捕巨蜈蚣後未能解鎖 arthropod_warning！\n" NOR);
        return 1;
    }
    
    // 烹飪並食用蟲肉
    player->force_me("roast meat");
    if (!player->has_factor("arthropod_nutrition")) {
        write(HIR "❌ 測試 13 失敗: 烘烤蟲肉後未能解鎖 arthropod_nutrition！\n" NOR);
        return 1;
    }
    player->force_me("eat cooked_meat");
    
    // 食用熟肉後進度檢查，晉升至 wind_stalker
    pm->check_player_stage_advancement(player, "main");
    if (pm->query_current_stage(player, "main") != "wind_stalker") {
        write(HIR "❌ 測試 13 失敗: 完成第一頓飯後未晉升至 wind_stalker 階段！目前: " + pm->query_current_stage(player, "main") + "\n" NOR);
        return 1;
    }
    
    // 前往史前河床，透過專注聲音 (focus sound) 領悟 vibration_translation
    object riverbed = load_object("/rooms/triassic_riverbed/room");
    move_object(player, riverbed);
    riverbed->enter(player);
    player->force_me("focus sound");
    
    if (!player->has_factor("vibration_translation")) {
        write(HIR "❌ 測試 13 失敗: 專注聲音後未能領悟 vibration_translation！\n" NOR);
        return 1;
    }
    
    // 領悟後進度檢查，晉升至 convection_navigation
    pm->check_player_stage_advancement(player, "main");
    if (pm->query_current_stage(player, "main") != "convection_navigation") {
        write(HIR "❌ 測試 13 失敗: 領悟震動翻譯後未晉升至 convection_navigation 階段！目前: " + pm->query_current_stage(player, "main") + "\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功獵捕巨蜈蚣、食熟肉，並前往河床觀察地鳴震動，順利解鎖震動翻譯並晉級大氣與對流考驗階段。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 13-2：驗證 convection_navigation 階段（火山熔岩對流與沙塵暴偏振定位）
    // ----------------------------------------------------
    write(CYN "🧪 [測試 13-2] 測試 convection_navigation 階段 (wind_canyon / lava_tube / alkali_flats)...\n" NOR);
    
    // 1. 大峽谷拾取雲母
    object canyon_room = load_object("/rooms/wind_canyon/room");
    move_object(player, canyon_room);
    canyon_room->enter(player);
    
    player->force_me("focus ground");
    player->force_me("pickup mica");
    
    // 檢查是否獲得 mica 道具
    object *inv = all_inventory(player);
    int has_mica = 0;
    foreach (object ob in inv) {
        if (ob->query_item_id() == "mica") {
            has_mica = 1;
            break;
        }
    }
    if (!has_mica) {
        write(HIR "❌ 測試 13-2 失敗: 拾取雲母石片後背包內未找到 mica 道具！\n" NOR);
        return 1;
    }

    // 2. 火山熔岩管熱對流觀測與引導
    object tube_room = load_object("/rooms/lava_tube/room");
    move_object(player, tube_room);
    tube_room->enter(player);
    
    // 原地等待或奔跑應該被扣 HP 導致死亡並送回起點（驗證失敗是信息）
    player->force_me("run");
    string env_name = environment(player) ? base_name(environment(player)) : "none";
    if (strsrch(env_name, "wind_canyon") == -1) {
        write(HIR "❌ 測試 13-2 失敗: 熔岩管缺氧奔跑未觸發致死與起點 (wind_canyon) 轉移！實際位置: " + env_name + "\n" NOR);
        return 1;
    }
    
    // 重新走回熔岩管，進行對流觀測測試
    move_object(player, tube_room);
    tube_room->enter(player);
    
    // 利用火種觀測對流
    player->force_me("focus wind");
    player->force_me("harness convection");
    
    if (!player->has_factor("convection_currents")) {
        write(HIR "❌ 測試 13-2 失敗: harness convection 後未能領悟 convection_currents！\n" NOR);
        return 1;
    }

    // 3. 鹽鹼荒灘沙塵暴偏振導航定位
    object flats_room = load_object("/rooms/alkali_flats/room");
    move_object(player, flats_room);
    flats_room->enter(player);
    
    // 直接亂走應該迷失並扣 HP
    player->set_hp(100);
    old_hp = player->query_hp();
    player->force_me("go north");
    if (player->query_hp() >= old_hp) {
        write(HIR "❌ 測試 13-2 失敗: 沙塵暴盲目探索未受迷失方向與割傷懲罰！\n" NOR);
        return 1;
    }
    player->set_hp(100);
    
    // 使用雲母觀測偏振光
    player->force_me("look clouds through mica");
    if (!player->has_factor("polarization_navigation")) {
        write(HIR "❌ 測試 13-2 失敗: 觀測偏振後未能領悟 polarization_navigation！\n" NOR);
        return 1;
    }
    
    // 觸發進度檢驗，晉升至 jurassic_arrival
    pm->check_player_stage_advancement(player, "main");
    if (pm->query_current_stage(player, "main") != "jurassic_arrival") {
        write(HIR "❌ 測試 13-2 失敗: 領悟偏振導航與熱對流後未晉升至 jurassic_arrival 階段！目前: " + pm->query_current_stage(player, "main") + "\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功在大峽谷取得雲母、熔岩管導引對流、鹽鹼荒灘辨識偏振，晉級侏羅紀降臨階段。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 14：驗證 jurassic_arrival 階段（製作抓鉤並爬樹）
    // ----------------------------------------------------
    write(CYN "🧪 [測試 14] 測試 jurassic_arrival 階段 (jurassic_valley)...\n" NOR);
    object valley = load_object("/rooms/jurassic_valley/room");
    move_object(player, valley);
    valley->enter(player);
    
    player->force_me("focus ground");
    player->force_me("cut fibers");
    player->force_me("make hook");
    player->force_me("climb tree");
    
    if (!player->has_factor("canopy_climbing")) {
        write(HIR "❌ 測試 14 失敗: 攀爬樹木後未解鎖 canopy_climbing！\n" NOR);
        return 1;
    }
    
    player->force_me("go up");
    if (strsrch(base_name(environment(player)), "canopy_refuge") == -1) {
        write(HIR "❌ 測試 14 失敗: go up 未能進入樹冠避難所房間！目前位置: " + base_name(environment(player)) + "\n" NOR);
        return 1;
    }
    
    // 觸發進度檢驗
    player->force_me("look");
    if (pm->query_current_stage(player, "main") != "canopy_builder") {
        write(HIR "❌ 測試 14 失敗: 未正確升級至 canopy_builder 階段！目前: " + pm->query_current_stage(player, "main") + "\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功在侏羅紀河谷採集纖維、編織抓鉤並攀爬至樹冠避難所。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 15：驗證 canopy_builder 階段（構築高空木屋避難所）
    // ----------------------------------------------------
    write(CYN "🧪 [測試 15] 測試 canopy_builder 階段 (canopy_refuge)...\n" NOR);
    player->force_me("focus wind");
    player->force_me("focus ground");
    write("DEBUG: has flint_knapping = " + player->has_factor("flint_knapping") + "\n");
    write("DEBUG: has found_vines = " + player->query_temp("found_vines") + "\n");
    player->force_me("twist vines");
    write("DEBUG: after twist vines, has fiber_strength = " + player->has_factor("fiber_strength") + "\n");
    player->force_me("build platform");
    write("DEBUG: after build platform, has structural_load = " + player->has_factor("structural_load") + "\n");
    player->force_me("build shelter");
    
    if (!player->has_factor("altitude_wind") || !player->has_factor("fiber_strength") || !player->has_factor("structural_load")) {
        write(HIR "❌ 測試 15 失敗: 未能解鎖全部樹冠建築 Factors (altitude_wind/fiber_strength/structural_load)！\n" NOR);
        return 1;
    }
    
    // 觸發進度檢驗
    player->force_me("look");
    if (pm->query_current_stage(player, "main") != "allosaurus_gauntlet") {
        write(HIR "❌ 測試 15 失敗: 未正確升級至 allosaurus_gauntlet 階段！目前: " + pm->query_current_stage(player, "main") + "\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功在高空樹冠層完成防風觀測、繩索編織、平台搭架與避難所搭建。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 16：驗證 allosaurus_gauntlet 階段（靜止避開異特龍與遷徙規律解讀）
    // ----------------------------------------------------
    write(CYN "🧪 [測試 16] 測試 allosaurus_gauntlet 階段 (migration_trail)...\n" NOR);
    object trail = load_object("/rooms/migration_trail/room");
    move_object(player, trail);
    trail->enter(player);
    
    // 先嘗試跟隨足跡（應失敗，因為異特龍擋著）
    player->force_me("follow tracks");
    
    player->force_me("freeze");
    write("DEBUG 16: after freeze, has bypassed_allosaurus = " + player->query_temp("bypassed_allosaurus") + "\n");
    if (!player->has_factor("motion_stillness")) {
        write(HIR "❌ 測試 16 失敗: 靜止不動後未能領悟 motion_stillness！\n" NOR);
        return 1;
    }
    
    player->force_me("sneak");
    write("DEBUG 16: after sneak, has passed_gauntlet = " + player->query_temp("passed_gauntlet") + "\n");
    player->force_me("follow tracks");
    
    if (!player->has_factor("migration_cycle")) {
        write(HIR "❌ 測試 16 失敗: 跟隨足跡後未領悟 migration_cycle！\n" NOR);
        return 1;
    }
    
    player->force_me("go cliff");
    if (strsrch(base_name(environment(player)), "pterosaur_cliff") == -1) {
        write(HIR "❌ 測試 16 失敗: 進入 cliff 出口失敗！目前位置: " + base_name(environment(player)) + "\n" NOR);
        return 1;
    }
    
    // 檢驗進度
    player->force_me("look");
    if (pm->query_current_stage(player, "main") != "fire_master") {
        write(HIR "❌ 測試 16 失敗: 未能正確升級至 fire_master 階段！目前: " + pm->query_current_stage(player, "main") + "\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功運用靜止偽裝避開異特龍，繞行穿越並解讀恐龍遷徙走廊。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 17：驗證 fire_master 階段（石圈防風控火與熟食餵養翼龍）
    // ----------------------------------------------------
    write(CYN "🧪 [測試 17] 測試 fire_master 階段 (pterosaur_cliff)...\n" NOR);
    player->force_me("build fire_ring");
    player->force_me("light fire");
    player->force_me("hunt lizard");
    player->force_me("roast raw_meat");
    
    if (!player->has_factor("fire_control")) {
        write(HIR "❌ 測試 17 失敗: 築起石圈後未能解鎖 fire_control！\n" NOR);
        return 1;
    }
    
    player->force_me("feed pterosaur");
    if (!player->has_factor("pterosaur_bond")) {
        write(HIR "❌ 測試 17 失敗: 餵養翼龍後未能解鎖 pterosaur_bond！\n" NOR);
        return 1;
    }
    
    player->force_me("go shore");
    if (strsrch(base_name(environment(player)), "cretaceous_shore") == -1) {
        write(HIR "❌ 測試 17 失敗: 飛越峭壁進入白堊紀海岸失敗！目前位置: " + base_name(environment(player)) + "\n" NOR);
        return 1;
    }
    
    // 檢驗進度
    player->force_me("look");
    if (pm->query_current_stage(player, "main") != "cretaceous_dawn") {
        write(HIR "❌ 測試 17 失敗: 未能正確升級至 cretaceous_dawn 階段！目前: " + pm->query_current_stage(player, "main") + "\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功在狂風峭壁建立石圈控火，烹煮蜥蜴肉餵養翼龍，順利飛抵白堊紀海岸。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 18：驗證 cretaceous_dawn 階段（直面霸王龍與感知極限）
    // ----------------------------------------------------
    write(CYN "🧪 [測試 18] 測試 cretaceous_dawn 階段 (cretaceous_shore)...\n" NOR);
    player->force_me("freeze");
    
    if (!player->has_factor("tyrex_senses")) {
        write(HIR "❌ 測試 18 失敗: 避開霸王龍後未能領悟 tyrex_senses！\n" NOR);
        return 1;
    }
    
    player->force_me("go ridge");
    if (strsrch(base_name(environment(player)), "settlement_ridge") == -1) {
        write(HIR "❌ 測試 18 失敗: 前往聚落山脊失敗！目前位置: " + base_name(environment(player)) + "\n" NOR);
        return 1;
    }
    
    // 檢驗進度
    player->force_me("look");
    if (pm->query_current_stage(player, "main") != "last_stand") {
        write(HIR "❌ 測試 18 失敗: 未能正確升級至 last_stand 終局階段！目前: " + pm->query_current_stage(player, "main") + "\n" NOR);
        return 1;
    }
    write(HIG "  ✓ 通過: 成功以極限靜止偽裝避開霸王龍，繞行抵達黃土山脊聚落。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 19：驗證 last_stand 終局（陶器燒製、文字刻畫與文明遺產）
    // ----------------------------------------------------
    write(CYN "🧪 [測試 19] 測試 last_stand 終局階段 (settlement_ridge / ancestor_cave)...\n" NOR);
    // 製作陶器
    player->force_me("bake clay");
    if (!player->has_factor("pottery_craft")) {
        write(HIR "❌ 測試 19 失敗: 燒製黏土未能領悟 pottery_craft！\n" NOR);
        return 1;
    }
    
    // 前往祖先溶洞
    player->force_me("go cave");
    if (strsrch(base_name(environment(player)), "ancestor_cave") == -1) {
        write(HIR "❌ 測試 19 失敗: 進入祖先洞穴失敗！\n" NOR);
        return 1;
    }
    
    // 模擬獲得黑曜石獵刀 (obsidian_knife)
    object knife = clone_object("/std/item.c");
    knife->set_item_id("obsidian_knife");
    knife->set_name("精製黑曜石獵刀");
    move_object(knife, player);
    
    player->force_me("draw wall");
    if (!player->has_factor("symbol_abstraction")) {
        write(HIR "❌ 測試 19 失敗: 刻寫岩壁未能領悟 symbol_abstraction！\n" NOR);
        return 1;
    }
    
    player->force_me("build altar");
    if (!player->has_factor("ritual_altar")) {
        write(HIR "❌ 測試 19 失敗: 構築祭壇未能領悟 ritual_altar！\n" NOR);
        return 1;
    }
    
    player->force_me("write legacy");
    if (!player->has_factor("civilization_meaning")) {
        write(HIR "❌ 測試 19 失敗: 刻下文明遺產未能領悟 civilization_meaning！\n" NOR);
        return 1;
    }
    
    write(HIG "  ✓ 通過: 成功在末日前夕燒製陶罐、在溶洞中建立祭壇並刻下文明符號記憶，終局挑戰圓滿完成。\n" NOR);

    // ----------------------------------------------------
    // 🧪 測試 20：驗證中文指令正規化與同義詞對照轉換服務
    // ----------------------------------------------------
    write(CYN "🧪 [測試 20] 測試中文指令別名翻譯與別名輸入對照...\n" NOR);
    // 重設 civilization_meaning 因子，以便透過中文重新觸發解鎖
    object fs = load_object("/runtime/services/factor_service.c");
    if (fs) {
        fs->remove_factor(player, "civilization_meaning");
    }
    
    // 測試帶空格與不帶空格的中文指令輸入
    player->force_me("寫下傳承"); // 不帶空格，應翻譯為 "write legacy"
    
    if (!player->has_factor("civilization_meaning")) {
        write(HIR "❌ 測試 20 失敗: 中文指令 '寫下傳承' 未能正確解析並觸發解鎖！\n" NOR);
        return 1;
    }
    
    write(HIG "  ✓ 通過: 中文別名『寫下傳承』成功正規化並觸發對應英文指令解鎖。\n" NOR);

    destruct(player);
    write(HIW "\n============================================\n" +
              "  ✓ FSE PC — 史前文明所有整合測試全部通過！\n" +
              "============================================\n" NOR);
    return 0;
}
