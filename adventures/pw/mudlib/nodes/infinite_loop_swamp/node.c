// /adventures/pw/nodes/infinite_loop_swamp/node.c
// 無窮迴圈沼澤節點邏輯 (繼承 FSE 通用 Node)
#include "/include/ansi.h"

inherit "/fse/std/node";

void create() {
    ::create();
    set_entity_id("node:infinite_loop_swamp");

    // 註冊條件顯示層 (Reveal Layer)：當玩家探索解鎖 loop_termination 因素時，浮現 break_block 工具
    add_reveal_layer(([
        "reveal_block" : "break_block",
        "checks" : ({
            ([
                "type" : "factor_discovered",
                "factor_id" : "loop_termination"
            ])
        })
    ]));
}

int receive_execution(object player, mapping ast) {
    if (!player || !ast) return 0;

    write("💡 虛擬機正在分析積木結構...\n");

    // 🚀 資料驅動：從通用 mapping 目錄載入挑戰預期配置
    string quest_content = read_file("/mapping/challenges/infinite_loop_termination.yaml");
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

    // 1. 比對積木類型
    if (ast["type"] != expected["type"]) {
        write("❌ 執行失敗：積木語法無法在此地標生效。\n");
        return 0;
    }

    // 2. 因果判定：如果玩家尚未發現 loop_termination
    int has_factor = load_object("/fse/engine/factor_d.c")->factor_discovered(player, "loop_termination");

    // 3. 比對是否滿足 expected_ast 條件
    if (ast["has_break"] == 1) {
        if (!has_factor) {
            write("❌ 執行失敗：你目前的工具箱中並不存在 [中斷] 積木！請勿越級組裝。\n");
            return 0;
        }

        // 成功破關！
        tell_object(player, HIG "🎉 很棒！你成功用 [中斷] 積木修復了無窮迴圈，挽救了即將崩潰的記憶體！執行成功！\n" NOR);
        return 1;
    }

    // 4. 玩家未加中斷，導致崩潰 (Consequence)
    player->add_physical_state("memory", -20);
    player->set_temp("failure_history/memory_leak_crash", 1);

    write(RED "🔥 [錯誤警告] 重複執行無終止條件！\n" NOR);
    write("💥 程式進入無窮迴圈！系統內存正在急速流失...\n");
    write("💥 玩家體力 (Memory) 扣減 20 點！(當前剩餘: " + player->query_physical_state("memory") + "%)\n");
    write("⚡ 提示：似乎需要一種能夠在滿足條件時「跳出」或「中斷」迴圈的控制積木...\n");

    // 觸發因素探索解鎖
    load_object("/fse/engine/factor_d.c")->discover_factor(player, "loop_termination");

    return 0;
}
