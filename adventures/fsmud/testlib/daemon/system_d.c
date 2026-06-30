// /daemon/system_d.c
// 系統核心守護進程，處理全域設定（如 mudlib_id）

inherit "/std/object";

string mudlib_name;
string mudlib_id;

void create() {
    ::create();
    if (!restore_object("/data/system.o")) {
        // 如果沒有存檔，回退到預設值，但通常會在第一次登入時被設定
        mudlib_name = "unnamed mud";
        mudlib_id = "unnamed.mud";
    }
}

string query_mudlib_name() {
    return mudlib_name;
}

string query_mudlib_id() {
    return mudlib_id;
}

void set_mudlib_name(string name, string id) {
    mudlib_name = name;
    mudlib_id = id;
    // 這裡我們需要使用本物件來取得寫入權限，但 efun 預設會拿 current_player
    // 我們在 valid.c 中已經特別放行了 login.c，所以可以存檔
    save_object("/data/system.o");
}
