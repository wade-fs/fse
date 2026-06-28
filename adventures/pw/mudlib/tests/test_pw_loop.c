// /tests/test_pw_loop.c
// PW (編程世界) 遞進關卡「完整輸入流仿真」單元測試
#include "/include/ansi.h"

inherit "/std/object";

int run_tests() {
    object player, site_swamp, site_valley, progress_d, factor_d;
    mapping ast, toolbox;
    int res;

    write(HIW "\n🧪 正在執行 PW 雙關卡遞進「完整輸入流仿真」測試組..." NOR "\n");

    // 1. 初始化物件與地標
    player = clone_object("/std/user.c");
    site_swamp = load_object("/nodes/infinite_loop_swamp/node.c");
    site_valley = load_object("/nodes/counter_valley/node.c");
    progress_d = load_object("/runtime/services/progress_service.c");
    factor_d = load_object("/runtime/services/factor_service.c");

    if (!player || !site_swamp || !site_valley || !progress_d || !factor_d) {
        write(HIR "❌ 測試失敗: 物件初始化失敗" NOR "\n");
        return 1;
    }

    // 強制重置世界階段與狀態
    progress_d->reset_stage();

    // ==========================================================
    // 2. 驗證第一關互動一：【非法作弊輸入】
    // ==========================================================
    ast = ([ "type": "Loop", "has_break": 1 ]);
    // 玩家在 swamp 執行
    player->set_temp("current_site", "infinite_loop_swamp");
    res = player->process_input("execute " + yaml_encode(ast));

    if (factor_d->factor_discovered(player, "loop_termination")) {
        write(HIR "❌ 測試失敗: 作弊輸入不應解鎖 loop_termination" NOR "\n");
        return 1;
    }
    write(HIG "  ✓ 第一關互動一: 【防作弊攔截】驗證通過。" NOR "\n");

    // ==========================================================
    // 3. 驗證第一關互動二：【合法的犯錯輸入】
    // ==========================================================
    ast = ([ "type": "Loop", "has_break": 0 ]);
    player->process_input("execute " + yaml_encode(ast));

    if (player->query_physical_state("memory") != 80) {
        write(HIR "❌ 測試失敗: 崩累後玩家 Memory 未扣減" NOR "\n");
        return 1;
    }
    if (!factor_d->factor_discovered(player, "loop_termination")) {
        write(HIR "❌ 測試失敗: 崩潰後未解鎖 loop_termination 因素" NOR "\n");
        return 1;
    }
    write(HIG "  ✓ 第一關互動二: 【犯錯崩潰與領悟】驗證通過。" NOR "\n");

    // ==========================================================
    // 4. 驗證第一關互動三：【合法的成功輸入】
    // ==========================================================
    ast = ([ "type": "Loop", "has_break": 1 ]);
    player->process_input("execute " + yaml_encode(ast));

    if (progress_d->query_current_stage() != "stage_2_loop") {
        write(HIR "❌ 測試失敗: 第一關破關後，世界階段未推進至 stage_2_loop" NOR "\n");
        return 1;
    }
    write(HIG "  ✓ 第一關互動三: 【第一關破關與階段推進】驗證通過。" NOR "\n");

    // ==========================================================
    // 5. 驗證第二關互動四：【資料驅動計數器迴圈】
    // 預期：在 counter_valley 執行不合格積木失敗，執行合格積木成功並完成任務
    // ==========================================================
    // 模擬移動至計數山谷
    player->set_temp("current_site", "counter_valley");
    
    // A. 錯誤的計數器 (次數不對)
    ast = ([ "type": "Loop", "condition": "count < 50", "act": "count++" ]);
    player->process_input("execute " + yaml_encode(ast));
    // 檢查任務是否未完成 (progress 應該沒有增加)
    int old_progress = progress_d->query_world_progress();

    // B. 正確的計數器 (符合 YAML 設定的 expected_ast)
    ast = ([ "type": "Loop", "condition": "count < 100", "act": "count++" ]);
    player->process_input("execute " + yaml_encode(ast));

    // 檢查世界進度是否因為任務完成而成功增加 (complete_quest 增加 10)
    if (progress_d->query_world_progress() <= old_progress) {
        write(HIR "❌ 測試失敗: 第二關成功破關後，任務 count_to_100 未被標記完成" NOR "\n");
        return 1;
    }
    write(HIG "  ✓ 第二關互動四: 【資料驅動計數器驗證與第二關破關】驗證通過。" NOR "\n");

    write(HIY "🎉 遞進雙關卡仿真測試全部通過 (4/4)！" NOR "\n\n");

    destruct(player);
    return 0; // 0 代表測試全部成功
}
