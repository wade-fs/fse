// /cmds/admin/cmd_tests.c
// 極簡單元測試啟動指令
#include "/include/ansi.h"

inherit "/std/object";

void create() {
    ::create();
}

int main(object me, string arg, string extra) {
    object test_ob;
    int failures = 0;

    write("🧪 開始執行 PW 單元測試自動化套件...\n");

    // 載入並執行 /tests/test_pw_loop.c
    test_ob = load_object("/tests/test_pw_loop.c");
    if (test_ob) {
        failures = test_ob->run_tests();
    } else {
        write(HIR "❌ 無法載入測試檔案 /tests/test_pw_loop.c" NOR "\n");
        failures = 1;
    }

    if (failures == 0) {
        write(HIG "✨ [SUCCESS] 所有 PW 測試 100% 通過！" NOR "\n");
    } else {
        write(HIR "💥 [FAILURE] 測試失敗！" NOR "\n");
    }

    return failures;
}
