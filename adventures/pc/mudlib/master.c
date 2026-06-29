// /master.c  (史前文明 PC)
// Prehistoric Civilization — Adventure Master
// 驗證 FSE runtime 跨 Adventure 共用能力
#include "/include/ansi.h"

inherit "/secure/valid.c";

void create() {
    ::create();
    write("===========================================\n");
    write("  FSE PC — 史前文明 (Prehistoric Civilization)\n");
    write("         三疊紀生存冒險  啟動成功\n");
    write("===========================================\n");

    // ─── 載入並配置 FSE 共用核心服務 ───
    object progress_svc = load_object("/runtime/services/progress_manager.c");
    object event_bus    = load_object("/runtime/services/event_bus.c");

    // 注入 PC 的 progression 定義目錄
    progress_svc->register_progression_path("/content/progression");

    // 注入 PC 的預設初始階段（novice），不涉及 PW 的任何概念
    progress_svc->set_default_initial_stage("main", "novice");

    write("  [runtime/progress_manager] 已載入，初始階段：novice\n");
    write("  [runtime/event_bus]        已載入\n");

    // 測試模式
    if (getenv("MUD_TEST_MODE")) {
        call_out("run_test_mode", 1);
    }
}

void run_test_mode() {
    write("[TEST] PC 史前文明啟動測試完成，無自動化測試。\n");
    shutdown(0);
}

string get_root_uid()   { return "Root"; }
string get_bb_uid()     { return "Backbone"; }
string get_simul_efun() { return "/secure/simul_efun.c"; }

// 連線入口
object connect(string token) {
    object user_ob = clone_object("/std/user.c");
    return user_ob;
}
