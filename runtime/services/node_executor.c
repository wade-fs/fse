// /runtime/services/node_executor.c
// FSE 通用關卡執行判定引擎 (Node Executor Service)
// 職責：
//   - 讀取關卡的 challenges 清單
//   - 依據定義的 executor（例如 ast_matcher）對輸入積木進行校驗與執行效果處理。
#include "/include/ansi.h"

// 輔助函式：根據 JSONPath 風格（簡單支援 $.key）取得值
mixed get_ast_value(mapping ast, string path) {
    if (!ast || !path) return 0;
    if (path[0..1] == "$.") {
        return ast[path[2..]];
    }
    return ast[path];
}

// 彈性 AST 匹配器
int match_ast(mapping ast, mapping expected) {
    if (!expected) return 0;
    string matcher = expected["matcher"];
    
    // 如果未指定 matcher，相容舊版直接比對
    if (!matcher || matcher == "basic") {
        if (expected["type"] && ast["type"] != expected["type"]) return 0;
        if (expected["condition"] && ast["condition"] != expected["condition"]) return 0;
        if (expected["act"] && ast["act"] != expected["act"]) return 0;
        if (!undefinedp(expected["has_break"]) && ast["has_break"] != expected["has_break"]) return 0;
        return 1;
    }
    
    if (matcher == "rule_based") {
        mixed *rules = expected["rules"];
        if (!arrayp(rules)) return 0;
        
        foreach(mapping rule in rules) {
            string path = rule["path"];
            mixed actual_val = get_ast_value(ast, path);
            
            if (!undefinedp(rule["value"])) {
                if (actual_val != rule["value"]) return 0;
            }
            if (!undefinedp(rule["contains"])) {
                if (stringp(actual_val) && strsrch(actual_val, rule["contains"]) == -1) return 0;
                if (arrayp(actual_val) && member_array(rule["contains"], actual_val) == -1) return 0;
            }
        }
        return 1;
    }
    
    if (matcher == "custom_matcher") {
        string func = expected["function"];
        string file = expected["file"];
        if (func && file) {
            object ob = load_object(file);
            if (ob) return call_other(ob, func, ast, expected);
        }
        return 0;
    }
    
    return 0;
}

// 執行指定關卡的挑戰判定。傳入當前地標物件、玩家物件與前端傳回的積木 AST
int execute_challenges(object node_obj, object player, mapping ast) {
    if (!node_obj || !player || !ast) return 0;

    mapping config = node_obj->query_node_config();
    string node_dir = node_obj->query_node_dir();
    if (!config || !node_dir) return 0;

    object i18n = load_object("/runtime/services/i18n_service.c");
    if (!i18n) return 0; // 若無法載入翻譯服務則中斷

    mixed *challenges = config["challenges"];
    if (!challenges || !sizeof(challenges)) {
        write(i18n->translate("core.executor.no_challenges"));
        return 1;
    }

    foreach (mapping chal in challenges) {
        string cid = chal["id"];
        string executor = chal["executor"];

        if (executor == "ast_matcher") {
            // 載入該挑戰的詳細 YAML
            string chal_path = sprintf("%schallenges/%s.yaml", node_dir, cid);
            if (file_size(chal_path) <= 0) continue;

            string raw = read_file(chal_path);
            if (!raw) continue;

            mapping chal_data = yaml_decode(raw);
            if (!chal_data) continue;

            write(i18n->translate("core.executor.analyzing"));

            mapping expected = chal_data["expected_ast"];
            if (!expected) {
                write(i18n->translate("core.executor.missing_expected"));
                return 0;
            }

            // 檢查 required_factor
            string req_factor = chal_data["required_factor"];
            if (req_factor && ast["has_break"] == 1) {
                int has_factor = load_object("/runtime/services/factor_service.c")->factor_discovered(player, req_factor);
                if (!has_factor) {
                    write(i18n->translate("core.executor.missing_factor", ([ "factor_name": req_factor ])));
                    return 0;
                }
            }

            // 檢查 AST 匹配 (使用新的彈性匹配器)
            int matched = match_ast(ast, expected);

            if (matched) {
                string success_msg = chal_data["success_msg"];
                if (!success_msg) {
                    success_msg = "🎉 挑戰成功！\n";
                }
                tell_object(player, HIG + success_msg + NOR);
                
                // 呼叫進度管理器完成任務
                load_object("/runtime/services/progress_manager.c")->complete_quest(player, cid);
                return 1;
            }

            // 匹配失敗，執行懲罰與提示
            mapping consequence = chal_data["failure_consequence"];
            if (consequence) {
                if (consequence["memory_cost"]) {
                    player->add_physical_state("memory", -consequence["memory_cost"]);
                }
                if (consequence["failure_flag"]) {
                    player->set_temp("failure_history/" + consequence["failure_flag"], 1);
                }
                if (consequence["discover_factor"]) {
                    load_object("/runtime/services/factor_service.c")->discover_factor(player, consequence["discover_factor"]);
                }
                if (consequence["error_warning"]) {
                    write(RED + consequence["error_warning"] + NOR);
                }
            } else {
                write(i18n->translate("core.executor.semantic_error"));
                write(i18n->translate("core.executor.semantic_hint"));
            }
            return 0;
        } else {
            write(i18n->translate("core.executor.unknown_type", ([ "executor": executor ])));
            return 0;
        }
    }

    return 0;
}
