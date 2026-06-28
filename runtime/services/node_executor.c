// /runtime/services/node_executor.c
// FSE 通用關卡執行判定引擎 (Node Executor Service)
// 職責：
//   - 讀取關卡的 challenges 清單
//   - 依據定義的 executor（例如 ast_matcher）對輸入積木進行校驗與執行效果處理。
#include "/include/ansi.h"

// 執行指定關卡的挑戰判定。傳入當前地標物件、玩家物件與前端傳回的積木 AST
int execute_challenges(object node_obj, object player, mapping ast) {
    if (!node_obj || !player || !ast) return 0;

    mapping config = node_obj->query_node_config();
    string node_dir = node_obj->query_node_dir();
    if (!config || !node_dir) return 0;

    mixed *challenges = config["challenges"];
    if (!challenges || !sizeof(challenges)) {
        write("ℹ️ 此地標目前沒有需要執行的挑戰。\n");
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

            write("💡 虛擬機正在分析積木結構...\n");

            mapping expected = chal_data["expected_ast"];
            if (!expected) {
                write("❌ 執行失敗：挑戰設定檔中缺少預期 AST 欄位。\n");
                return 0;
            }

            // 檢查 required_factor
            string req_factor = chal_data["required_factor"];
            if (req_factor && ast["has_break"] == 1) {
                int has_factor = load_object("/runtime/services/factor_service.c")->factor_discovered(player, req_factor);
                if (!has_factor) {
                    write("❌ 執行失敗：你目前的工具箱中並不存在 [中斷] 積木！請勿越級組裝。\n");
                    return 0;
                }
            }

            // 檢查 AST 匹配
            int matched = 1;
            if (ast["type"] != expected["type"]) {
                matched = 0;
            } else {
                if (expected["condition"] && ast["condition"] != expected["condition"]) {
                    matched = 0;
                }
                if (expected["act"] && ast["act"] != expected["act"]) {
                    matched = 0;
                }
                if (expected["has_break"] != 0 && !undefinedp(expected["has_break"])) {
                    if (ast["has_break"] != expected["has_break"]) {
                        matched = 0;
                    }
                }
            }

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
                write(RED "🔥 [語意錯誤] 積木邏輯未對齊預期條件！\n" NOR);
                write("⚡ 提示：請重新檢查你的迴圈條件與更新陳述！\n");
            }
            return 0;
        } else {
            write("❌ 未知的執行器類型：" + executor + "\n");
            return 0;
        }
    }

    return 0;
}
