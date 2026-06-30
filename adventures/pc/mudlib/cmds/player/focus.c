// /adventures/pc/mudlib/cmds/focus.c
#include "/runtime/include/ansi.h"
inherit "/std/object";

void create() { ::create(); }

void main(object me, string arg) {
    object room = environment(me);
    if (!room) {
        tell_object(me, "你處於虛無之中，無法聚焦感官。\n");
        return;
    }

    arg = trim(arg);
    if (arg != "smell" && arg != "sound" && arg != "wind" && arg != "ground") {
        // 若玩家輸入 focus forest / focus fire 等，給予情境提示而非冷冰冰的錯誤
        string hint = "你試圖感知「" + arg + "」，但你的感官沒有辦法直接感受一個地點或物件。\n";
        hint += "💡 你只能透過以下感官去「理解」周遭的世界：\n";
        hint += "   focus smell  — 嗅聞空氣中的氣息\n";
        hint += "   focus sound  — 傾聽周遭的聲音\n";
        hint += "   focus wind   — 感受風的方向與氣味\n";
        hint += "   focus ground — 俯身貼地，感知震動與地貌\n";
        tell_object(me, hint);
        return;
    }

    // 呼叫房間獲取特定感官訊號，傳入玩家以檢查其解鎖的 Factor
    string signal = room->query_sensory_signal(me, arg);
    tell_object(me, signal + "\n");
}
