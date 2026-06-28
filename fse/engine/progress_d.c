// /fse/engine/progress_d.c
// FSE (Flow State Engine) 通用核心進度與解鎖管理器 (Progress Manager)
#include "/include/ansi.h"

inherit "/fse/std/object";

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

void create() {
    ::create();
    global_events = ([]);
    restore_state();

    // 訂閱 Discovery 事件以驅動進度
    call_out("subscribe_events", 1);
}

void subscribe_events() {
    load_object("/fse/engine/event_d.c")->subscribe("FactorDiscovered", "on_factor_discovered");
    load_object("/fse/engine/event_d.c")->subscribe("DiscoveryCompleted", "on_discovery_completed");
}

string query_current_stage() {
    return current_stage_id;
}

int query_world_progress() {
    return world_progress;
}

// 判定世界階段是否完成
int stage_completed(string stage) {
    if (!stage) return 0;
    if (current_stage_id == "stage_2_loop") {
        return 1;
    }
    return current_stage_id == stage;
}

// 取得當前階段設定資料
mapping query_current_stage_data() {
    string yaml_path = sprintf("/content/stages/%s.yaml", current_stage_id);
    if (file_size(yaml_path) <= 0) return 0;
    string content = read_file(yaml_path);
    if (!content) return 0;
    return yaml_decode(content);
}

// 世界進階
void next_stage() {
    string old_stage = current_stage_id;
    if (current_stage_id == "stage_1_sequence") {
        current_stage_id = "stage_2_loop";
    } else {
        return; // 已是最高階段
    }

    world_progress = 0;
    save_state();

    mapping stage_data = query_current_stage_data();
    string name = stage_data ? stage_data["name"] : current_stage_id;

    // 廣播世界邏輯階段前進
    shout(HIW "\n【世界邏輯共振完成】\n即將進入邏輯階段：" HIG + name + " (" + current_stage_id + ")" + NOR "\n\n");
    
    load_object("/fse/engine/event_d.c")->publish("StageShifted", ([
        "from_stage"  : old_stage,
        "to_stage"    : current_stage_id,
        "timestamp"   : time()
    ]));
}

// 增加進度並檢查進階
void add_world_progress(int val) {
    world_progress += val;
    save_state();

    int threshold = 50; // 預設進階閾值
    mapping stage_data = query_current_stage_data();
    if (stage_data && intp(stage_data["min_progress"]))
        threshold = stage_data["min_progress"];

    if (world_progress >= threshold) {
        next_stage();
    }
}

// 完成 Challenge/Quest
void complete_quest(object player, string qid) {
    if (!player) return;
    
    // 印出通用 Challenge/Quest 完成提示
    tell_object(player, HIG "🏆 [挑戰完成] 恭喜你完成了任務/挑戰：[" + qid + "]！\n" NOR);
    
    log_file("progress.log", sprintf("[%s] 玩家 %s 完成了任務 %s\n", 
        ctime(time()), player->query_entity_id(), qid));
    
    add_world_progress(10);
}

// Factor 事件連鎖處理
void on_factor_discovered(mapping event) {
    mapping data = event["data"];
    object player;
    if (!data) return;

    player = data["player"];
    string fid = data["factor_id"];
    int progress_val = data["progress"];
    if (!progress_val) progress_val = 50;

    add_world_progress(progress_val);

    // 🚀 資料驅動連鎖：當發現 loop_termination 時，解鎖首個無窮迴圈挑戰
    if (player && fid == "loop_termination") {
        complete_quest(player, "infinite_loop_termination");
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
            "player": data["player"],
            "factor_id": data["discovery_id"],
            "progress": data["progress"]
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
    current_stage_id = "stage_1_sequence";
    world_progress = 0;
    save_state();
}
