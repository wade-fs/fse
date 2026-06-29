// /std/user.c (PW 玩家載體)
inherit "/std/entity";

private string id;
private mapping discovered_factors;
private mapping physical_state;

void create() {
    ::create();
    set_entity_id("user:tester");
    set_entity_type("user");
    discovered_factors = ([]);
    physical_state = ([ "memory": 100 ]);
}

void set_id(string new_id) { id = new_id; }
string get_id() { return id; }

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
}

int query_physical_state(string state) {
    if (!physical_state) return 0;
    return physical_state[state] || 0;
}

// 權限 UID 相容
string query_role() { return "god"; }
string *query_write_paths() { return ({ "/" }); }

// 模擬測試與真實連線的指令分發攔截 (由 Go 驅動優先呼叫)
mixed process_input(string cmd) {
    cmd = trim(cmd);
    write_file("/data/state/system/test_execute.txt", sprintf("[%s] CMD received: %s\n", ctime(time()), cmd), 0);
    if (strsrch(cmd, "execute") == 0) {
        string arg = "";
        if (strlen(cmd) > 7) {
            arg = cmd[7..];
            arg = trim(arg);
        }
        write_file("/data/state/system/test_execute.txt", sprintf("[%s] Routed to execute.c with arg: %s\n", ctime(time()), arg), 0);
        load_object("/cmds/player/execute.c")->main(this_object(), arg, 0);
        return 1; // 回傳 1 代表已完全處理，阻止 Go 印出 "什麼？"
    }
    return 0;
}

int force_me(string cmd) {
    return process_input(cmd);
}
