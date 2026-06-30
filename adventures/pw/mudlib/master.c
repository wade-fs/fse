// /master.c (PW 冒險總管)
#include "/runtime/include/ansi.h"

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

    // === 自動讀取 manifest.yaml 宣告式配置 ===
    string raw = read_file("/manifest.yaml");
    if (raw) {
        mapping manifest = yaml_decode(raw);
        if (manifest) {
            mapping content_paths = manifest["content_paths"];
            string init_stage = manifest["initial_stage"];

            // 1. 註冊多語系目錄並載入語系
            if (content_paths && content_paths["locales"]) {
                i18n_svc->register_locale_path(content_paths["locales"]);
                i18n_svc->set_language("zh_TW"); // 預設中文
                i18n_svc->reload_language();
            }

            // 2. 註冊因素探索目錄
            if (content_paths && content_paths["factors"]) {
                mixed factors = content_paths["factors"];
                if (stringp(factors)) {
                    factor_svc->register_discovery_path(factors);
                } else if (arrayp(factors)) {
                    foreach (string path in factors) {
                        factor_svc->register_discovery_path(path);
                    }
                }
            }

            // 3. 註冊進度階段與設定初始階段
            if (content_paths && content_paths["progression"] && init_stage) {
                progress_svc->register_progression_path(content_paths["progression"]);
                progress_svc->set_default_initial_stage(0, init_stage, "main");
            }
            write("  [master] 成功讀取 /manifest.yaml 並完成宣告式註冊。\n");
        }
    } else {
        write("  [master] 警告：找不到 /manifest.yaml 檔案，請檢查配置。\n");
    }

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
object compile_object(string file) {
    return load_object("/runtime/core/virtual.c")->compile_virtual_object(file);
}
