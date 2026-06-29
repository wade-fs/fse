// /runtime/services/node_executor.c
// FSE 通用關卡執行判定引擎 (Node Executor Service)
// 職責：
//   - 讀取關卡的 challenges 清單
//   - 依據定義的 executor（例如 ast_matcher）對輸入積木進行校驗與執行效果處理。
#include "/include/ansi.h"

// 輔助函式：根據 JSONPath 風格（簡單支援 $.key 與 $.key[idx].subkey）取得值
mixed get_ast_value(mapping ast, string path) {
    if (!ast || !path) return 0;
    if (path[0..1] != "$.") {
        return ast[path];
    }
    
    string clean_path = path[2..];
    string *parts = explode(clean_path, ".");
    mixed cur = ast;
    
    foreach (string part in parts) {
        if (!part || part == "") continue;
        
        string key;
        int idx;
        
        // 支援包含陣列索引的語法，例如 body[0]
        if (sscanf(part, "%s[%d]", key, idx) == 2) {
            if (!mappingp(cur)) return 0;
            cur = cur[key];
            if (!arrayp(cur) || idx < 0 || idx >= sizeof(cur)) return 0;
            cur = cur[idx];
        } else {
            if (!mappingp(cur)) return 0;
            cur = cur[part];
        }
    }
    return cur;
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
            string op = rule["operator"];
            mixed val = rule["value"];
            
            // 相容無 operator 舊格式
            if (!op) {
                if (!undefinedp(val)) {
                    if (actual_val != val) return 0;
                }
                if (!undefinedp(rule["contains"])) {
                    mixed c_val = rule["contains"];
                    if (stringp(actual_val) && strsrch(actual_val, c_val) == -1) return 0;
                    if (arrayp(actual_val) && member_array(c_val, actual_val) == -1) return 0;
                }
                continue;
            }
            
            switch (op) {
                case "eq":
                    if (actual_val != val) return 0;
                    break;
                case "ne":
                case "neq":
                    if (actual_val == val) return 0;
                    break;
                case "contains":
                    if (stringp(actual_val) && strsrch(actual_val, val) == -1) return 0;
                    if (arrayp(actual_val) && member_array(val, actual_val) == -1) return 0;
                    break;
                case "in":
                    if (!arrayp(val) || member_array(actual_val, val) == -1) return 0;
                    break;
                default:
                    return 0; // 未知運算子直接拒絕
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

            // 🚀 關鍵修改：如果前端傳來的是 Sequence 積木鏈，且挑戰預期的是單一節點（如 Loop），自動在 Sequence 中尋找該節點進行匹配
            if (ast["type"] == "Sequence" && expected["type"] != "Sequence") {
                mapping found = 0;
                foreach (mapping child in ast["body"]) {
                    if (child["type"] == expected["type"]) {
                        found = child;
                        break;
                    }
                }
                // 如果在 rules 比對模式，我們可以直接將 ast 覆寫為裡面的第一個 Loop 子積木
                if (!found && expected["matcher"] == "rule_based") {
                    foreach (mapping child in ast["body"]) {
                        if (child["type"] == "Loop") {
                            found = child;
                            break;
                        }
                    }
                }
                if (found) {
                    ast = found;
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
                int success_prog = chal_data["success_progress"];
                if (!success_prog) success_prog = 10;
                load_object("/runtime/services/progress_manager.c")->complete_player_quest(player, cid, "main", success_prog);
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
            // 嘗試載入擴充的 executor
            string executor_path = "/runtime/executors/" + executor + ".c";
            if (file_size(executor_path) > 0) {
                object exec_obj = load_object(executor_path);
                if (exec_obj) {
                    string chal_path = sprintf("%schallenges/%s.yaml", node_dir, cid);
                    mapping chal_data = ([]);
                    if (file_size(chal_path) > 0) {
                        string raw = read_file(chal_path);
                        if (raw) chal_data = yaml_decode(raw);
                    }
                    if (exec_obj->execute(node_obj, player, ast, chal_data, cid)) {
                        return 1;
                    }
                }
            } else {
                write(i18n->translate("core.executor.unknown_type", ([ "executor": executor ])));
                return 0;
            }
        }
    }

    return 0;
}
