// /cmds/inventory.c  (史前文明背包/持有物品查詢)
#include "/include/ansi.h"
inherit "/std/object";

void create() { ::create(); }

void main(object me, string arg) {
    object *inv = all_inventory(me);
    // 過濾出符合 item.c 類型的物件
    inv = filter(inv, (: function_exists("query_item_id", $1) :));

    if (sizeof(inv) == 0) {
        tell_object(me, YEL + "你目前兩手空空，沒有攜帶任何史前工具與火種。\n" + NOR);
        return;
    }

    string out = HIW "\n====================================\n" +
                     "  🎒 你持有的史前實體物資：\n" +
                     "====================================\n" NOR;
    foreach (object ob in inv) {
        out += sprintf(" - %-20s : %s\n", 
            CYN + ob->query_short() + NOR, 
            ob->query_long());
    }
    out += HIW "====================================\n" NOR;
    tell_object(me, out);
}
