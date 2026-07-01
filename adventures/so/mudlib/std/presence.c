// /std/presence.c (蜀山遊記 SO 存在/掠食者/因果障礙)
#include "/runtime/include/ansi.h"
inherit "/std/living";

protected string true_name;
protected string disguised_name;
protected string *recognition_factors;

void create() {
    ::create();
    true_name = "神祕存在";
    disguised_name = "一團模糊的黑影";
    recognition_factors = ({});
}

// 設定與查詢認知辨識 Factors
void set_recognition_rules(string t_name, string d_name, string *factors) {
    true_name = t_name;
    disguised_name = d_name;
    recognition_factors = factors;
}

string *query_identifiable_by_factors() {
    return recognition_factors;
}

// 覆寫 LPC query_name 進行過濾
string query_name() {
    object viewer = this_player();
    if (!viewer) return true_name;

    // 如果沒有設定辨識條件，或者觀看者擁有所有需要的 factors，顯示真名
    if (!recognition_factors || sizeof(recognition_factors) == 0) {
        return true_name;
    }

    int identified = 1;
    foreach (string factor in recognition_factors) {
        if (viewer && function_exists("has_factor", viewer) && !viewer->has_factor(factor)) {
            identified = 0;
            break;
        }
    }

    return identified ? true_name : disguised_name;
}

// 覆寫 query_short
string query_short() {
    return query_name();
}
