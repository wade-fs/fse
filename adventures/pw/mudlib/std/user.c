// /std/user.c (PW 玩家載體)
inherit "/std/entity";

private string id;
private string *discovered_factors;
private mapping physical_state;

void create() {
    ::create();
    set_entity_id("user:tester");
    set_entity_type("user");
    discovered_factors = ({});
    physical_state = ([ "memory": 100 ]);
}

void set_id(string new_id) { id = new_id; }
string get_id() { return id; }

string *query_discovered_factors() { return discovered_factors; }

void discover_factor(string fid) {
    if (member_array(fid, discovered_factors) == -1) {
        discovered_factors += ({ fid });
    }
}

int has_factor(string fid) {
    if (!discovered_factors) return 0;
    return member_array(fid, discovered_factors) != -1;
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
    if (strsrch(cmd, "execute ") == 0) {
        string arg = substr(cmd, 8, strlen(cmd));
        load_object("/cmds/player/execute.c")->main(this_object(), arg, 0);
        return 1; // 回傳 1 代表已完全處理，阻止 Go 印出 "什麼？"
    }
    return 0;
}

int force_me(string cmd) {
    return process_input(cmd);
}
