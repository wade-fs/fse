// /adventures/pc/mudlib/cmds/go.c
// go [方向] — 移動到指定出口
#include "/include/ansi.h"
inherit "/std/object";

void create() { ::create(); }

void main(object me, string arg) {
    object room = environment(me);
    if (!room) {
        tell_object(me, "你不在任何地方。\n");
        return;
    }

    arg = trim(lower_case(arg));
    if (!arg || arg == "") {
        mapping exits = room->query_exits();
        if (!exits || !sizeof(exits)) {
            tell_object(me, "這裡沒有明顯的出口。\n");
        } else {
            tell_object(me, "你可以往以下方向移動：" + implode(keys(exits), "  ") + "\n");
        }
        return;
    }

    mapping exits = room->query_exits();
    if (!exits || undefinedp(exits[arg])) {
        tell_object(me, YEL + "「" + arg + "」方向沒有出路。\n" + NOR);

        // 若玩家知道有出口但還沒解鎖，給一點方向感
        if (exits && sizeof(exits) > 0) {
            tell_object(me, "💡 目前可通行的方向：" + implode(keys(exits), "  ") + "\n");
        } else {
            tell_object(me, "💡 也許你需要先探索環境，才能找到離開的路。\n");
        }
        return;
    }

    string dest = exits[arg];
    object dest_room = load_object(dest);
    if (!dest_room) {
        tell_object(me, RED + "那個方向的路似乎被某種力量阻擋了。\n" + NOR);
        return;
    }

    // 離開舊房間
    room->leave(me);

    // 進入新房間
    move_object(me, dest_room);
    dest_room->enter(me);

    // 顯示新房間描述
    tell_object(me, "\n" + dest_room->describe(me));

    // 感官提示
    tell_object(me, YEL + "\n💡 在新環境中，試著 focus [smell/sound/wind/ground] 感知周遭。\n" + NOR);
}
