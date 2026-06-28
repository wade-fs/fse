// /adventures/pw/nodes/counter_valley/node.c
// 計數山谷節點邏輯 (繼承 FSE 通用 Node)
#include "/include/ansi.h"

inherit "/fse/std/node";

void create() {
    ::create();
    set_entity_id("node:counter_valley");
}

int receive_execution(object player, mapping ast) {
    if (!player || !ast) return 0;

    write("💡 虛擬機正在分析計數器迴圈結構...\n");

    // 🚀 資料驅動：從通用 mapping 目錄載入挑戰預期配置
    string quest_content = read_file("/mapping/challenges/count_to_100.yaml");
    if (!quest_content) {
        write("❌ 執行失敗：無法讀取任務設定檔。\n");
        return 0;
    }
    mapping quest_data = yaml_decode(quest_content);
    mapping expected = quest_data ? quest_data["expected_ast"] : 0;

    if (!expected) {
        write("❌ 執行失敗：任務設定檔中缺少預期 AST 欄位。\n");
        return 0;
    }

    // 1. 比對積木結構與變數條件 (資料驅動比對)
    if (ast["type"] == expected["type"] && 
        ast["condition"] == expected["condition"] && 
        ast["act"] == expected["act"]) {
        
        tell_object(player, HIG "🎉 完美！你成功組裝了計數器迴圈，程式精確循環了 100 次後退出！\n" NOR);
        
        // 觸發任務完成事件
        load_object("/fse/engine/progress_d.c")->complete_quest(player, "count_to_100");
        return 1;
    }

    // 2. 失敗判定 (條件不符)
    write(RED "🔥 [語意錯誤] 計數器邏輯未對齊預期條件！\n" NOR);
    write("⚡ 提示：請確保迴圈終止條件為 [count < 100]，且每次循環執行 [count++] 累加！\n");
    return 0;
}
