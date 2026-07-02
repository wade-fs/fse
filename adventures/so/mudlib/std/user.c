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

protected int is_meditating = 0;

int query_meditating() { return is_meditating; }
void set_meditating(int val) { is_meditating = val; }

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
    object env = environment(this_object());

    // 1. 打坐與修煉恢復邏輯
    if (is_meditating) {
        float recovery_multiplier = 1.0;
        if (env && function_exists("query_virtual_config", env)) {
            mapping env_cfg = env->query_virtual_config();
            if (env_cfg && env_cfg["environmental_multipliers"]) {
                recovery_multiplier = env_cfg["environmental_multipliers"]["spiritual_recovery"] || 1.0;
            }
        }
        
        int base_recover = 5;
        int recovered = to_int(base_recover * recovery_multiplier);
        add_spiritual_energy(recovered);
        tell_object(this_object(), HIC "【 🧘 冥想 】你沉入吐納之中，引導天地靈氣流轉，靈力恢復了 " + recovered + " 點。\n" NOR);

        // 2. 隨機悟性 (Insight) 機制 (15% 機率發現微風中的一絲道韻)
        if (random(100) < 15) {
            string insight_sig = "wind_insight";
            if (!this_object()->has_observation(insight_sig)) {
                this_object()->add_observation(insight_sig);
                tell_object(this_object(), MAG "【 💡 頓悟 】你在冥想中忽然捕捉到耳邊風聲中夾雜的一絲自然道韻 (wind_insight)！\n" NOR);
            }
        }
    }

    // 3. 業力心魔與因果天雷天劫 (當業力大於 80 時，極高機率遭受天劫惩罚)
    if (karma > 80 && random(10) == 0) {
        tell_object(this_object(), HIR "⚡【 ⚠️ 天劫降臨 】你一身紅塵業力引動了虛空雷霆！一道天雷劈打在你的元神上！\n" NOR);
        add_spiritual_energy(-15);
        this_object()->add_observation("chest_tightness"); // 遭受重創，胸口發悶
        tell_object(this_object(), YEL "💡 提示：你的因果業力 (karma) 過高，請前往「紅塵當舖」典當因果消業，或發願了結塵緣。\n" NOR);
    } else if (karma > 50 && random(15) == 0) {
        tell_object(this_object(), HIY "【 🌀 心魔擾動 】你感到一陣莫名的煩躁，心神不寧，體內靈氣運轉有些滯澀...\n" NOR);
    }

    // 4. Entropy：每 N 個心跳，通知 factor_service 進行衰退檢查 (約每 5 分鐘 / 300 秒)
    if ((time() % 300) == 0) {
        object fs = load_object("/runtime/services/factor_service.c");
        if (fs && function_exists("tick_entropy", fs)) {
            fs->tick_entropy(this_object());
        }
    }
}

// ── WebSocket 實時推播介面 ──
void send_json_event(string event_name, mapping data) {
    mapping packet = ([
        "type": event_name,
        "from": "system",
        "to": this_object()->query_name(),
        "payload": json_encode(data)
    ]);
    tell_object(this_object(), "__JSON_MSG__" + json_encode(packet) + "\n");
}

void push_status_update() {
    send_json_event("status_update", ([
        "spiritual_energy": spiritual_energy,
        "karma": karma,
        "is_meditating": is_meditating,
        "observations": keys(query_observations())
    ]));
}

// ── 觀察 Evidence 介面（由陣列改為 Mapping，帶有時間標記支援 session/衰退）──
mapping query_observations() {
    if (!observations) observations = ([]);
    return observations;
}

void add_observation(string sig) {
    if (!observations) observations = ([]);
    observations[sig] = time();
    push_status_update(); // 推播感知更新
}

int has_observation(string sig) {
    if (!observations) observations = ([]);
    return !undefinedp(observations[sig]);
}

void clear_observation(string sig) {
    if (!observations) observations = ([]);
    map_delete(observations, sig);
    push_status_update(); // 推播感知更新
}

void clear_observations() {
    observations = ([]);
    push_status_update(); // 推播感知更新
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


