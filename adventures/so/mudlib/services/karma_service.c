// /adventures/so/mudlib/services/karma_service.c
// 尋仙問道 — 專屬全局業力心魔調度服務 (Karma Service Daemon)
// 職責：
//   - 監聽全局因果發願事件
//   - 定時檢查全門派玩家之善惡業力分配
//   - 當特定玩家紅塵業障（karma）過重時，調度天劫或心魔雷擊事件。
#include "/runtime/include/ansi.h"

void create() {
    write(HIG "  [KarmaService] 尋仙問道因果業力守護進程啟動成功。\n" NOR);
    
    // 訂閱事件以監聽發願了結
    object event_bus = load_object("/runtime/services/event_bus.c");
    if (event_bus) {
        event_bus->subscribe("VowResolved", "on_vow_resolved");
    }
}

// 事件回調：當玩家了結自心發願時，給予天道功德與功德降臨提示
void on_vow_resolved(mapping data) {
    if (!data) return;
    object player = data["player"];
    string vow_id = data["vow_id"];
    
    if (player) {
        tell_object(player, HIY "\n✨【 天道感應 】你成功了結了「" + vow_id + "」之自心發願，塵緣漸了，一身業力退去，道心更顯澄澈！\n\n" NOR);
    }
}
