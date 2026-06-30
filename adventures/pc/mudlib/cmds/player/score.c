// /cmds/score.c  (史前文明 PC)
// score — 顯示玩家目前的狀態 (符合 FSE 精神)
#include "/runtime/include/ansi.h"
inherit "/std/object";

void create() { ::create(); }

void main(object me, string arg) {
    int hp     = me->query_hp();
    int max_hp = me->query_max_hp();
    int fatigue = me->query_fatigue();

    int hunger = me->query_hunger();
    int thirst = me->query_thirst();

    // HP 顏色
    string hp_color = GRN;
    if (hp * 100 / max_hp < 30) hp_color = RED;
    else if (hp * 100 / max_hp < 60) hp_color = YEL;

    // Fatigue 顏色 (越低越安全)
    string fat_color = GRN;
    if (fatigue > 80) fat_color = RED;
    else if (fatigue > 50) fat_color = YEL;

    // Hunger & Thirst 顏色
    string hunger_color = GRN;
    if (hunger > 80) hunger_color = RED;
    else if (hunger > 50) hunger_color = YEL;

    string thirst_color = GRN;
    if (thirst > 80) thirst_color = RED;
    else if (thirst > 50) thirst_color = YEL;

    tell_object(me,
        HIG + "╔══════════════════════════════╗\n" + NOR +
        HIG + "║      史前文明 — 求生狀態      ║\n" + NOR +
        HIG + "╠══════════════════════════════╣\n" + NOR +
        sprintf(HIG + "║" + NOR + " 代號  ：%-22s " + HIG + "║\n" + NOR, me->query_id()) +
        sprintf(HIG + "║" + NOR + " 生命值：" + hp_color + "%d/%d" + NOR + "%-20s" + HIG + "║\n" + NOR, hp, max_hp, "") +
        sprintf(HIG + "║" + NOR + " 疲勞值：" + fat_color + "%d / 100" + NOR + "%-17s" + HIG + "║\n" + NOR, fatigue, "") +
        sprintf(HIG + "║" + NOR + " 飢餓度：" + hunger_color + "%d / 100" + NOR + "%-17s" + HIG + "║\n" + NOR, hunger, "") +
        sprintf(HIG + "║" + NOR + " 口渴度：" + thirst_color + "%d / 100" + NOR + "%-17s" + HIG + "║\n" + NOR, thirst, "") +
        HIG + "╚══════════════════════════════╝\n" + NOR
    );

    // 顯示 progress_manager 的進度與解鎖因素
    object pm = load_object("/runtime/services/progress_manager.c");
    if (pm) {
        string stage = pm->query_current_stage(me, "main");
        if (stage) {
            tell_object(me, CYN + "  🔹 生存階段：" + BOLD + stage + NOR + "\n");
        }
    }

    // 顯示已解鎖的 FSE 生存因素
    object factor_svc = load_object("/runtime/services/factor_service.c");
    if (factor_svc) {
        tell_object(me, YEL + "  🔸 領悟因素 (Discoveries)：\n" + NOR);
        int has_any = 0;
        string *all_factors = ({
            "thermodynamics", "flint_knapping", "stealth_camouflage",
            "oxygen_scarcity", "heat_regulation", "co2_toxicity", "water_boiling"
        });
        foreach (string fid in all_factors) {
            if (me->has_factor(fid)) {
                mapping f_data = factor_svc->load_factor_data(fid);
                string fname = f_data ? f_data["name"] : fid;
                tell_object(me, GRN + "     - " + fname + " (" + fid + ")\n" + NOR);
                has_any = 1;
            }
        }
        if (!has_any) {
            tell_object(me, GRN + "     (尚無任何領悟。試著 focus 不同的感官，或從失敗中摸索)\n" + NOR);
        }
    }
}

