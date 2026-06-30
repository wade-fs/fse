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
    object factor_svc   = load_object("/runtime/services/factor_service.c");
    object i18n_svc     = load_object("/runtime/services/i18n_service.c");

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
            // 4. 註冊宣告式虛擬物件規則
            if (manifest["virtual_rules"]) {
                object virtual_core = load_object("/runtime/core/virtual.c");
                if (virtual_core) {
                    foreach (string prefix, string std_file in manifest["virtual_rules"]) {
                        virtual_core->register_virtual_rule(prefix, std_file);
                    }
                }
            }
            write("  [master] 成功讀取 /manifest.yaml 並完成宣告式註冊。\n");
        }
    } else {
        write("  [master] 警告：找不到 /manifest.yaml 檔案，請檢查配置。\n");
        // 警告後降級為預設硬編碼註冊
        object virtual_core = load_object("/runtime/core/virtual.c");
        if (virtual_core) {
            virtual_core->register_virtual_rule("rooms", "/std/room.c");
            virtual_core->register_virtual_rule("monsters", "/std/monster.c");
        }
    }

    // 測試模式
    if (getenv("MUD_TEST_MODE")) {
        call_out("run_test_mode", 1);
    }
}

void run_test_mode() {
    write("[TEST] 開始執行史前文明 (PC) 整合測試...\n");
    object test_suite = load_object("/tests/test_pc_survival.c");
    if (!test_suite) {
        write(HIR "❌ 無法載入測試套件 /tests/test_pc_survival.c\n" NOR);
        shutdown(1);
        return;
    }
    int result = test_suite->run_all_tests();
    if (result != 0) {
        write(HIR "❌ 測試失敗！\n" NOR);
        shutdown(1);
    } else {
        write(HIG "🏆 所有測試成功通過！\n" NOR);
        shutdown(0);
    }
}

string get_root_uid()   { return "Root"; }
string get_bb_uid()     { return "Backbone"; }
string get_simul_efun() { return "/secure/simul_efun.c"; }

// 連線入口
object connect(string token) {
    object user_ob = clone_object("/std/user.c");
    return user_ob;
}

// 支援完全資料驅動 (Virtual Object) 機制
object compile_object(string file) {
    return load_object("/runtime/core/virtual.c")->compile_virtual_object(file);
}
