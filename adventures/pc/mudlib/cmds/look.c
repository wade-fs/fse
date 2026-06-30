// /cmds/look.c  (史前文明 PC)
// look [目標] — 觀察房間或特定生物，符合 FSE 精神 (感知層過濾)
#include "/include/ansi.h"
inherit "/std/object";

void create() { ::create(); }

void main(object me, string arg) {
    object room = environment(me);
    if (!room) {
        tell_object(me, "你不在任何地方。\n");
        return;
    }
    if (!arg || arg == "") {
        tell_object(me, room->describe(me));
        return;
    }

    // 只有解鎖了戰鬥生存或基礎觀察，才能看清恐龍細節
    if (!me->has_factor("combat_survival") && !me->has_factor("predator_scent")) {
        tell_object(me, YEL + "在昏暗而陌生的史前光線下，你無法看清「" + arg + "」的細節，貿然直視只會引起危險。\n" +
            "💡 試著利用感官 focus 來理解周圍事物。\n" + NOR);
        return;
    }

    // 尋找目標
    foreach (object ob in room->query_occupants()) {
        if (ob == me) continue;
        if (function_exists("query_name", ob) &&
            lower_case(ob->query_name()) == lower_case(arg)) {
            tell_object(me, HIG + ob->query_name() + NOR + "\n");
            tell_object(me, ob->query_long() + "\n");
            return;
        }
    }
    tell_object(me, "你看不到「" + arg + "」。\n");
}

