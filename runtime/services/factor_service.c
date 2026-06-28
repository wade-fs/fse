// /runtime/services/factor_service.c
// FSE 通用核心因素管理器 (Factor Service)
#include "/include/ansi.h"

private nosave mapping factors;

void create() {
    factors = ([]);
}

// 從 YAML 載入特定因素定義 (優先搜尋全域 /content/factors/{fid}.yaml，再搜尋 Node 自包含目錄)
mapping load_factor_data(string fid) {
    string path = sprintf("/content/factors/%s.yaml", fid);
    if (file_size(path) <= 0) {
        // 搜尋 Node 自包含目錄
        path = sprintf("/content/nodes/infinite_loop_swamp/discoveries/%s.yaml", fid);
    }
    if (file_size(path) <= 0) return 0;
    string content = read_file(path);
    if (!content) return 0;
    return yaml_decode(content);
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
        write(HIR "❌ 因素探索失敗：找不到該因素的設定資料 [" + fid + "]\n" NOR);
        return 0;
    }

    // 儲存至玩家狀態中
    player->discover_factor(fid);

    // 印出通用因素發現視覺提示
    write("\n" HIY "【 💡 因素探索發現 (Factor Discovery) 】" NOR "\n");
    write("你獲得了關鍵概念發現：" HIG + (factor["name"] || fid) + NOR "\n");
    if (factor["description"]) {
        write(factor["description"] + "\n\n");
    }

    // 發送事件至 FSE EventBus 讓進度管理器連鎖更新
    event_bus = load_object("/runtime/services/event_service.c");
    if (event_bus) {
        event_bus->publish("FactorDiscovered", ([ 
            "player": player, 
            "factor_id": fid,
            "progress": factor["progress"] || 50 
        ]));
    }

    return 1;
}
