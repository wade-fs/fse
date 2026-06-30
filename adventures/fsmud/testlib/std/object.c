// std/object.c - 所有物件的基底類別
// 提供 id、名稱、描述等最基本的屬性

#include "/include/config.h"

string  name;          // 內部名稱/名稱
string  short_name;    // 短描述
string  long_name;     // 長描述
mixed   id_list;       // 識別字串陣列

// 🚀 新增：基本旗標
int     no_get;        // 不可拿取 (例如: 巨大的石頭)
int     no_drop;       // 不可掉落 (例如: 詛咒物品、重要任務道具)
int     money_value;   // 🚀 新增：如果是錢袋，紀錄金額

void create() {
    name       = "某個東西";
    short_name = "某個東西";
    long_name  = "這是一個沒有描述的東西。";
    id_list    = ({});
    no_get     = 0;
    no_drop    = 0;
}

// ── 設定函式 ────────────────────────────────────────────
void set_name(mixed n) { 
    name = n; 
    if (is_living()) {
        set_living_name(to_string(select_lang(n)));
    }
}
void set_short(mixed s)      { short_name = s; }
void set_long(mixed s)       { long_name  = s; }
void set_id(mixed ids)       { if (arrayp(ids)) id_list = ids; else id_list = ({ ids }); }
void set_no_get(int v)       { no_get = v; }
void set_no_drop(int v)      { no_drop = v; }
void set_money_value(int v)  { money_value = v; }

// ── 方向輔助 ────────────────────────────────────────────
string query_reverse_dir(string dir) {
    switch(dir) {
        case "north": return "south";
        case "south": return "north";
        case "east":  return "west";
        case "west":  return "east";
        case "up":    return "down";
        case "down":  return "up";
    }
    return "here";
}

// ── 查詢函式 ────────────────────────────────────────────
string query_name()      { return select_lang(name); }
string query_localized_name(string lang) { return select_lang(name, lang); }
string query_short()     { return select_lang(short_name); }
string query_long()      { return select_lang(long_name); }
mixed  query_id()        { return id_list; }
int    query_no_get()    { return no_get; }
int    query_no_drop()   { return no_drop; }
int    query_money_value(){ return money_value; }

// 🚀 新增：取得坐標 (若自身無坐標則向環境查詢)
mixed query_coordinate() {
    object env = environment(this_object());
    if (env) return env->query_coordinate();
    return 0;
}

// ── 取得用於指令互動的主要識別字 ─────────────────────
string query_key_id() {
	if (arrayp(id_list) && sizeof(id_list) > 0) {
        return id_list[0];
    }
    if (stringp(id_list) && id_list != "") {
        return id_list;
    }

    // 最後防線：使用 simul_efun 的 get_name
    return get_name(this_object());
}
// id(string str) - 判斷這個物件是否符合指定識別詞
int id(string str) {
    if (!str) { return 0; }
    int i;
    for (i = 0; i < sizeof(id_list); i++) {
        if (id_list[i] == str) { return 1; }
    }
    return 0;
}

// 🚀 新增：初始化掛接 (用於註冊指令 add_action)
void init() {}

// 判斷是否為生物
int is_living() { return 0; }

// catch_tell(string msg) - 收到訊息（預設不處理）
void catch_tell(string msg) {}

// 🚀 新增：標準移動函式
int move(mixed dest) {
    if (!dest) return 0;
    return move_object(this_object(), dest);
}

// ── 關鍵字互動系統 ──────────────────────────────────────
mapping temp_vars;
mapping responses;

void set_temp(string key, mixed val) {
    if (!temp_vars) temp_vars = ([]);
    temp_vars[key] = val;
}

mixed query_temp(string key) {
    if (!temp_vars) return 0;
    return temp_vars[key];
}

void delete_temp(string key) {
    if (temp_vars) m_delete(temp_vars, key);
}


void add_response(mixed keyword, mixed response) {
    if (!responses) responses = ([]);
    
    if (arrayp(keyword)) {
        int i;
        for (i = 0; i < sizeof(keyword); i++) {
            add_response(keyword[i], response);
        }
        return;
    }

    if (!responses[keyword]) responses[keyword] = ({});
    if (stringp(response) || mapp(response)) {
        responses[keyword] += ({ response });
    } else if (arrayp(response)) {
        responses[keyword] += response;
    }
}

mapping query_responses() {
    if (!responses) return ([]);
    return responses;
}

// ── 生命週期管理：垃圾回收 ─────────────────────────────
int clean_up(int inherited_count) {
    // 1. 如果是被繼承的基底類別，不清理
    if (inherited_count > 0) return 1;

    // 2. 如果是藍圖物件 (Blueprint)，且非緩存資源，則不輕易清理
    string oname = object_name(this_object());
    if (strsrch(oname, "#") == -1 && strsrch(oname, FS_CACHE_DIR) != 0) return 1;

    // 3. 如果物件目前在某個生物身上 (玩家背包或裝備中)，不清理
    object env = environment(this_object());
    if (env && living(env)) return 1;

    // 4. 如果物件內部還有東西，不清理
    if (sizeof(all_inventory(this_object())) > 0) return 1;

    // 5. 物件若閒置超過 15 分鐘 (900秒)，則自動銷毀以釋放資源
    if (query_idle(this_object()) > 900) {
        destruct(this_object());
        return 0;
    }

    return 1;
}
