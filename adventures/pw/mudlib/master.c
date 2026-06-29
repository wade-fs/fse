// /master.c (PW 冒險總管)
#include "/include/ansi.h"

inherit "/secure/valid.c";

void create() {
    ::create();
    write("===================================\n");
    write("    FSE PW (編程世界) 啟動成功     \n");
    write("===================================\n");

    // 啟動 FSE 通用核心服務
    object factor_svc   = load_object("/runtime/services/factor_service.c");
    object progress_svc = load_object("/runtime/services/progress_manager.c");
    object i18n_svc     = load_object("/runtime/services/i18n_service.c");
    load_object("/runtime/services/discovery_service.c");

    // === PW 冒險注入 (Adventure-specific Configuration) ===
    // 注入語系檔目錄並載入預設語系
    i18n_svc->register_locale_path("/content/locales");
    i18n_svc->set_language("zh_TW");
    // 注冊因素定義的搜尋目錄 (factor_service 零路徑知識，全由此注入)
    factor_svc->register_discovery_path("/content/factors");
    factor_svc->register_discovery_path("/content/nodes/infinite_loop_swamp/discoveries");
    factor_svc->register_discovery_path("/content/nodes/counter_valley/discoveries");

    // 注冊進度階段 YAML 目錄，並設定初始階段
    progress_svc->register_progression_path("/content/progression");
    progress_svc->set_default_initial_stage("main", "stage_1_sequence");

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
object connect(string token) {
    object user_ob = clone_object("/std/user.c");
    return user_ob;
}

// 支援完全資料驅動 (Virtual Object) 機制
// 當載入 /nodes/<site>/node.c 時，若無實體檔案，會呼叫此處 compile_object
object compile_object(string file) {
    if (sscanf(file, "%s.c", file)) {}
    
    string *parts = explode(file, "/");
    // 如果路徑以斜線開頭，explode 產生的數組 parts 的第一個元素會是 ""
    int offset = (sizeof(parts) > 0 && parts[0] == "") ? 1 : 0;
    
    if (sizeof(parts) >= (3 + offset) && parts[offset] == "nodes" && parts[sizeof(parts)-1] == "node") {
        // 這是節點虛擬物件載入請求
        // 我們直接 clone_object 通用的 /std/node.c 作為虛擬物件
        object virtual_node = clone_object("/std/node.c");
        return virtual_node;
    }
    return 0;
}

