// /tests/test_pw_loop.c
// PW (編程世界) 擴展測試套件
// 包含：單元測試、屬性測試、整合測試
#include "/runtime/include/ansi.h"

inherit "/std/object";

int test_unit_factor_unlock() {
    object factor_d = load_object("/runtime/services/factor_service.c");
    object player = clone_object("/std/user.c");
    
    if (!factor_d || !player) {
        write(HIR "❌ 單元測試失敗: 無法載入必要服務或玩家物件" NOR "\n");
        return 1;
    }

    // 1. 確保初始無 factor
    if (factor_d->factor_discovered(player, "factor_b")) {
        write(HIR "❌ 單元測試失敗: 玩家不應預設擁有 factor_b" NOR "\n");
        return 1;
    }
    
    // 2. 嘗試解鎖 factor_b (應因缺少 factor_a 前置而失敗)
    if (factor_d->discover_factor(player, "factor_b") != 0) {
        write(HIR "❌ 單元測試失敗: 缺少前置的因素應該無法被解鎖" NOR "\n");
        return 1;
    }

    // 3. 解鎖 factor_a (應成功)
    if (factor_d->discover_factor(player, "factor_a") != 1) {
        write(HIR "❌ 單元測試失敗: 解鎖 factor_a 失敗" NOR "\n");
        return 1;
    }

    // 4. 再次嘗試解鎖 factor_b (前置已足夠，應成功)
    if (factor_d->discover_factor(player, "factor_b") != 1) {
        write(HIR "❌ 單元測試失敗: 前置條件滿足後依然解鎖 factor_b 失敗" NOR "\n");
        return 1;
    }

    // 5. 驗證 Player 的 metadata 儲存
    mapping meta = player->query_discovered_factors_metadata();
    if (!mappingp(meta) || undefinedp(meta["factor_a"]) || !intp(meta["factor_a"]["unlocked_at"])) {
        write(HIR "❌ 單元測試失敗: 解鎖因素並未正確寫入解鎖時間等 metadata" NOR "\n");
        return 1;
    }

    destruct(player);
    write(HIG "  ✓ 單元測試: 【Factor 解鎖與狀態檢查、前置條件驗證】驗證通過。" NOR "\n");
    return 0;
}

int test_property_ast_matcher() {
    object executor = load_object("/runtime/services/node_executor.c");
    if (!executor) {
        write(HIR "❌ 屬性測試失敗: 無法載入 node_executor 服務" NOR "\n");
        return 1;
    }

    mapping expected_rule = ([
        "matcher": "rule_based",
        "rules": ({
            ([ "path": "$.type", "value": "Loop" ]),
            ([ "path": "$.has_break", "value": 1 ])
        })
    ]);

    // 1. 正常案例 (應通過)
    if (!executor->match_ast(([ "type": "Loop", "has_break": 1 ]), expected_rule)) {
        write(HIR "❌ 屬性測試失敗: 正常 AST 未能匹配" NOR "\n");
        return 1;
    }

    // 2. 錯誤類型 (不應通過)
    if (executor->match_ast(([ "type": "If", "has_break": 1 ]), expected_rule)) {
        write(HIR "❌ 屬性測試失敗: 錯誤的 Type 被意外匹配" NOR "\n");
        return 1;
    }

    // 3. 缺少必要屬性 (不應通過)
    if (executor->match_ast(([ "type": "Loop" ]), expected_rule)) {
        write(HIR "❌ 屬性測試失敗: 缺少屬性的 AST 被意外匹配" NOR "\n");
        return 1;
    }

    // 4. 空 AST 與亂碼 (不應崩潰且不應通過)
    if (executor->match_ast(([]), expected_rule)) return 1;
    if (executor->match_ast(([ "random_key": "random_value" ]), expected_rule)) return 1;

    // 5. 帶有無關屬性的巢狀結構 (應通過，容錯性)
    if (!executor->match_ast(([ "type": "Loop", "has_break": 1, "body": "some_code", "nested": ([ "x": 1 ]) ]), expected_rule)) {
        write(HIR "❌ 屬性測試失敗: 帶無關屬性的合法 AST 未能匹配" NOR "\n");
        return 1;
    }

    // 6. 測試全新的巢狀 JSONPath 查詢以及 in/contains 運算子
    mapping nested_ast = ([
        "type": "Loop",
        "body": ({
            ([ "type": "Break", "value": 1 ]),
            ([ "type": "Print", "text": "hello" ])
        }),
        "loop_type": "while"
    ]);

    mapping advanced_expected = ([
        "matcher": "rule_based",
        "rules": ({
            ([ "path": "$.body[0].type", "operator": "eq", "value": "Break" ]),
            ([ "path": "$.body[1].text", "operator": "contains", "value": "ell" ]),
            ([ "path": "$.loop_type", "operator": "in", "value": ({ "while", "for" }) ])
        })
    ]);

    if (!executor->match_ast(nested_ast, advanced_expected)) {
        write(HIR "❌ 屬性測試失敗: 進階巢狀 JSONPath 與運算子匹配失敗" NOR "\n");
        return 1;
    }

    // 7. 測試 in 運算子不匹配的情況 (應攔截不通過)
    mapping mismatch_ast = nested_ast + ([ "loop_type": "do_while" ]);
    if (executor->match_ast(mismatch_ast, advanced_expected)) {
        write(HIR "❌ 屬性測試失敗: 運算子 in 比對失敗卻意外通過" NOR "\n");
        return 1;
    }

    write(HIG "  ✓ 屬性測試: 【邊界案例與隨機 AST 容錯性】驗證通過。" NOR "\n");
    return 0;
}

