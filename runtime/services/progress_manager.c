// /runtime/services/progress_manager.c
// FSE 通用核心進度管理器 (Progress Manager)
// 支援「多線並行進度」 (Parallel Tracks)
#include "/include/ansi.h"

// active_tracks 結構： ([ "track_name" : ([ "stage_id": "xxx", "progress": 0 ]) ])
mapping active_tracks;
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
    if (!active_tracks) active_tracks = ([]);
}

void create() {
    active_tracks = ([]);
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
void register_progression_path(string path) {
    progression_path = path;
}

// 初始化進階軌道 (預設 track 為 "main")
varargs void set_initial_stage(string stage_id, string track) {
    if (!track) track = "main";
    if (!active_tracks[track] || active_tracks[track]["stage_id"] == "") {
        active_tracks[track] = ([ "stage_id": stage_id, "progress": 0 ]);
        save_state();
    }
}

// 取得特定軌道目前的階段 (預設 main)
varargs string query_current_stage(string track) {
    if (!track) track = "main";
    if (!active_tracks[track]) return 0;
    return active_tracks[track]["stage_id"];
}

// 取得特定軌道目前的進度數值
varargs int query_world_progress(string track) {
    if (!track) track = "main";
    if (!active_tracks[track]) return 0;
    return active_tracks[track]["progress"];
}

// 判定特定階段是否完成 (若不指定軌道，則搜尋所有啟動中的軌道)
varargs int stage_completed(string stage, string track) {
    if (!stage) return 0;
    if (track) {
        return active_tracks[track] && active_tracks[track]["stage_id"] == stage;
    }
    foreach (string t, mapping data in active_tracks) {
        if (data["stage_id"] == stage) return 1;
    }
    return 0;
}

// 從注冊路徑讀取軌道當前階段的 YAML 設定
mapping query_current_stage_data(string track) {
    if (!progression_path || !active_tracks[track]) return 0;
    string sid = active_tracks[track]["stage_id"];
    string yaml_path = sprintf("%s/%s.yaml", progression_path, sid);
    if (file_size(yaml_path) <= 0) return 0;
    string raw = read_file(yaml_path);
    if (!raw) return 0;
    return yaml_decode(raw);
}

// 軌道進階
void next_stage(string track) {
    mapping stage_data = query_current_stage_data(track);
    string next = stage_data ? stage_data["next"] : 0;
    if (!next) return;  // 已是最後階段

    string old_stage = active_tracks[track]["stage_id"];
    active_tracks[track]["stage_id"] = next;
    active_tracks[track]["progress"] = 0;
    save_state();

    mapping next_data = query_current_stage_data(track);
    string name = next_data ? next_data["name"] : next;

    string track_display = (track == "main") ? "" : " [" + track + "軌道]";
    
    object i18n = load_object("/runtime/services/i18n_service.c");
    if (i18n) {
        string msg = i18n->translate("core.progress.stage_shifted", ([ "track_display": track_display, "name": name, "next": next ]));
        shout(msg);
    }

    load_object("/runtime/services/event_bus.c")->publish("StageShifted", ([
        "track"       : track,
        "from_stage"  : old_stage,
        "to_stage"    : next,
        "timestamp"   : time()
    ]));
}

// 增加特定軌道的進度並檢查進階
varargs void add_world_progress(int val, string track) {
    if (!track) track = "main";
    if (!active_tracks[track]) return;

    active_tracks[track]["progress"] += val;
    save_state();

    int threshold = 50;  // 預設進階閾值
    mapping stage_data = query_current_stage_data(track);
    if (stage_data && intp(stage_data["min_progress"]))
        threshold = stage_data["min_progress"];

    if (active_tracks[track]["progress"] >= threshold) {
        next_stage(track);
    }
}

// 完成 Challenge/Quest (通用)
varargs void complete_quest(object player, string qid, string track) {
    if (!player) return;
    if (!track) track = "main";
    if (!active_tracks[track]) return;

    if (!active_tracks[track]["completed_quests"]) {
        active_tracks[track]["completed_quests"] = ({});
    }

    // 避免重複完成同一個任務並重複加進度
    if (member_array(qid, active_tracks[track]["completed_quests"]) != -1) {
        return;
    }

    active_tracks[track]["completed_quests"] += ({ qid });
    save_state();

    object i18n = load_object("/runtime/services/i18n_service.c");
    if (i18n) {
        tell_object(player, i18n->translate("core.progress.quest_completed", ([ "qid": qid ])));
    }

    log_file("progress.log", sprintf("[%s] 玩家 %s 完成了任務 %s (Track: %s)\n",
        ctime(time()), player->query_entity_id(), qid, track));

    // 任務完成預設加 10 進度
    add_world_progress(10, track);
}

// Factor 事件連鎖處理
void on_factor_discovered(mapping event) {
    mapping data = event["data"];
    if (!data) return;

    object player     = data["player"];
    string fid        = data["factor_id"];
    int progress_val  = data["progress"];
    mapping factor_data = data["factor_data"];

    if (!progress_val) progress_val = 50;

    // 若 factor 指定了所屬 track，則進度加在該 track，否則預設 main
    string target_track = "main";
    if (factor_data && factor_data["track"]) {
        target_track = factor_data["track"];
        // 如果這個 track 還沒啟動，自動初始化 (可選邏輯)
        if (!active_tracks[target_track]) {
            set_initial_stage(fid, target_track);
        }
    }

    add_world_progress(progress_val, target_track);

    if (player && factor_data) {
        string quest_trigger = factor_data["quest_trigger"];
        if (quest_trigger) {
            complete_quest(player, quest_trigger, target_track);
        }
    }

    log_file("progress.log", sprintf(
        "[%s] FactorDiscovered: %s 由 %s 觸發，+%d 進度 (Track: %s)\n",
        ctime(time()),
        fid || "unknown",
        player ? player->query_entity_id() : "unknown",
        progress_val,
        target_track
    ));
}

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

varargs void set_stage(object player, string new_stage, string track) {
    if (!track) track = "main";
    if (!active_tracks[track]) active_tracks[track] = ([]);
    
    active_tracks[track]["stage_id"] = new_stage;
    save_state();

    if (player) {
        object i18n = load_object("/runtime/services/i18n_service.c");
        if (i18n) {
            tell_object(player, i18n->translate("core.progress.stage_advanced", ([ "track": track, "new_stage": new_stage ])));
        }
    }

    log_file("progress.log", sprintf("[%s] 世界階段 (%s) 推進至 %s\n", ctime(time()), track, new_stage));
}

// 供測試或重置使用的強制初始化函數
void reset_stage() {
    active_tracks = ([]);
    save_state();
}
