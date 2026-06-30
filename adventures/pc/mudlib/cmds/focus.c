// /adventures/pc/mudlib/cmds/focus.c
#include "/include/ansi.h"
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
        tell_object(me, "你想專注於什麼感官？ (smell / sound / wind / ground)\n");
        return;
    }

    // 呼叫房間獲取特定感官訊號，傳入玩家以檢查其解鎖的 Factor
    string signal = room->query_sensory_signal(me, arg);
    tell_object(me, signal + "\n");
}
