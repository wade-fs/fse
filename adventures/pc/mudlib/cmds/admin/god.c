// /cmds/god.c (Wizard/GM 難度管理指令)
#include "/runtime/include/ansi.h"
inherit "/std/object";

void create() { ::create(); }

void main(object me, string arg) {
    // 檢查管理員權限 (FSE 系統簡化版驗證角色)
    if (me->query_role() != "wizard" && me->query_role() != "player") {
        tell_object(me, RED + "你沒有管理權限，無法使用此指令。\n" + NOR);
        return;
    }

    string raw = read_file("/manifest.yaml");
    if (!raw) {
        tell_object(me, RED + "無法讀取 /manifest.yaml 檔案。\n" + NOR);
        return;
    }
    mapping manifest = yaml_decode(raw);

    if (!arg || arg == "") {
        // 顯示當前狀態
        string cur = manifest["difficulty"]["current"];
        mapping mode = manifest["difficulty"]["modes"][cur];
        
        string out = HIW "\n====================================\n" +
                         "  👑 史前文明全域難度儀表板\n" +
                         "====================================\n" NOR;
        out += sprintf(" 當前全域難度: %s\n", HIR + upper_case(cur) + NOR);
        out += sprintf(" ├─ 疲勞消耗倍率: %.1f 倍\n", to_float(mode["fatigue_multiplier"]));
        out += sprintf(" ├─ 環境傷害倍率: %.1f 倍\n", to_float(mode["damage_multiplier"]));
        out += sprintf(" └─ 工具衰減倍率: %.1f 倍\n", to_float(mode["decay_multiplier"]));
        out += HIW "====================================\n" +
                   " 💡 使用 [ god difficulty <easy/normal/nightmare> ] 切換難度\n" + NOR;
        tell_object(me, out);
        return;
    }

    string cmd, val;
    if (sscanf(arg, "%s %s", cmd, val) != 2) {
        tell_object(me, "語法：god difficulty <easy/normal/nightmare>\n");
        return;
    }

    cmd = trim(cmd);
    val = trim(lower_case(val));

    if (cmd == "difficulty") {
        if (val != "easy" && val != "normal" && val != "nightmare") {
            tell_object(me, RED + "不支援的難度等級。請選擇：easy, normal, nightmare\n" + NOR);
            return;
        }

        // 修改 manifest.yaml 中的 current 難度
        manifest["difficulty"]["current"] = val;
        
        // 重新編碼回 yaml 並寫入
        string updated_yaml = yaml_encode(manifest);
        write_file("/manifest.yaml", updated_yaml, 1);

        tell_object(me, HIG + "👑 全域難度成功變更為：[" + upper_case(val) + "]！\n" + NOR);
        
        // 重新載入，模擬 master.c 宣告
        object i18n = load_object("/runtime/services/i18n_service.c");
        if (i18n) i18n->reload_language();
    } else {
        tell_object(me, "未知命令。請使用：god difficulty <mode>\n");
    }
}
