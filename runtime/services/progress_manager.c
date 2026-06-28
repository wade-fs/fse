// /runtime/services/progress_manager.c
// FSE 通用核心進度管理器 (Progress Manager)
// 零冒險認知 (Adventure-Agnostic)：不含任何冒險特定階段名稱或因素 ID
#include "/include/ansi.h"

string current_stage_id;
int world_progress;
private nosave mapping global_events;
private nosave string progression_path;  // 由 Adventure 注入

void save_state() {
    if (file_size("/data/state/system/") < 0) {
        mkdir("/data/state/system/");
    }
    save_object("/data/state/system/progress");
}

void restore_state() {
    if (file_size("/data/state/system/progress.o") > 0) {
        restore_object("/data/state/system/progress");
    }
    // 若無存檔，current_stage_id 保持空值，等待 Adventure 呼叫 set_initial_stage()
}

void create() {
    global_events = ([]);
    progression_path = "";
    restore_state();

    // 訂閱 Discovery 事件以驅動進度
    call_out("subscribe_events", 1);
}

void subscribe_events() {
    load_object("/runtime/services/event_bus.c")->subscribe("FactorDiscovered", "on_factor_discovered");
    load_object("/runtime/services/event_bus.c")->subscribe("DiscoveryCompleted", "on_discovery_completed");
}

// Adventure 注冊 progression YAML 目錄 (Adventure-injected)
// 例如："/content/progression"
void register_progression_path(string path) {
    progression_path = path;
}

// Adventure 設定初始階段 (僅在無存檔狀態時生效，不覆蓋已還原的存檔)
void set_initial_stage(string stage_id) {
    if (!current_stage_id || current_stage_id == "") {
        current_stage_id = stage_id;
        world_progress = 0;
        save_state();
    }
}

string query_current_stage() {
    return current_stage_id;
}

int query_world_progress() {
    return world_progress;
}

// 判定世界階段是否完成 (純粹比對字串，無冒險知識)
int stage_completed(string stage) {
    if (!stage || !current_stage_id) return 0;
    return current_stage_id == stage;
}

// 從注冊路徑讀取當前階段的 YAML 設定 (資料驅動，路徑由 Adventure 注入)
mapping query_current_stage_data() {
    if (!progression_path || !current_stage_id) return 0;
    string yaml_path = sprintf("%s/%s.yaml", progression_path, current_stage_id);
    if (file_size(yaml_path) <= 0) return 0;
    string raw = read_file(yaml_path);
    if (!raw) return 0;
    return yaml_decode(raw);
}

// 世界進階 (完全資料驅動，下一個 stage 由 YAML 的 "next" 欄位決定)
void next_stage() {
    mapping stage_data = query_current_stage_data();
    string next = stage_data ? stage_data["next"] : 0;
    if (!next) return;  // 已是最後階段，無 "next" 欄位

    string old_stage = current_stage_id;
    current_stage_id = next;
    world_progress = 0;
    save_state();

    mapping next_data = query_current_stage_data();
    string name = next_data ? next_data["name"] : current_stage_id;

    shout(HIW "\n【世界邏輯共振完成】\n即將進入邏輯階段：" HIG + name + " (" + current_stage_id + ")" + NOR "\n\n");

    load_object("/runtime/services/event_bus.c")->publish("StageShifted", ([
        "from_stage"  : old_stage,
        "to_stage"    : current_stage_id,
        "timestamp"   : time()
    ]));
}

// 增加進度並檢查進階
void add_world_progress(int val) {
    world_progress += val;
    save_state();

    int threshold = 50;  // 預設進階閾值
    mapping stage_data = query_current_stage_data();
    if (stage_data && intp(stage_data["min_progress"]))
        threshold = stage_data["min_progress"];

    if (world_progress >= threshold) {
        next_stage();
    }
}

// 完成 Challenge/Quest (通用，不含冒險特定邏輯)
void complete_quest(object player, string qid) {
    if (!player) return;

    tell_object(player, HIG "🏆 [挑戰完成] 恭喜你完成了任務/挑戰：[" + qid + "]！\n" NOR);

    log_file("progress.log", sprintf("[%s] 玩家 %s 完成了任務 %s\n",
        ctime(time()), player->query_entity_id(), qid));

    add_world_progress(10);
}

// Factor 事件連鎖處理 (資料驅動：從 factor_data 讀取 quest_trigger，無硬編碼)
void on_factor_discovered(mapping event) {
    mapping data = event["data"];
    if (!data) return;

    object player     = data["player"];
    string fid        = data["factor_id"];
    int progress_val  = data["progress"];
    mapping factor_data = data["factor_data"];  // 由 factor_service 攜帶，完整 factor 定義

    if (!progress_val) progress_val = 50;
    add_world_progress(progress_val);

    // 資料驅動連鎖：若 factor YAML 定義了 quest_trigger，則自動完成對應任務
    if (player && factor_data) {
        string quest_trigger = factor_data["quest_trigger"];
        if (quest_trigger) {
            complete_quest(player, quest_trigger);
        }
    }

    log_file("progress.log", sprintf(
        "[%s] FactorDiscovered: %s 由 %s 觸發，+%d 進度（目前 %d）\n",
        ctime(time()),
        fid || "unknown",
        player ? player->query_entity_id() : "unknown",
        progress_val,
        world_progress
    ));
}

// Discovery 事件連鎖處理 (相容舊架構)
void on_discovery_completed(mapping event) {
    mapping data = event["data"];
    if (!data) return;
    on_factor_discovered(([
        "data": ([
            "player":      data["player"],
            "factor_id":   data["discovery_id"],
            "progress":    data["progress"],
            "factor_data": data["factor_data"]
        ])
    ]));
}

void set_stage(object player, string new_stage) {
    current_stage_id = new_stage;
    save_state();

    if (player) {
        tell_object(player, HIC "🚀 [世界階段前進] 你的程式功力大增，世界階段已推進至 [" + new_stage + "]！\n" NOR);
    }

    log_file("progress.log", sprintf("[%s] 世界階段推進至 %s\n", ctime(time()), new_stage));
}

// 供測試或重置使用的強制初始化函數
void reset_stage() {
    mapping stage_data = query_current_stage_data();
    // 讀取原始初始 stage (若已設定 progression_path，從第一個有 start:true 的找起)
    // 簡化實作：直接清空，等 Adventure 重新呼叫 set_initial_stage
    current_stage_id = "";
    world_progress = 0;
    save_state();
}
