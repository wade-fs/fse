// master.c
// 這是 MUD 引擎啟動後第一個載入的總管物件

inherit "/secure/valid.c";

void create() {
    ::create();
    // 伺服器啟動時，可以在這裡做一些全域的初始化
    write("===================================\n");
    write("  MudScript Master Object 啟動成功 \n");
    write("===================================\n");

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
    }

    if (getenv("MUD_TEST_MODE")) {
        call_out("run_test_mode", 1);
    }
}

void run_test_mode() {
    object test_cmd = load_object("/cmds/admin/cmd_tests.c");
    if (test_cmd) {
        object fake_me = clone_object("/std/user.c");
        fake_me->set_id("tester");
        fake_me->set_role("god");
        int failures = test_cmd->main(fake_me, "tests", "");
        destruct(fake_me);

        // 如果失敗次數 > 0，則以結束代碼 1 關閉
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

// ==========================================
// 1. 玩家連線入口 (非常重要)
// ==========================================
// 當玩家透過 TCP 連線時，Driver 的 AcceptConnection 會呼叫此函式
object connect(string lang) {
    object login_ob;
    
    // 複製一個 login 物件來處理後續的登入流程
    login_ob = clone_object("/std/login.c");
    
    if (login_ob) {
        login_ob->set_browser_lang(lang);
    }

    // 將這個 login 物件回傳給 Driver
    return login_ob;
}

// ==========================================
// 2. 系統權限標籤 (UIDs)
// ==========================================
// Driver 啟動時會呼叫這兩個函式來確立系統最高權限的 UID
// 傳統 MUD 依賴這兩個字串來判斷某個物件是否具有讀寫核心檔案的權限

string get_root_uid() {
    return "Root";
}

string get_bb_uid() {
    return "Backbone";
}

string get_simul_efun() {
    return "/runtime/secure/simul_efun.c";
}

int is_creator(string id) {
    // 透過 get_dir 掃描是否已經有存檔
    mixed *files = get_dir("/data/user/*.o");
    if (sizeof(files) == 0) {
        return 1; // 第一個登入者自動獲得 God 權限
    }
    return 0;
}

// ==========================================
// 3. 錯誤處理攔截 (Error Handling)
// ==========================================
// 當 Evaluator 執行期間發生崩潰 (Runtime Error) 時，Driver 會呼叫此函式
// 傳入的參數是錯誤訊息字串與發生錯誤的檔案名稱
void runtime_error(string err_msg, string file) {
    write("【系統嚴重警告】執行期錯誤！\n");
    write("檔案：" + file + "\n");
    write("訊息：" + err_msg + "\n");
}

// 支援完全資料驅動 (Virtual Object) 機制
object compile_object(string file) {
    return load_object("/runtime/core/virtual.c")->compile_virtual_object(file);
}
