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

        // ── 守衛：first_fire 已完成，禁止重複觸發 ──
        // thermodynamics factor 是 first_fire 閉環唯一的成功憑證。
        // 有它代表玩家已學過，重複操作只會讓火燒得更旺，不會再次領悟。
        if (me->has_factor("thermodynamics")) {
            tell_object(me, "你嫻熟地用耐旱蕨類根部與樹枝摩擦，火苗輕易地跳躍而出。\n" +
                YEL + "🔥 營火已在燃燒中。\n" + NOR);
            return;
        }

        // ── 前置守衛：必須先 focus ground 找到乾燥根部 ──
        if (!me->query_temp("found_roots")) {
            // 失敗，產生 Confusion
            me->player_confused("rub_without_kindling");
            tell_object(me, RED + "【 🌀 產生困惑 】你用力拿樹枝去磨擦普通的岩石與濕土，除了一身汗之外，什麼也沒發生。\n" +
                "大雨將至，寒風吹過，你的體溫正在迅速流失！\n" +
                "💡 這裡有乾燥的引火介質嗎？也許你應該觀察地面 (focus ground)..." + NOR + "\n");
            me->add_fatigue(15);
            return;
        }

        // ── 成功！觸發 Discovery 與 Quest 完成（只會發生一次）──
        tell_object(me, HIG + "==================================================\n" +
            "  🌟【 困惑 ➔ 領悟 (Insight) 】摩擦生熱與引火！\n" +
            "==================================================\n" +
            "你用乾燥的耐旱蕨類根部作為火絨，將質地堅硬的松科樹枝急速旋轉摩擦。\n" +
            "一縷青煙冒出，伴隨著微小的火星，火被點燃了！\n" + NOR);

        // 授與 Factor（此後 has_factor("thermodynamics") == true，守衛生效）
        object factor_svc = load_object("/runtime/services/factor_service.c");
        if (factor_svc) {
            factor_svc->discover_factor(me, "thermodynamics");
        }

        // 推進進度（progress_manager 內部也有防重複，雙重保護）
        object pm = load_object("/runtime/services/progress_manager.c");
        if (pm) {
            pm->complete_player_quest(me, "first_fire", "main", 100);
        }

        tell_object(me, YEL + "🔥 一堆小小的營火在你面前升起，驅散了黑暗與寒冷。\n" + NOR);
        room->set_long(room->query_long() + "\n" + YEL + "🔥 一堆營火在中央跳動著，照亮了原本黑暗的世界邊緣。" + NOR);

        // 營火 Reveal 了新路徑（由 YAML 動態計算與 user.c 自動輸出，這裡只提供後續引導）
        tell_object(me, YEL + "\n💡 你可以輸入 [ look ] 查看目前狀態與出口，或輸入 [ go forest ] 向蕨類森林前進。\n" +
            "   ⚠️  但是——你先前 focus smell 感知到了腥臭氣味，還有風向的問題...\n" +
            "   也許你應該先弄清楚：風在把你的氣味帶往哪裡？(focus wind)\n" + NOR);

    } else {
        tell_object(me, "你試圖摩擦 " + item1 + " 與 " + item2 + "，但這兩者無法產生足夠的摩擦生熱效果。\n");
    }
}
