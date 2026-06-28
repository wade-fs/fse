// /cmds/player/execute.c
// 接收前端 Blockly AST 指令並執行的 LPC 指令
#include "/include/ansi.h"

inherit "/std/object";

void create() {
    ::create();
}

int main(object me, string arg, string extra) {
    mapping ast;
    object env;
    int res;

    if (!me) return 0;
    if (!arg || arg == "") {
        write("❌ 執行失敗：無效的積木資料。\n");
        return 1;
    }

    // 1. 解析來自前端的 JSON AST 字串 (JSON 是 YAML 子集，直接用 yaml_decode)
    mixed err = catch(ast = yaml_decode(arg));
    if (err) {
        log_file("sys_error.log", sprintf("[%s] execute.c yaml_decode error: %s\n", ctime(time()), err));
    }
    if (!ast || !mapp(ast)) {
        write("❌ 執行失敗：積木語意解析失敗。\n");
        return 1;
    }

    // 2. 獲取當前地標環境
    env = environment(me);
    if (!env) {
        string test_site = me->query_temp("current_site");
        if (test_site) {
            env = load_object("/nodes/" + test_site + "/node");
        } else {
            env = load_object("/nodes/infinite_loop_swamp/node");
        }
    }

    if (!env) {
        write("❌ 執行失敗：目前找不到執行地標。\n");
        return 1;
    }

    // 3. 呼叫地標的 receive_execution 執行判定
    res = env->receive_execution(me, ast);

    // 4. 反饋結果 (若是測試環境，也會將結果寫回 terminal)
    if (!res) {
        write(HIR "💥 [系統核心崩潰] 執行被迫中斷。\n" NOR);
    }

    // 🚀 無論成功或失敗，都同步將當前最新狀態的工具箱發送給前端 (解題/失敗解鎖即時更新)
    mapping toolbox = env->resolve_toolbox(me);
    tell_object(me, "__TOOLBOX__" + yaml_encode(toolbox));

    return 1;
}
