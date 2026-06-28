// /daemon/discovery_d.c
// 
// 領悟守護進程。
//
// 職責：
//   - 載入並索引所有 /content/discoveries/ 下的領悟定義
//   - 判定玩家是否滿足領悟解鎖條件
//   - 提供領悟解鎖與完成度查詢，並與玩家存檔整合。

#include "/include/ansi.h"

inherit "/std/entity.c";

// 記憶體快取：所有載入的領悟設定
// ([ discovery_id: discovery_mapping ])
private nosave mapping discoveries_cache;

void rehash() {
    discoveries_cache = ([]);
    string dir = "/content/discoveries/";
    if (file_size(dir) != -2) return; // 必須是目錄

    string *files = get_dir(dir);
    if (!files) return;

    foreach (string file in files) {
        if (strlen(file) < 5 || substr(file, strlen(file)-5, 5) != ".yaml") continue;
        string path = dir + file;
        string content = read_file(path);
        if (!content) continue;
        mapping data = yaml_decode(content);
        if (!data || !data["discovery_id"]) continue;

        string id = data["discovery_id"];
        discoveries_cache[id] = data;
    }
}

void create() {
    ::create();
    set_entity_id("daemon:discovery");
    set_entity_type("daemon");
    rehash();
}

mapping load_discovery(string id) {
    if (!discoveries_cache) rehash();
    return discoveries_cache[id];
}

mapping query_all_discoveries() {
    if (!discoveries_cache) rehash();
    return copy(discoveries_cache);
}

// 判定特定領悟是否已完成（為 reveal 條件提供）
int discovery_completed(object player, string discovery_id) {
    if (!player || !discovery_id) return 0;
    string *unlocked = player->query_unlocked_discoveries();
    if (!unlocked) return 0;
    return member_array(discovery_id, unlocked) != -1;
}

int is_discovery_completed(string discovery_id) {
    return 0;
}

// 觸發領悟完成 (成功解鎖並發送事件)
void unlock_discovery(object player, string did) {
    if (!player || !did) return;
    if (discovery_completed(player, did)) return;

    if (!discoveries_cache) rehash();
    mapping disc = load_discovery(did);
    if (disc) {
        player->add_unlocked_discovery(did);
        
        tell_object(player, "\n" + HIG + "【邏輯思維領悟 (Discovery)】" + NOR + "你獲得了關鍵概念領悟：\n");
        tell_object(player, HIW + "「" + disc["name"] + "」" + NOR + "\n");
        tell_object(player, disc["description"] + "\n\n");

        // 發送事件，通知 progress_d
        load_object("/secure/event_d.c")->publish("DiscoveryCompleted", ([
            "player_id":     player->query_entity_id(),
            "discovery_id":  did,
            "progress":      disc["progress"] || 50,
            "timestamp":     time()
        ]));

        // 🚀 新增：即時向玩家印出領悟成功的精美提示
        tell_object(player, HIY "✨ [概念領悟] 你領悟了全新編程概念：[" + did + "]！\n" NOR);
    }
}
