// /daemon/progress_d.c
#include "/include/ansi.h"

inherit "/std/object";

string current_stage_id;
int world_progress;
private nosave mapping global_events;

void save_state() {
    if (file_size("/data/state/system/") < 0) {
        mkdir("/data/state/system/");
    }
    save_object("/data/state/system/progress");
}

void restore_state() {
    if (file_size("/data/state/system/progress.o") > 0) {
        restore_object("/data/state/system/progress");
    } else {
        current_stage_id = "stage_1_sequence";
        world_progress = 0;
        save_state();
    }
}

void complete_quest(object player, string qid) {
    if (!player) return;
    
    // 🚀 新增：通知玩家任務完成
    tell_object(player, HIG "🏆 [任務完成] 恭喜你完成了任務：[" + qid + "]！\n" NOR);
    
    log_file("progress.log", sprintf("[%s] 玩家 %s 完成了任務 %s\n", 
        ctime(time()), player->query_entity_id(), qid));
    
    // 增加世界進度值
    world_progress += 10;
    save_state();
    
    check_stage_advancement(player);
}

void set_stage(object player, string new_stage) {
    current_stage_id = new_stage;
    save_state();
    
    // 🚀 新增：通知玩家世界階段前進
    if (player) {
        tell_object(player, HIC "🚀 [世界階段前進] 你的程式功力大增，世界階段已推進至 [" + new_stage + "]！\n" NOR);
    }
    
    log_file("progress.log", sprintf("[%s] 世界階段推進至 %s\n", 
        ctime(time()), new_stage));
}

// 供測試或重置使用的強制初始化函數
void reset_stage() {
    current_stage_id = "stage_1_sequence";
    world_progress = 0;
    save_state();
}

void create() {
    ::create();
    global_events = ([]);
    restore_state();

    // 訂閱 Discovery 完成事件，驅動進度
    call_out("subscribe_events", 1);
}

void set_global_event(string event, int val) {
    if (!global_events) global_events = ([]);
    global_events[event] = val;
}

int global_event_triggered(string event) {
    if (!global_events) return 0;
    return global_events[event] || 0;
}

void subscribe_events() {
    load_object("/secure/event_d.c")->subscribe("DiscoveryCompleted", "on_discovery_completed");
}

string query_current_stage() {
    return current_stage_id;
}

int query_world_progress() {
    return world_progress;
}

int stage_completed(string stage) {
    if (!stage) return 0;
    if (current_stage_id == "stage_2_loop") {
        return 1;
    }
    return current_stage_id == stage;
}

// 取得當前階段的詳細資料 (從 YAML 讀取)
mapping query_current_stage_data() {
    string yaml_path = sprintf("/content/stages/%s.yaml", current_stage_id);
    if (file_size(yaml_path) <= 0) return 0;
    string content = read_file(yaml_path);
    if (!content) return 0;
    return yaml_decode(content);
}

// 階段推進
void next_stage() {
    string old_stage = current_stage_id;
    if (current_stage_id == "stage_1_sequence") {
        current_stage_id = "stage_2_loop";
    } else {
        return; // 已是最高階段
    }

    world_progress = 0; // 重設進度
    save_state();

    // 讀取新階段的名稱
    mapping stage_data = query_current_stage_data();
    string name = stage_data ? stage_data["name"] : current_stage_id;

    // 廣播給所有線上玩家
    shout(HIW "\n【世界邏輯共振完成】\n即將進入邏輯階段：" HIG + name + " (" + current_stage_id + ")" + NOR "\n\n");
    
    // 發送領域事件
    load_object("/secure/event_d.c")->publish("StageShifted", ([
        "from_stage"  : old_stage,
        "to_stage"    : current_stage_id,
        "timestamp"   : time()
    ]));
}

// 增加世界邏輯進度
void add_world_progress(int val) {
    world_progress += val;
    save_state();

    int threshold = 50; // 預設進階閾值改為 50 (迎合最小閉環)
    mapping stage_data = query_current_stage_data();
    if (stage_data && intp(stage_data["min_progress"]))
        threshold = stage_data["min_progress"];

    if (world_progress >= threshold) {
        next_stage();
    }
}

// Discovery 完成事件處理函式
void on_discovery_completed(mapping event) {
    mapping data = event["data"];
    object player;
    if (!data) return;

    player = data["player"];
    int progress_val = data["progress"];
    if (!progress_val) progress_val = 50; // 每完成一次領悟 +50 進度

    add_world_progress(progress_val);

    // 🚀 新增：當解鎖 loop_termination 領悟時，連帶完成第一關任務 infinite_loop_termination
    if (player && data["discovery_id"] == "loop_termination") {
        complete_quest(player, "infinite_loop_termination");
    }

    log_file("progress.log", sprintf(
        "[%s] DiscoveryCompleted: %s 由 %s 觸發，+%d 進度（目前 %d）\n",
        ctime(time()),
        data["discovery_id"] || "unknown",
        player ? player->query_entity_id() : "unknown",
        progress_val,
        world_progress
    ));
}
