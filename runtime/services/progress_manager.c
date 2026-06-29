// /runtime/services/progress_manager.c
// FSE 通用核心進度管理器 (Progress Manager)
// 支援「多線並行進度」 (Parallel Tracks)
#include "/include/ansi.h"

private nosave mapping global_events;
private nosave string progression_path;  // 由 Adventure 注入

void create() {
    global_events = ([]);
    progression_path = "";

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

// 取得玩家 progression 資料 (輔助函式)
private mapping get_player_track_data(object player, string track) {
    if (!player) return 0;
    mapping prog = player->query_progression();
    if (!prog) prog = ([]);
    if (!prog[track]) {
        // 預設初始化為第一階段 (pw 第一階段為 stage_1_sequence)
        prog[track] = ([ "stage_id": "stage_1_sequence", "progress": 0, "completed_quests": ({}) ]);
    }
    return prog[track];
}

// 寫入玩家 progression 資料 (輔助函式)
private void set_player_track_data(object player, string track, mapping track_data) {
    if (!player) return;
    mapping prog = player->query_progression();
    if (!prog) prog = ([]);
    prog[track] = track_data;
    player->set_progression(prog);
}

// 初始化進階軌道 (預設 track 為 "main")
varargs void set_initial_stage(object player, string stage_id, string track) {
    if (!player) return;
    if (!track) track = "main";
    mapping track_data = get_player_track_data(player, track);
    if (!track_data || track_data["stage_id"] == "") {
        track_data = ([ "stage_id": stage_id, "progress": 0, "completed_quests": ({}) ]);
        set_player_track_data(player, track, track_data);
    }
}

// 取得特定軌道目前的階段 (預設 main)
varargs string query_current_stage(object player, string track) {
    if (!player) return 0;
    if (!track) track = "main";
    mapping track_data = get_player_track_data(player, track);
    return track_data["stage_id"];
}

// 取得特定軌道目前的進度數值
varargs int query_player_progress(object player, string track) {
    if (!player) return 0;
    if (!track) track = "main";
    mapping track_data = get_player_track_data(player, track);
    return track_data["progress"];
}

// 判定特定階段是否完成
varargs int player_stage_completed(object player, string stage, string track) {
    if (!player || !stage) return 0;
    if (track) {
        mapping track_data = get_player_track_data(player, track);
        return track_data && track_data["stage_id"] == stage;
    }
    mapping prog = player->query_progression();
    if (!prog) return 0;
    foreach (string t, mapping data in prog) {
        if (data["stage_id"] == stage) return 1;
    }
    return 0;
}

// 從注冊路徑讀取軌道當前階段的 YAML 設定
mapping query_current_stage_data(object player, string track) {
    if (!progression_path || !player) return 0;
    string sid = query_current_stage(player, track);
    string yaml_path = sprintf("%s/%s.yaml", progression_path, sid);
    if (file_size(yaml_path) <= 0) return 0;
    string raw = read_file(yaml_path);
    if (!raw) return 0;
    return yaml_decode(raw);
}

// 軌道進階
void next_player_stage(object player, string track) {
    if (!player) return;
    mapping stage_data = query_current_stage_data(player, track);
    string next = stage_data ? stage_data["next"] : 0;
    if (!next) return;  // 已是最後階段

    mapping track_data = get_player_track_data(player, track);
    string old_stage = track_data["stage_id"];
    track_data["stage_id"] = next;
    track_data["progress"] = 0;
    set_player_track_data(player, track, track_data);

    mapping next_data = query_current_stage_data(player, track);
    string name = next_data ? next_data["name"] : next;

    string track_display = (track == "main") ? "" : " [" + track + "軌道]";
    
    object i18n = load_object("/runtime/services/i18n_service.c");
    if (i18n) {
        string msg = i18n->translate("core.progress.stage_shifted", ([ "track_display": track_display, "name": name, "next": next ]));
        tell_object(player, msg); // 對個人進行提示，而非 shout 全世界！
    }

    load_object("/runtime/services/event_bus.c")->publish("StageShifted", ([
        "player"      : player,
        "track"       : track,
        "from_stage"  : old_stage,
        "to_stage"    : next,
        "timestamp"   : time()
    ]));
}

// 增加特定軌道的進度並檢查進階
varargs void add_player_progress(object player, int val, string track) {
    if (!player) return;
    if (!track) track = "main";

    mapping track_data = get_player_track_data(player, track);
    track_data["progress"] += val;
    set_player_track_data(player, track, track_data);

    int threshold = 50;  // 預設進階閾值
    mapping stage_data = query_current_stage_data(player, track);
    if (stage_data && intp(stage_data["min_progress"]))
        threshold = stage_data["min_progress"];

    if (track_data["progress"] >= threshold) {
        next_player_stage(player, track);
    }
}

// 完成 Challenge/Quest (通用)
varargs void complete_player_quest(object player, string qid, string track, int progress_val) {
    if (!player) return;
    if (!track) track = "main";

    mapping track_data = get_player_track_data(player, track);
    if (!track_data["completed_quests"]) {
        track_data["completed_quests"] = ({});
    }

    // 避免重複完成同一個任務並重複加進度
    if (member_array(qid, track_data["completed_quests"]) != -1) {
        return;
    }

    track_data["completed_quests"] += ({ qid });
    set_player_track_data(player, track, track_data);

    object i18n = load_object("/runtime/services/i18n_service.c");
    if (i18n) {
        tell_object(player, i18n->translate("core.progress.quest_completed", ([ "qid": qid ])));
    }

    log_file("progress.log", sprintf("[%s] 玩家 %s 完成了任務 %s (Track: %s)\n",
        ctime(time()), player->query_entity_id(), qid, track));

    // 任務完成預設加 10 進度，若有指定則加指定值
    if (!progress_val) progress_val = 10;
    add_player_progress(player, progress_val, track);
}

// Factor 事件連鎖處理
void on_factor_discovered(mapping event) {
    mapping data = event["data"];
    if (!data) return;

    object player     = data["player"];
    string fid        = data["factor_id"];
    int progress_val  = data["progress"];
    mapping factor_data = data["factor_data"];

    if (undefinedp(data["progress"])) progress_val = 0;

    // 若 factor 指定了所屬 track，則進度加在該 track，否則預設 main
    string target_track = "main";
    if (factor_data && factor_data["track"]) {
        target_track = factor_data["track"];
        // 自動初始化
        if (player) {
            set_initial_stage(player, fid, target_track);
        }
    }

    if (player) {
        add_player_progress(player, progress_val, target_track);

        if (factor_data) {
            string quest_trigger = factor_data["quest_trigger"];
            if (quest_trigger) {
                complete_player_quest(player, quest_trigger, target_track);
            }
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
    if (!player) return;
    if (!track) track = "main";
    
    mapping track_data = get_player_track_data(player, track);
    track_data["stage_id"] = new_stage;
    set_player_track_data(player, track, track_data);

    object i18n = load_object("/runtime/services/i18n_service.c");
    if (i18n) {
        tell_object(player, i18n->translate("core.progress.stage_advanced", ([ "track": track, "new_stage": new_stage ])));
    }

    log_file("progress.log", sprintf("[%s] 玩家 %s 階段 (%s) 推進至 %s\n", ctime(time()), player->query_entity_id(), track, new_stage));
}
