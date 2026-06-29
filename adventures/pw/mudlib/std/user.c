// /std/user.c (PW 玩家載體)
inherit "/std/entity";

private string id;
private mapping discovered_factors;
private mapping physical_state;
private mapping progression;

void create() {
    ::create();
    set_entity_id("user:tester");
    set_entity_type("user");
    discovered_factors = ([]);
    physical_state = ([ "memory": 100 ]);
    progression = ([]);
}

void set_id(string new_id) { id = new_id; }
string get_id() { return id; }

void save_state() {
    if (!id || id == "") return;
    if (file_size("/data/pw/state/players/") < 0) {
        mkdir("/data/pw/state/players/");
    }
    save_object("/data/pw/state/players/" + id);
}

void restore_state() {
    if (!id || id == "") return;
    if (file_size("/data/pw/state/players/" + id + ".o") > 0) {
        restore_object("/data/pw/state/players/" + id);
    }
    if (!discovered_factors) discovered_factors = ([]);
    if (!physical_state) physical_state = ([ "memory": 100 ]);
    if (!progression) progression = ([]);
}

mapping query_progression() {
    if (!progression) progression = ([]);
    return progression;
}

void set_progression(mapping prog) {
    progression = prog;
    save_state();
}

string *query_discovered_factors() { 
    return keys(discovered_factors); 
}

mapping query_discovered_factors_metadata() {
    return discovered_factors;
}

varargs void discover_factor(string fid, mapping metadata) {
    if (!discovered_factors) discovered_factors = ([]);
    if (!discovered_factors[fid]) {
        if (!metadata) metadata = ([]);
        if (undefinedp(metadata["unlocked_at"])) {
            metadata["unlocked_at"] = time();
        }
        discovered_factors[fid] = metadata;
        save_state();
    }
}

int has_factor(string fid) {
    if (!discovered_factors) return 0;
    return !undefinedp(discovered_factors[fid]);
}

// 兼容舊架構接口
string *query_unlocked_discoveries() { return query_discovered_factors(); }
void add_unlocked_discovery(string did) { discover_factor(did); }
int has_discovery(string did) { return has_factor(did); }

void add_physical_state(string state, int val) {
    if (!physical_state) physical_state = ([]);
    physical_state[state] = (physical_state[state] || 100) + val;
    save_state();
}

int query_physical_state(string state) {
    if (!physical_state) return 0;
    return physical_state[state] || 0;
}

// 權限 UID 相容
string query_role() { return "god"; }
string *query_write_paths() { return ({ "/" }); }

string password_hash;

void get_account(string acc);
void check_password(string pwd);
void new_password(string pwd);

void logon() {
    write("請輸入帳號 (Nickname): \n");
    input_to("get_account");
}

void get_account(string acc) {
    acc = trim(acc);
    if (!acc || acc == "") {
        write("請輸入帳號: \n");
        input_to("get_account");
        return;
    }
    set_id(acc);
    if (file_size("/data/pw/state/players/" + acc + ".o") > 0) {
        restore_state();
        write("請輸入密碼: \n");
        input_to("check_password");
    } else {
        write("新帳號，請設定密碼: \n");
        input_to("new_password");
    }
}

void check_password(string pwd) {
    pwd = trim(pwd);
    if (pwd != password_hash) {
        write("密碼錯誤！請重新輸入: \n");
        input_to("check_password");
        return;
    }
    set_living_name(get_id());
    write("登入成功！歡迎回來，" + get_id() + "！\n");
    
    // 🚀 根據當前世界進程階段動態決定玩家的初始房間
    string dest = load_object("/runtime/services/progress_manager.c")->query_spawn_node(this_object(), "main");
    if (!dest) {
        // PW-specific fallback: 避免未設定 spawn_node 時無處可去
        dest = "/nodes/infinite_loop_swamp/node";
    }
    
    object dest_obj = load_object(dest);
    if (dest_obj) {
        catch(move_object(this_object(), dest_obj));
    }
    
    process_input("execute {\"type\":\"REQUEST_TOOLBOX\"}");
}

void new_password(string pwd) {
    pwd = trim(pwd);
    if (!pwd || pwd == "") {
        write("密碼不能為空，請重新設定: \n");
        input_to("new_password");
        return;
    }
    password_hash = pwd;
    save_state();
    set_living_name(get_id());
    write("註冊成功！歡迎加入，" + get_id() + "！\n");
    
    // 🚀 根據當前世界進程階段動態決定玩家的初始房間
    string dest = load_object("/runtime/services/progress_manager.c")->query_spawn_node(this_object(), "main");
    if (!dest) {
        // PW-specific fallback: 避免未設定 spawn_node 時無處可去
        dest = "/nodes/infinite_loop_swamp/node";
    }
    
    object dest_obj = load_object(dest);
    if (dest_obj) {
        catch(move_object(this_object(), dest_obj));
    }
    
    process_input("execute {\"type\":\"REQUEST_TOOLBOX\"}");
}

// 模擬測試與真實連線的指令分發攔截 (由 Go 驅動優先呼叫)
mixed process_input(string cmd) {
    cmd = trim(cmd);
    write_file("/data/pw/state/system/test_execute.txt", sprintf("[%s] CMD received: %s\n", ctime(time()), cmd), 0);
    
    if (cmd == "logon") {
        logon();
        return 1;
    }
    
    // 只切第一個空白，arg 保留所有剩餘內容（含 JSON payload）
    string verb, arg;
    int sp = strsrch(cmd, " ");
    if (sp == -1) {
        verb = cmd;
        arg  = "";
    } else {
        verb = cmd[0..sp-1];
        arg  = trim(cmd[sp+1..]);
    }
    
    // 動態分派指令，避免直接 hardcode /cmds 裡面的具體指令
    object cmd_ob = load_object("/cmds/player/" + verb + ".c");
    if (cmd_ob) {
        write_file("/data/pw/state/system/test_execute.txt", sprintf("[%s] Routed to %s.c with arg: %s\n", ctime(time()), verb, arg), 0);
        cmd_ob->main(this_object(), arg, 0);
        return 1;
    }
    
    return 0;
}

int force_me(string cmd) {
    return process_input(cmd);
}
