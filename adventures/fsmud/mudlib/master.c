// master.c
// 這是 MUD 引擎啟動後第一個載入的總管物件

inherit "/secure/valid.c";

void create() {
    ::create();
    // 伺服器啟動時，可以在這裡做一些全域的初始化
    write("===================================\n");
    write("  MudScript Master Object 啟動成功 \n");
    write("===================================\n");

    // 啟動全域守護進程
    load_object("/secure/event_d.c");
    load_object("/daemon/timeline_d.c");
    load_object("/daemon/settlement_d.c");
    load_object("/daemon/footprint_d.c");
    load_object("/daemon/faction_d.c");
    load_object("/daemon/historical_event_d.c");
    load_object("/daemon/era_d.c");
    load_object("/daemon/site_d.c");
    load_object("/daemon/route_d.c");
    load_object("/daemon/chronicle_d.c");

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

// 🚀 新增：告知 Driver SimulEfun 的存放路徑
string get_simul_efun() {
    return "/secure/simul_efun.c";
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