int test_integration_progression() {
    object player, site_swamp, site_valley, site_var, progress_d, factor_d;
    mapping ast;
    int res;

    player = clone_object("/std/user.c");
    site_swamp = load_object("/nodes/infinite_loop_swamp/node");
    site_valley = load_object("/nodes/counter_valley/node");
    site_var = load_object("/nodes/variable_forest/node");
    progress_d = load_object("/runtime/services/progress_manager.c");
    factor_d = load_object("/runtime/services/factor_service.c");

    if (!player || !site_swamp || !site_valley || !site_var || !progress_d || !factor_d) {
        write(HIR "❌ 整合測試失敗: 物件初始化失敗" NOR "\n");
        return 1;
    }

    // 強制重置世界階段與狀態
    player->set_progression(([]));
    progress_d->set_initial_stage(player, "stage_1_sequence", "main"); // 帶入新版多軌道參數

    // ==========================================================
    // 互動一：【非法作弊輸入】
    // ==========================================================
    ast = ([ "type": "Loop", "has_break": 1 ]);
    player->set_temp("current_site", "infinite_loop_swamp");
    res = player->process_input("execute " + yaml_encode(ast));

    if (factor_d->factor_discovered(player, "loop_termination")) {
        write(HIR "❌ 整合測試失敗: 作弊輸入不應解鎖 loop_termination" NOR "\n");
        return 1;
    }
    write(HIG "  ✓ 整合測試 (1/5): 【防作弊攔截】驗證通過。" NOR "\n");

    // ==========================================================
    // 互動二：【合法的犯錯輸入】
    // ==========================================================
    ast = ([ "type": "Loop", "has_break": 0 ]);
    player->process_input("execute " + yaml_encode(ast));

    if (player->query_physical_state("memory") != 80) {
        write(HIR "❌ 整合測試失敗: 崩累後玩家 Memory 未扣減" NOR "\n");
        return 1;
    }
    if (!factor_d->factor_discovered(player, "loop_termination")) {
        write(HIR "❌ 整合測試失敗: 崩潰後未解鎖 loop_termination 因素" NOR "\n");
        return 1;
    }
    write(HIG "  ✓ 整合測試 (2/5): 【犯錯崩潰與領悟】驗證通過。" NOR "\n");

    // ==========================================================
    // 互動三：【合法的成功輸入】
    // ==========================================================
    ast = ([ "type": "Loop", "has_break": 1 ]);
    player->process_input("execute " + yaml_encode(ast));

    if (progress_d->query_current_stage(player, "main") != "stage_2_loop") {
        write(HIR "❌ 整合測試失敗: 第一關破關後，世界階段未推進至 stage_2_loop" NOR "\n");
        return 1;
    }
    write(HIG "  ✓ 整合測試 (3/5): 【第一關破關與階段推進】驗證通過。" NOR "\n");

    // ==========================================================
    // 互動四：【資料驅動計數器迴圈】
    // ==========================================================
    player->set_temp("current_site", "counter_valley");
    
    // A. 錯誤的計數器 (次數不對)
    ast = ([ "type": "Loop", "condition": "count < 50", "act": "count++" ]);
    player->process_input("execute " + yaml_encode(ast));
    int old_progress = progress_d->query_player_progress(player, "main");

    // B. 正確的計數器
    string old_stage_id = progress_d->query_current_stage(player, "main");
    ast = ([ "type": "Loop", "condition": "count < 100", "act": "count++" ]);
    player->process_input("execute " + yaml_encode(ast));

    if (progress_d->query_player_progress(player, "main") <= old_progress && progress_d->query_current_stage(player, "main") == old_stage_id) {
        write(HIR "❌ 整合測試失敗: 第二關成功破關後，任務未被標記完成" NOR "\n");
        return 1;
    }
    write(HIG "  ✓ 整合測試 (4/5): 【資料驅動計數器驗證與破關】驗證通過。" NOR "\n");

    // ==========================================================
    // 互動五：【變數與賦值關卡】
    // ==========================================================
    player->set_temp("current_site", "variable_forest");
    
    // A. 錯誤的變數賦值 (數值不符)
    ast = ([ "type": "Assignment", "var_name": "x", "value": 99 ]);
    player->process_input("execute " + yaml_encode(ast));
    int mid_progress = progress_d->query_player_progress(player, "main");

    // B. 正確的變數賦值 (應能完成挑戰並晉升 stage_3_variable)
    ast = ([ "type": "Assignment", "var_name": "x", "value": 42 ]);
    player->process_input("execute " + yaml_encode(ast));

    if (progress_d->query_current_stage(player, "main") != "stage_3_variable") {
        write(HIR "❌ 整合測試失敗: 變數森林關卡過關後，世界階段未推進至 stage_3_variable" NOR "\n");
        return 1;
    }
    write(HIG "  ✓ 整合測試 (5/5): 【變數與賦值過關驗證】驗證通過。" NOR "\n");

    destruct(player);
    return 0;
}

int run_tests() {
    int failed = 0;
    write(HIW "\n🧪 開始執行 PW 擴展自動化測試套件 (單元 / 屬性 / 整合)..." NOR "\n");
    
    if (test_unit_factor_unlock()) failed++;
    if (test_property_ast_matcher()) failed++;
    if (test_integration_progression()) failed++;

    if (failed > 0) {
        write(HIR "\n❌ 測試結果: 共 " + failed + " 個測試單元未通過。" NOR "\n");
        return 1;
    }
    
    write(HIY "\n🎉 所有 PW 測試單元 (單元、屬性、整合) 均順利通過！" NOR "\n\n");
    return 0;
}
