// /cmds/admin/cmd_tests.c
// 執行系統自動化測試指令

inherit "/std/object";

#include "/include/ansi.h"

int main(object me, string verb, string arg) {
    if (me->query_role() != "god") {
        write("只有管理員可以使用此指令。\n");
        return 1;
    }

    write("$HIW$🚀 開始執行 MudScript 核心功能驗證...\n$NOR$");

    string old_lang = me->query_lang();
    me->set_lang("zh-TW"); // 🚀 強制設定為中文，以滿足測試案例中的硬編碼斷言

    string *test_files = ({
        "/tests/test_quit_restore.c",
        "/tests/test_efuns.c",
        "/tests/test_initialization.c",
        "/tests/test_syntax.c",
        "/tests/test_slice.c",
        "/tests/test_closure_type.c",
        "/tests/test_logic.c",
        "/tests/test_coords.c",
        "/tests/test_control_flow.c",
        "/tests/test_logic.c",

        "/tests/test_nature.c",
        "/tests/test_save.c",
        "/tests/test_ternary.c",
        "/tests/test_security.c",
        "/tests/test_language.c",
        "/tests/test_quest.c",
        "/tests/test_crafting.c",
        "/tests/test_skills.c",
        "/tests/test_combat_classes.c",
        "/tests/test_guilds.c",
        "/tests/test_combat_team.c",
        "/tests/test_currency_chat.c",
        "/tests/test_pets_mounts.c",
        "/tests/test_death_corpse.c",
        "/tests/test_incident.c",
        "/tests/test_i18n.c",
        "/tests/test_new_efuns.c",
        "/tests/test_init_action.c",
        "/tests/test_look_dir.c",
        "/tests/test_localization.c",
        "/tests/p3_test.c",
        "/tests/p4_test.c",
        "/tests/test_file_mgmt.c",
        "/tests/test_var_decl.c",
        "/tests/test_yaml.c",
        "/tests/test_canon_world.c",
        "/tests/test_reveal_validation.c",
        "/tests/test_log_file.c",
        "/tests/test_look_travel_debug.c"
    });

    int total = 0;
    int passed = 0;

    foreach (string file in test_files) {
        object test_ob = load_object(file);
        if (!test_ob) {
            write("$HIR$❌ 無法載入測試檔案: $NOR$" + file + "\n");
            continue;
        }
        
        test_ob->run_tests(me);
        total += test_ob->query_total();
        passed += test_ob->query_passed();
        
        // 測試完後銷毀，確保下次執行是乾淨的
        destruct(test_ob);
    }

    write("$HIW$==========================================$NOR$" + "\n");
    if (passed == total) {
        write("$HIG$✨ 恭喜！所有核心測試全部通過！(" + passed + "/" + total + ")$NOR$" + "\n");
    } else {
        write("$HIR$⚠️ 警報！共有 " + (total - passed) + " 個測試項目失敗！$NOR$" + "\n");
    }
    write("$HIW$==========================================$NOR$" + "\n");

    me->set_lang(old_lang); // 🚀 恢復原本語系

    return total - passed; // 回傳失敗次數
}

string help() {
    return "【管理指令】\n" +
           "  tests    執行系統核心功能自動化測試，驗證直譯器正確性\n";
}

string *query_verbs() { return ({ "tests" }); }
string query_category() { return "Admin"; }
