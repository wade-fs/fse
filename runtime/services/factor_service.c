// /runtime/services/factor_service.c
// FSE 通用核心因素管理器 (Factor Service)
// 零冒險認知 (Adventure-Agnostic)：不含任何冒險特定路徑或邏輯
#include "/runtime/include/ansi.h"

private nosave mapping factors;
private nosave string *discovery_paths;

void create() {
    factors = ([]);
    discovery_paths = ({});
}

// Adventure 在啟動時注冊因素定義目錄 (Adventure-injected)
// 例如："/content/factors", "/content/nodes/some_node/discoveries"
void register_discovery_path(string path) {
    if (!path) return;
    if (member_array(path, discovery_paths) == -1)
        discovery_paths += ({ path });
}

// 在已注冊路徑中搜尋因素 YAML 定義 (通用搜尋，不含冒險特定路徑)
mapping load_factor_data(string fid) {
    foreach (string base in discovery_paths) {
        string path = sprintf("%s/%s.yaml", base, fid);
        if (file_size(path) > 0) {
            string raw = read_file(path);
            if (raw) return yaml_decode(raw);
        }
    }
    return 0;
}

// 判定玩家是否已解鎖特定因素
int factor_discovered(object player, string fid) {
    if (!player) return 0;
    return player->has_factor(fid);
}

// 解鎖因素
int discover_factor(object player, string fid) {
    object event_bus;
    if (!player || !fid) return 0;

    // 若玩家已解鎖，直接返回
    if (factor_discovered(player, fid)) return 0;

    // 載入因素資料
    mapping factor = load_factor_data(fid);
    if (!factor) {
        tell_object(player, HIR + "❌ 因素探索失敗：找不到該因素的設定資料 [" + fid + "]\n" + NOR);
        return 0;
    }

    // 檢查前置條件
    mixed prereqs = factor["prerequisites"];
    if (prereqs) {
        string *missing = ({});
        if (stringp(prereqs)) prereqs = ({ prereqs });
        
        if (arrayp(prereqs)) {
            foreach (string pre in prereqs) {
                if (!factor_discovered(player, pre)) {
                    mapping pre_data = load_factor_data(pre);
                    missing += ({ pre_data ? (pre_data["name"] || pre) : pre });
                }
            }
        }
        
        if (sizeof(missing) > 0) {
            object i18n = load_object("/runtime/services/i18n_service.c");
            if (i18n) {
                tell_object(player, i18n->translate("core.factor.missing_prerequisite", ([ "pre_names": implode(missing, ", ") ])));
            } else {
                tell_object(player, HIR + "❌ 因素探索失敗：你必須先解鎖前置概念 [" + implode(missing, ", ") + "]！\n" + NOR);
            }
            return 0;
        }
    }

    // 儲存至玩家狀態中，支援額外的解鎖 metadata (如解鎖時間)
    player->discover_factor(fid, ([
        "unlocked_at": time(),
        "source": "factor_service"
    ]));

    // 印出通用因素發現視覺提示
    object i18n = load_object("/runtime/services/i18n_service.c");
    if (player->query_temp("is_confused")) {
        if (i18n) {
            string insight_msg = i18n->translate("core.factor.insight_transition");
            if (insight_msg) tell_object(player, insight_msg);
        } else {
            tell_object(player, "\n" + HIC + "【 🌟 困惑 → 領悟 (Insight) 】 之前的疑惑瞬間解開了！" + NOR + "\n");
        }
        player->delete_temp("is_confused");
    }

    tell_object(player, "\n" + HIY + "【 💡 因素探索發現 (Factor Discovery) 】" + NOR + "\n");
    tell_object(player, "你獲得了關鍵概念發現：" + HIG + (factor["name"] || fid) + NOR + "\n");
    if (factor["description"]) {
        tell_object(player, factor["description"] + "\n\n");
    }

    // 發送事件至 FSE EventBus 讓進度管理器連鎖更新
    // 事件攜帶完整的 factor_data，讓接收端可以讀取 quest_trigger 等欄位
    event_bus = load_object("/runtime/services/event_bus.c");
    if (event_bus) {
        int prog_val = 0;
        if (!undefinedp(factor["progress"])) {
            prog_val = factor["progress"];
        }
        event_bus->publish("FactorDiscovered", ([
            "player":      player,
            "factor_id":   fid,
            "progress":    prog_val,
            "factor_data": factor   // 完整資料由接收端自行讀取，無需 runtime 理解其內容
        ]));
    }

    return 1;
}

void remove_factor(object player, string fid) {
    if (player && function_exists("remove_factor", player)) {
        player->remove_factor(fid);
    }
}
