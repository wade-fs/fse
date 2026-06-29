// /runtime/services/progress_manager.c
// FSE 通用核心進度管理器 (Progress Manager)
// 支援「多線並行進度」 (Parallel Tracks)
#include "/runtime/include/ansi.h"

private nosave mapping global_events;
private nosave string progression_path;  // 由 Adventure 注入
private nosave mapping default_stages;   // 由 Adventure 注入的預設階段

void create() {
    global_events = ([]);
    progression_path = "";
    default_stages = ([]);

    // 訂閱 Discovery 事件以驅動進度
    call_out("subscribe_events", 1);
    
    // 啟動熵值 (Entropy) 循環
    call_out("entropy_tick", 180); // 每 3 分鐘衰退一次
}

void entropy_tick() {
    object *players = users();
    foreach (object p in players) {
        mapping prog = p->query_progression();
        if (!prog) continue;
        
        int changed = 0;
        foreach (string track, mapping track_data in prog) {
            // 讓進度自然衰退 (熵增)
            if (track_data["progress"] > 0) {
                track_data["progress"] -= 1; // 輕微衰退
                changed = 1;
                
                // 當進度衰退時，偶爾給予提示
                if (track_data["progress"] % 10 == 0) {
                    tell_object(p, HIK + "\n【世界熵增】 你感覺在 [" + track + "] 的某些情境連結，似乎隨著時間變得有些模糊了...\n" + NOR);
                }
            }
        }
        
        if (changed) {
            p->set_progression(prog);
        }
    }
    
    call_out("entropy_tick", 180);
}

void subscribe_events() {
    load_object("/runtime/services/event_bus.c")->subscribe("FactorDiscovered", "on_factor_discovered");
    load_object("/runtime/services/event_bus.c")->subscribe("DiscoveryCompleted", "on_discovery_completed");
}

// Adventure 注冊 progression YAML 目錄 (Adventure-injected)
void register_progression_path(string path) {
    progression_path = path;
}

// Adventure 注冊軌道預設階段
varargs void set_default_initial_stage(string track, string stage_id) {
    if (!track) track = "main";
    default_stages[track] = stage_id;
}

// 取得玩家 progression 資料 (輔助函式)
private mapping get_player_track_data(object player, string track) {
    if (!player) return 0;
    mapping prog = player->query_progression();
    if (!prog) prog = ([]);
    if (!prog[track]) {
        string init_stage = default_stages[track] ? default_stages[track] : "";
        prog[track] = ([ "stage_id": init_stage, "progress": 0, "completed_quests": ({}) ]);
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

// 針對個別玩家初始化進階軌道
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

// 取得當前階段的初始節點
varargs string query_spawn_node(object player, string track) {
    if (!player) return 0;
    if (!track) track = "main";
    mapping stage_data = query_current_stage_data(player, track);
    if (stage_data && stage_data["spawn_node"]) {
        return stage_data["spawn_node"];
    }
    return 0; // 若無設定，回傳 0
}

// 抵抗熵增：玩家與世界互動時增加 progress
varargs void resist_entropy(object player, string track, int amount) {
    if (!player) return;
    if (!track) track = "main";
    
    mapping track_data = get_player_track_data(player, track);
    if (!track_data) return;
    
    if (!amount) amount = 1;
    track_data["progress"] += amount;
    
    set_player_track_data(player, track, track_data);
}

// 檢查玩家階段晉級條件 (Condition-based Evaluator)
void check_player_stage_advancement(object player, string track) {
    if (!player) return;
    if (!track) track = "main";

    mapping stage_data = query_current_stage_data(player, track);
    if (!stage_data) return;

    mapping requires = stage_data["requires"];
    if (!requires) return; // 無特別條件限制，則不自動升階

    // 1. 檢查 Quests 挑戰是否全部完成
    if (arrayp(requires["quests"])) {
        mapping track_data = get_player_track_data(player, track);
        string *completed = track_data["completed_quests"];
        if (!completed) completed = ({});
        
        foreach (string req_quest in requires["quests"]) {
            if (member_array(req_quest, completed) == -1) {
                return; // 還有需要的挑戰未完成，中斷判定
            }
        }
    }

    // 2. 檢查 Factors 知識點是否解鎖
    if (arrayp(requires["factors"])) {
        foreach (string req_factor in requires["factors"]) {
            if (!player->has_factor(req_factor)) {
                return; // 還有需要的知識概念未解鎖，中斷判定
            }
        }
    }

    // 所有要求條件均滿足，晉升至下一階段！
    next_player_stage(player, track);
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
    set_player_track_data(player, track, track_data);

    mapping next_data = query_current_stage_data(player, track);
    string name = next_data ? next_data["name"] : next;

    string track_display = (track == "main") ? "" : " [" + track + "軌道]";
    
    object i18n = load_object("/runtime/services/i18n_service.c");
    if (i18n) {
        string msg = i18n->translate("core.progress.stage_shifted", ([ "track_display": track_display, "name": name, "next": next ]));
        tell_object(player, msg); // 對個人進行提示
    }

    load_object("/runtime/services/event_bus.c")->publish("StageShifted", ([
        "player"      : player,
        "track"       : track,
        "from_stage"  : old_stage,
        "to_stage"    : next,
        "timestamp"   : time()
    ]));
}

// 完成 Challenge/Quest (通用)
varargs void complete_player_quest(object player, string qid, string track, int progress_val) {
    if (!player) return;
    if (!track) track = "main";

    mapping track_data = get_player_track_data(player, track);
    if (!track_data["completed_quests"]) {
        track_data["completed_quests"] = ({});
    }

    // 避免重複完成同一個任務
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

    // 🚀 完成挑戰後，觸發條件檢查！
    check_player_stage_advancement(player, track);
}

// Factor 事件連鎖處理
void on_factor_discovered(mapping event) {
    mapping data = event["data"];
    if (!data) return;

    object player     = data["player"];
    string fid        = data["factor_id"];
    mapping factor_data = data["factor_data"];

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
        // 🚀 解鎖 Factor 後，觸發條件檢查！
        check_player_stage_advancement(player, target_track);

        if (factor_data) {
            string quest_trigger = factor_data["quest_trigger"];
            if (quest_trigger) {
                complete_player_quest(player, quest_trigger, target_track);
            }
        }
    }

    log_file("progress.log", sprintf(
        "[%s] FactorDiscovered: %s 由 %s 觸發 (Track: %s)\n",
        ctime(time()),
        fid || "unknown",
        player ? player->query_entity_id() : "unknown",
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
