// /cmds/kill.c  (史前文明 PC)
// kill <目標> — 對目標發動攻擊，開始戰鬥
#include "/include/ansi.h"
inherit "/std/object";

void create() { ::create(); }

void combat_round(object me);

void main(object me, string arg) {
    if (!arg || arg == "") {
        tell_object(me, "要攻擊誰？（kill <目標名稱>）\n");
        return;
    }
    if (me->query_in_combat()) {
        tell_object(me, "你已經在戰鬥中了！\n");
        return;
    }

    object room = environment(me);
    if (!room) { tell_object(me, "你不在任何地方。\n"); return; }

    object target = 0;
    foreach (object ob in room->query_occupants()) {
        if (ob == me) continue;
        if (function_exists("query_name", ob) &&
            lower_case(ob->query_name()) == lower_case(arg)) {
            target = ob;
            break;
        }
    }

    if (!target) {
        tell_object(me, "你找不到「" + arg + "」。\n");
        return;
    }
    if (!target->is_alive()) {
        tell_object(me, target->query_name() + " 已經死了。\n");
        return;
    }

    // 宣告戰鬥開始
    tell_object(me, RED + "你向 " + target->query_name() + " 發動攻擊！\n" + NOR);
    foreach (object ob in room->query_occupants()) {
        if (ob != me) tell_object(ob, me->query_name() + " 向 " + target->query_name() + " 發動攻擊！\n");
    }

    me->start_combat(target);

    // 怪物進入反擊狀態
    if (function_exists("engage", target)) {
        target->engage(me);
    }

    // 玩家戰鬥心跳
    call_out("combat_round", 2, me);
}

// 玩家戰鬥回合（獨立於怪物 AI 回合）
void combat_round(object me) {
    if (!objectp(me) || !me->query_in_combat()) return;
    object target = me->query_combat_target();
    if (!target || !objectp(target) || !target->is_alive()) {
        me->stop_combat();
        return;
    }
    // 玩家攻擊
    me->do_attack(target);

    // 怪物死亡
    if (!target->is_alive()) {
        me->stop_combat();
        // 觸發怪物死亡流程
        if (function_exists("on_death", target)) {
            target->on_death(me);
        }
        return;
    }
    // 繼續下一回合
    call_out("combat_round", 2, me);
}
