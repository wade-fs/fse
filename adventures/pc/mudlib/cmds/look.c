// /cmds/look.c  (史前文明 PC)
// look [目標] — 觀察房間或特定生物
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
    // 尋找目標
    foreach (object ob in room->query_occupants()) {
        if (ob == me) continue;
        if (function_exists("query_name", ob) &&
            lower_case(ob->query_name()) == lower_case(arg)) {
            tell_object(me, HIG + ob->query_name() + NOR + "\n");
            tell_object(me, ob->query_long() + "\n");
            if (function_exists("query_hp", ob)) {
                tell_object(me, sprintf("HP: %d / %d\n", ob->query_hp(), ob->query_max_hp()));
            }
            return;
        }
    }
    tell_object(me, "你看不到「" + arg + "」。\n");
}
