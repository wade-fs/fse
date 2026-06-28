// /runtime/services/i18n_service.c
// FSE 核心國際化 (i18n) 服務
// 支援從多個 locale 目錄載入 YAML 語系檔
#include "/include/ansi.h"

private mapping dictionary;
private string current_lang;

void create() {
    dictionary = ([]);
    current_lang = "zh_TW";
    set_language(current_lang, 0); // 預設先載入 core 語系
}

// 載入特定的 yaml 語系檔並合併到當前字典中
void load_locale_file(string file_path) {
    string raw = read_file(file_path);
    if (!raw) return;
    
    mixed err;
    mapping data;
    
    err = catch(data = yaml_decode(raw));
    if (err || !data) {
        log_file("sys_error.log", sprintf("[%s] i18n_service: 無法解析語系檔 %s - %s\n", ctime(time()), file_path, err));
        return;
    }
    
    // 合併字典
    foreach (string key, string val in data) {
        dictionary[key] = val;
    }
}

// 設定當前語系，並重新載入核心與冒險的語系檔
void set_language(string lang, string adventure_path) {
    current_lang = lang;
    dictionary = ([]);
    
    // 1. 載入 Core 語系檔 (例如 /runtime/locales/zh_TW.yaml)
    load_locale_file("/runtime/locales/" + lang + ".yaml");
    
    // 2. 載入 Adventure 語系檔 (例如 /adventures/pw/mudlib/content/locales/zh_TW.yaml)
    if (adventure_path) {
        load_locale_file(adventure_path + "/locales/" + lang + ".yaml");
    }
}

// 取得翻譯字串 (支援基礎變數替換，例如 {name})
varargs string translate(string key, mapping vars) {
    if (!dictionary[key]) return key; // 找不到翻譯則回傳 key 作為 fallback
    
    string text = dictionary[key];
    if (vars) {
        foreach (string v_key, string v_val in vars) {
            text = replace_string(text, "{" + v_key + "}", v_val);
        }
    }
    
    // 支援直接解析 ANSI 色碼替換 (方便在 YAML 寫 $HIG$ 之類的)
    text = replace_string(text, "$HIG$", HIG);
    text = replace_string(text, "$HIW$", HIW);
    text = replace_string(text, "$HIR$", HIR);
    text = replace_string(text, "$HIC$", HIC);
    text = replace_string(text, "$HIY$", HIY);
    text = replace_string(text, "$NOR$", NOR);
    text = replace_string(text, "$RED$", RED);
    
    return text;
}
