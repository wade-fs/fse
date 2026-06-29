// /adventures/pw/mudlib/cmds/player/execute.c
// 接收前端 Blockly EXECUTE 請求並執行，回傳標準 JSON 協定訊息
//
// 前端訊息格式（WebSocket 外殼）：
//   { "type": "cmd", "payload": "execute <JSON>" }
//   其中 <JSON> 為：
//     { "type": "EXECUTE",         "payload": { "ast": {...} } }
//   或
//     { "type": "REQUEST_TOOLBOX"                             }
//
// 後端回應（透過 tell_object，帶 __JSON_MSG__ 前綴讓 hub 直送前端）：
//   TOOLBOX_UPDATE, EXECUTION_RESULT, WORLD_STATE
#include "/include/ansi.h"

inherit "/std/object";

// blockly_service 路徑（PW 專屬，不在 runtime 下）
#define BLOCKLY_SVC "/adventures/pw/mudlib/services/blockly_service"

void create() {
    ::create();
}

// ──────────────────────────────────────────────────────────────
// 輔助：推送 JSON 訊息到前端（blockly_service 已加好 __JSON_MSG__ 前綴）
// ──────────────────────────────────────────────────────────────
private void push(object me, string json_str) {
    if (!me || !json_str) return;
    tell_object(me, json_str);
}

// ──────────────────────────────────────────────────────────────
// 取得或 fallback 當前節點環境
// ──────────────────────────────────────────────────────────────
private object get_env(object me) {
    object env = environment(me);
    if (!env) {
        string test_site = me->query_temp("current_site");
        if (test_site)
            env = load_object("/nodes/" + test_site + "/node");
        else
            env = load_object("/nodes/infinite_loop_swamp/node");
    }
    return env;
}

// ──────────────────────────────────────────────────────────────
// 處理 EXECUTE 請求
// ──────────────────────────────────────────────────────────────
private int handle_execute(object me, mapping packet) {
    object env;
    object blockly_svc;
    mapping inner_payload;
    mapping ast;

    inner_payload = packet["payload"];
    if (!inner_payload || !mapp(inner_payload)) {
        tell_object(me, "__JSON_MSG__" + json_encode(([
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
        tell_object(me, "__JSON_MSG__" + json_encode(([
            "type": "EXECUTION_RESULT",
            "payload": ([
                "success": 0,
                "message": "❌ 執行失敗：積木語意解析失敗（ast 欄位異常）。",
                "memory_cost": 0,
            ])
        ])));
        return 1;
    }

    env = get_env(me);
    if (!env) {
        tell_object(me, "__JSON_MSG__" + json_encode(([
            "type": "EXECUTION_RESULT",
            "payload": ([
                "success": 0,
                "message": "❌ 執行失敗：目前找不到執行地標。",
                "memory_cost": 0,
            ])
        ])));
        return 1;
    }

    // 執行節點挑戰判定（node_executor.c 會用 write()/tell_object() 印出文字訊息）
    // 我們在這裡接管結果，自己組裝 EXECUTION_RESULT 送給前端
    int res = env->receive_execution(me, ast);

    blockly_svc = load_object(BLOCKLY_SVC);

    // 組裝並送出 EXECUTION_RESULT
    // 注意：node_executor 的 success_msg / error_warning 已經透過 write() 送出純文字
    // 我們在這裡額外送一個結構化的 EXECUTION_RESULT 供前端動畫/狀態更新使用
    string exec_msg;
    if (res) {
        exec_msg = blockly_svc->format_execution_result(1, "", 0, ({}), me);
    } else {
        exec_msg = blockly_svc->format_execution_result(0, "", 0, ({}), me);
    }
    if (exec_msg) push(me, exec_msg);

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
    object blockly_svc = load_object(BLOCKLY_SVC);

    string toolbox_msg = blockly_svc->format_toolbox_update(me);
    if (toolbox_msg) push(me, toolbox_msg);

    // 同時推送世界狀態，讓前端初始化時可以同步 memory/progress
    string world_msg = blockly_svc->format_world_state(me);
    if (world_msg) push(me, world_msg);

    return 1;
}

// ──────────────────────────────────────────────────────────────
// 主入口：接收來自 WebSocket 橋接層的指令
//
// hub.go 路由規則：
//   前端送 { type: "cmd", payload: "execute <json_str>" }
//   → hub 呼叫 driver.ProcessCommand(pConn, "execute <json_str>")
//   → driver 解析指令動詞 "execute"，呼叫 execute.c main(me, arg, extra)
//   → arg = <json_str> = 完整 JSON 協定封包字串
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
        log_file("sys_error.log", sprintf("[%s] execute.c parse error: %s | raw_arg: %s\n",
            ctime(time()), err || "null packet", arg));
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
