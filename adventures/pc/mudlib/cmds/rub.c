// /adventures/pc/mudlib/cmds/rub.c
// rub [目標1] [目標2] — 摩擦引火指令 (第一個文明求生閉環)
#include "/include/ansi.h"
inherit "/std/object";

void create() { ::create(); }

void main(object me, string arg) {
    object room = environment(me);
    if (!room) {
        tell_object(me, "你不在任何地方，無法摩擦物品。\n");
        return;
    }

    if (!arg || arg == "") {
        tell_object(me, "你想摩擦什麼？ (例如: rub branches roots)\n");
        return;
    }

    string item1, item2;
    if (sscanf(arg, "%s %s", item1, item2) != 2) {
        tell_object(me, "摩擦引火需要兩個介質，例如：rub branches roots\n");
        return;
    }

    item1 = trim(item1);
    item2 = trim(item2);

    if ((item1 == "branches" && item2 == "roots") || (item1 == "roots" && item2 == "branches")) {
        // 檢查玩家是否攜帶/在場有這些東西
        // 史前文明簡化版：玩家在三疊紀荒野可以隨地撿起 branches (樹枝)，但 roots (耐旱蕨類乾枯根部) 需要玩家 focus ground 獲得
        if (!me->query_temp("found_roots")) {
            // 失敗，產生 Confusion
            me->player_confused("rub_without_kindling");
            tell_object(me, RED + "【 🌀 產生困惑 】你用力拿樹枝去磨擦普通的岩石與濕土，除了一身汗之外，什麼也沒發生。\n" +
                "大雨將至，寒風吹過，你的體溫正在迅速流失！\n" +
                "💡 這裡有乾燥的引火介質嗎？也許你應該觀察地面 (focus ground)..." + NOR + "\n");
            me->add_fatigue(15);
            return;
        }

        // 成功！觸發 Discovery 與 Quest 完成
        tell_object(me, HIG + "==================================================\n" +
            "  🌟【 困惑 ➔ 領悟 (Insight) 】摩擦生熱與引火！\n" +
            "==================================================\n" +
            "你用乾燥的耐旱蕨類根部作為火絨，將質地堅硬的松科樹枝急速旋轉摩擦。\n" +
            "一縷青煙冒出，伴隨著微小的火星，火被點燃了！\n" + NOR);

        // 授與 Factor (Discovery)
        object factor_svc = load_object("/runtime/services/factor_service.c");
        if (factor_svc) {
            factor_svc->discover_factor(me, "thermodynamics");
        }

        // 推進進度
        object pm = load_object("/runtime/services/progress_manager.c");
        if (pm) {
            pm->complete_player_quest(me, "first_fire", "main", 100);
        }

        tell_object(me, YEL + "🔥 一堆小小的營火在你面前升起，驅散了黑暗與寒冷。\n" + NOR);
        room->set_long(room->query_long() + "\n" + YEL + "🔥 一堆營火在中央跳動著，照亮了原本黑暗的世界邊緣。" + NOR);
        
        // 營火 Reveal 了新路徑
        room->add_exit("forest", "/rooms/fern_forest/room");
        tell_object(me, GRN + "\n【 🌲 顯現 (Reveal) 】在營火跳躍的火光下，遠方原本漆黑模糊的「蕨類森林 (forest)」路徑清晰地展現了出來！\n" + NOR);

    } else {
        tell_object(me, "你試圖摩擦 " + item1 + " 與 " + item2 + "，但這兩者無法產生足夠的摩擦生熱效果。\n");
    }
}
