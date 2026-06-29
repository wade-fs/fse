// /adventures/pw/mudlib/cmds/player/execute.c
// 接收前端 Blockly EXECUTE 請求並執行，回傳標準 JSON 協定訊息
// 支援的前端請求 type：
//   - EXECUTE       → 執行積木 AST，回傳 EXECUTION_RESULT + TOOLBOX_UPDATE
//   - REQUEST_TOOLBOX → 查詢工具箱，回傳 TOOLBOX_UPDATE
#include "/include/ansi.h"

inherit "/std/object";

void create() {
    ::create();
}

// 輔助：推送 JSON 訊息到前端
private void push(object me, string json_str) {
    if (!me || !json_str) return;
    tell_object(me, json_str);
}

// ──────────────────────────────────────────────────────────────
// 處理 EXECUTE 請求
// ──────────────────────────────────────────────────────────────
private int handle_execute(object me, mapping packet) {
    object env;
    object blockly_svc;
    mapping ast;
    mapping inner_payload;

    inner_payload = packet["payload"];
    if (!inner_payload || !mapp(inner_payload)) {
        push(me, json_encode(([
            "type": "EXECUTION_RESULT",
            "payload": ([
                "success": 0,
                "message": "❌ 執行失敗：無效的積木資料（缺少 payload）。",
                "memory_cost": 0,
            ])
        ])));
        return 1;
    }

    ast = inner_payload["ast"];
    if (!ast || !mapp(ast)) {
        push(me, json_encode(([
            "type": "EXECUTION_RESULT",
            "payload": ([
                "success": 0,
                "message": "❌ 執行失敗：積木語意解析失敗（ast 欄位異常）。",
                "memory_cost": 0,
            ])
        ])));
        return 1;
    }

    // 取得當前節點環境
    env = environment(me);
    if (!env) {
        string test_site = me->query_temp("current_site");
        if (test_site)
            env = load_object("/nodes/" + test_site + "/node");
        else
            env = load_object("/nodes/infinite_loop_swamp/node");
    }
    if (!env) {
        push(me, json_encode(([
            "type": "EXECUTION_RESULT",
            "payload": ([
                "success": 0,
                "message": "❌ 執行失敗：目前找不到執行地標。",
                "memory_cost": 0,
            ])
        ])));
        return 1;
    }

    // 執行節點挑戰判定
    int res = env->receive_execution(me, ast);

    // 使用 PW blockly_service 格式化回傳訊息
    blockly_svc = load_object("/adventures/pw/mudlib/services/blockly_service");

    // 🚀 無論成功或失敗，都同步推送最新 Toolbox（解題/失敗解鎖即時更新）
    string toolbox_msg = blockly_svc->format_toolbox_update(me);
    if (toolbox_msg) push(me, toolbox_msg);

    // 推送世界狀態（memory 更新等）
    string world_msg = blockly_svc->format_world_state(me);
    if (world_msg) push(me, world_msg);

    return 1;
}

// ──────────────────────────────────────────────────────────────
// 處理 REQUEST_TOOLBOX 請求
// ──────────────────────────────────────────────────────────────
private int handle_request_toolbox(object me, mapping packet) {
    object blockly_svc = load_object("/adventures/pw/mudlib/services/blockly_service");
    string toolbox_msg = blockly_svc->format_toolbox_update(me);
    if (toolbox_msg) push(me, toolbox_msg);

    // 同時推送世界狀態，讓前端初始化時可以同步 memory/progress
    string world_msg = blockly_svc->format_world_state(me);
    if (world_msg) push(me, world_msg);

    return 1;
}

// ──────────────────────────────────────────────────────────────
// 主入口：接收來自 WebSocket 橋接層的指令
// arg 格式：完整 JSON 字串，包含 "type" 欄位
// ──────────────────────────────────────────────────────────────
int main(object me, string arg, string extra) {
    if (!me) return 0;
    if (!arg || arg == "") {
        write("❌ 執行失敗：無效指令，缺少 JSON 資料。\n");
        return 1;
    }

    // 解析 JSON 封包（JSON 是 YAML 子集，直接用 yaml_decode）
    mapping packet;
    mixed err = catch(packet = yaml_decode(arg));
    if (err || !packet || !mapp(packet)) {
        write("❌ 執行失敗：JSON 解析失敗。\n");
        log_file("sys_error.log", sprintf("[%s] execute.c parse error: %s\n", ctime(time()), err || "null packet"));
        return 1;
    }

    string msg_type = packet["type"];
    if (!msg_type) {
        write("❌ 執行失敗：封包缺少 type 欄位。\n");
        return 1;
    }

    switch (msg_type) {
        case "EXECUTE":
            return handle_execute(me, packet);
        case "REQUEST_TOOLBOX":
            return handle_request_toolbox(me, packet);
        default:
            write("⚠️ 未知的 Blockly 指令類型：" + msg_type + "\n");
            return 1;
    }
}
