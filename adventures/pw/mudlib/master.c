// /master.c (PW 冒險總管)
#include "/include/ansi.h"

inherit "/secure/valid.c";

void create() {
    ::create();
    write("===================================\n");
    write("    FSE PW (編程世界) 啟動成功     \n");
    write("===================================\n");

    // 啟動 FSE 通用核心守護進程
    load_object("/fse/engine/progress_d.c");
    load_object("/fse/engine/factor_d.c");
    load_object("/fse/engine/discovery_d.c");

    // 如果是測試模式，自動在一秒後執行測試
    if (getenv("MUD_TEST_MODE")) {
        call_out("run_test_mode", 1);
    }
}

void run_test_mode() {
    object test_cmd = load_object("/cmds/admin/cmd_tests.c");
    if (test_cmd) {
        object fake_me = clone_object("/std/user.c");
        int failures = test_cmd->main(fake_me, "tests", "");
        destruct(fake_me);
        if (failures > 0) {
            shutdown(1);
        } else {
            shutdown(0);
        }
    } else {
        write("無法載入測試指令，測試失敗。\n");
        shutdown(1);
    }
}

string get_root_uid() { return "Root"; }
string get_bb_uid() { return "Backbone"; }
string get_simul_efun() { return "/secure/simul_efun.c"; }

// 提供一個最極簡的 connect 入口，回傳玩家物件
object connect(string lang) {
    object user_ob = clone_object("/std/user.c");
    user_ob->set_id("tester");
    return user_ob;
}
