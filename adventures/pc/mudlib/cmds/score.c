// /cmds/score.c  (史前文明 PC)
// score — 顯示玩家目前的狀態
#include "/include/ansi.h"
inherit "/std/object";

void create() { ::create(); }

void main(object me, string arg) {
    int hp     = me->query_hp();
    int max_hp = me->query_max_hp();
    int lv     = me->query_level();
    int atk    = me->query_attack();
    int def    = me->query_defense();

    // HP 顏色
    string hp_color = GRN;
    if (hp * 100 / max_hp < 30) hp_color = RED;
    else if (hp * 100 / max_hp < 60) hp_color = YEL;

    tell_object(me,
        HIG + "╔══════════════════════════════╗\n" + NOR +
        HIG + "║   史前文明 — 求生狀態        ║\n" + NOR +
        HIG + "╠══════════════════════════════╣\n" + NOR +
        sprintf(HIG + "║" + NOR + " 名稱：%-22s " + HIG + "║\n" + NOR, me->query_name()) +
        sprintf(HIG + "║" + NOR + " 等級：%-22d " + HIG + "║\n" + NOR, lv) +
        sprintf(HIG + "║" + NOR + " HP  ：" + hp_color + "%d/%d" + NOR + "%-20s" + HIG + "║\n" + NOR,
            hp, max_hp, "") +
        sprintf(HIG + "║" + NOR + " 攻擊：%-22d " + HIG + "║\n" + NOR, atk) +
        sprintf(HIG + "║" + NOR + " 防禦：%-22d " + HIG + "║\n" + NOR, def) +
        HIG + "╚══════════════════════════════╝\n" + NOR
    );

    // 顯示 progress_manager 的進度
    object pm = load_object("/runtime/services/progress_manager.c");
    if (pm) {
        string stage = pm->query_current_stage(me, "main");
        if (stage) {
            tell_object(me, CYN + "  生存階段：" + stage + NOR + "\n");
        }
    }
}
