// /std/user.c (蜀山遊記 SO 玩家載體)
// 職責：玩家的身份、存檔、進度介面、以及 SO 專屬的修仙屬性。
// 刻意不在 create() 呼叫 set_heart_beat()——
// heart_beat 的啟用由 master.c 的 connect() 統一管理。
#include "/runtime/include/ansi.h"
inherit "/std/living";

private string id;
private string password_hash;
private int karma;            // 業力 / 因果值（SO 專屬）
private int spiritual_energy; // 靈力（SO 專屬）
private mapping progression;  // 進度，供 progress_manager 讀寫
private mapping factors;      // 已探索因素，供 factor_service 讀寫

private mapping observations; // key: signal_id, value: timestamp (Evidence 列表)
private mapping vows;         // key: vow_id, value: ([ "origin_node": ..., "state": "active"|"resolved" ])

void create() {
    ::create();
    set_name("修行者");
    id               = "";
    password_hash    = "";
    karma            = 0;
    spiritual_energy = 100;
    progression      = ([]);
    factors          = ([]);
    observations     = ([]);
    vows             = ([]);
}

// ── 帳號 ──────────────────────────────────────────────
void   set_id(string s)  { id = s; }
string get_id()          { return id; }
string query_entity_id() { return id ? "user:" + id : "user:unknown"; }

// ── 存檔 ──────────────────────────────────────────────
void save_state() {
    if (!id || id == "") return;
    if (file_size("/data/state/players/") < 0) mkdir("/data/state/players/");
    save_object("/data/state/players/" + id);
}

void restore_state() {
    if (!id || id == "") return;
    if (file_size("/data/state/players/" + id + ".o") > 0) {
        restore_object("/data/state/players/" + id);
    }
    if (!progression) progression = ([]);
    if (!observations) observations = ([]);
    if (!vows) vows = ([]);
}

// ── 進度（供 progress_manager 讀寫）──────────────────
mapping query_progression()        { if (!progression) progression = ([]); return progression; }
void    set_progression(mapping p) { progression = p; save_state(); }

// ── 因素（供 factor_service 讀寫）────────────────────
int has_factor(string fid) {
    if (!factors) factors = ([]);
    return !undefinedp(factors[fid]);
}
void discover_factor(string fid, mapping metadata) {
    if (!factors) factors = ([]);
    factors[fid] = metadata || ([]);
    save_state();
}
mapping query_factors() {
    if (!factors) factors = ([]);
    return factors;
}

// ── SO 專屬修仙屬性 ──────────────────────────────────
int query_karma()          { return karma; }
void add_karma(int amount) { karma += amount; save_state(); }

int query_spiritual_energy() { return spiritual_energy; }
void add_spiritual_energy(int amount) {
    spiritual_energy += amount;
    if (spiritual_energy < 0) spiritual_energy = 0;
    save_state();
}

// ── 心跳（由 master.c connect() 呼叫 enable_heart_beat 後觸發）──
void heart_beat() {
    // 業力心魔
    if (karma > 50 && random(10) == 0) {
        tell_object(this_object(), HIY "【心魔】你感到一陣莫名的煩躁，靈力運轉受阻...\n" NOR);
    }
    // Entropy：每 N 個心跳，通知 factor_service 進行衰退檢查 (約每 5 分鐘 / 300 秒)
    if ((time() % 300) == 0) {
        object fs = load_object("/runtime/services/factor_service.c");
        if (fs && function_exists("tick_entropy", fs)) {
            fs->tick_entropy(this_object());
        }
    }
}

// ── 觀察 Evidence 介面（由陣列改為 Mapping，帶有時間標記支援 session/衰退）──
mapping query_observations() {
    if (!observations) observations = ([]);
    return observations;
}

void add_observation(string sig) {
    if (!observations) observations = ([]);
    observations[sig] = time();
}

int has_observation(string sig) {
    if (!observations) observations = ([]);
    return !undefinedp(observations[sig]);
}

void clear_observation(string sig) {
    if (!observations) observations = ([]);
    map_delete(observations, sig);
}

void clear_observations() {
    observations = ([]);
}

// ── Vow（自心發願）介面 ──
void make_vow(string vow_id, string origin_node) {
    if (!vows) vows = ([]);
    vows[vow_id] = ([ "origin_node": origin_node, "state": "active", "time": time() ]);
    save_state();
}

void resolve_vow(string vow_id) {
    if (vows && vows[vow_id]) {
        vows[vow_id]["state"] = "resolved";
        add_karma(-5); // 了結因果，消業
        save_state();
    }
}

mapping query_vows() {
    return vows || ([]);
}

// ── 統一 FSE 核心與其他冒險的 physical_state 介面對接 ──
int query_physical_state(string attr) {
    if (attr == "spiritual_energy" || attr == "energy") return spiritual_energy;
    if (attr == "karma") return karma;
    return 0;
}

void set_physical_state(string attr, int val) {
    if (attr == "spiritual_energy" || attr == "energy") {
        spiritual_energy = val;
        save_state();
    } else if (attr == "karma") {
        karma = val;
        save_state();
    }
}

void add_physical_state(string attr, int val) {
    if (attr == "spiritual_energy" || attr == "energy") {
        add_spiritual_energy(val);
    } else if (attr == "karma") {
        add_karma(val);
    }
}


