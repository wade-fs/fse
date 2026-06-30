// /adventures/pc/mudlib/cmds/strike.c
// strike [目標1] [目標2] — 敲擊指令 (第二個文明求生閉環：打製石器)
#include "/include/ansi.h"
inherit "/std/object";

void create() { ::create(); }

void main(object me, string arg) {
    object room = environment(me);
    if (!room) {
        tell_object(me, "你不在任何地方，無法敲擊物品。\n");
        return;
    }

    if (!arg || arg == "") {
        tell_object(me, "你想敲擊什麼？ (例如: strike obsidian obsidian)\n");
        return;
    }

    string item1, item2;
    if (sscanf(arg, "%s %s", item1, item2) != 2) {
        tell_object(me, "敲擊需要兩個介質彼此碰撞，例如：strike obsidian obsidian\n");
        return;
    }

    item1 = trim(item1);
    item2 = trim(item2);

    if (item1 == "obsidian" && item2 == "obsidian") {
        
        // ── 守衛：已獲得 flint_knapping (打製石器) 概念 ──
        if (me->has_factor("flint_knapping")) {
            tell_object(me, "你嫻熟地調整敲擊角度，輕敲黑曜石，剝落出薄而鋒利的石片。\n" +
                YEL + "🔪 你手中已經擁有極為锋利的黑曜石工具了。\n" + NOR);
            return;
        }

        // ── 前置守衛：必須先 focus ground 發現黑曜石 ──
        if (!me->query_temp("found_obsidian")) {
            me->player_confused("strike_without_materials");
            tell_object(me, RED + "【 🌀 產生困惑 】你胡亂敲擊周圍普通的石塊，除了震得手掌發麻、敲出一堆碎石粉末外，什麼也沒發生。\n" +
                "💡 這裡有適合製作工具的高硬度脆性礦石嗎？也許你應該觀察地面 (focus ground)..." + NOR + "\n");
            me->add_fatigue(10);
            return;
        }

        // ── 成功！觸發 Discovery 與領悟 ──
        tell_object(me, HIG + "==================================================\n" +
            "  🌟【 困惑 ➔ 領悟 (Insight) 】燧石打製與刃口形成！\n" +
            "==================================================\n" +
            "你將兩塊黑曜石以大約 45 度的夾角猛烈碰擊。\n" +
            "隨著清脆的破裂聲，石塊邊緣呈貝殼狀剝落，露出了薄如蟬翼、極為鋒利的邊緣！\n" + NOR);

        // 授與 Factor
        object factor_svc = load_object("/runtime/services/factor_service.c");
        if (factor_svc) {
            factor_svc->discover_factor(me, "flint_knapping");
        }

        // 獲得暫時工具標記
        me->set_temp("has_sharp_obsidian", 1);
        tell_object(me, YEL + "🔪 你獲得了 [ 鋒利的黑曜石片 ]。它可以用來切割堅韌的史前植物藤蔓。\n" + NOR);

        // 揭示通往峽谷的路徑（由 YAML 動態計算與 user.c 自動輸出，這裡只提供後續引導）
        tell_object(me, YEL + "💡 輸入 [ go canyon ] 前往捕食者峽谷。\n" + NOR);

    } else {
        tell_object(me, "你試圖敲擊 " + item1 + " 與 " + item2 + "，但除了一沉悶的撞擊聲外，沒有任何有用的脆性剝落。\n");
    }
}
