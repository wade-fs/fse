// /runtime/services/i18n_service.c
// FSE 核心國際化 (i18n) 服務
// 支援從多個 locale 目錄載入 YAML 語系檔
#include "/include/ansi.h"

private mapping dictionary;
private string *locale_paths;

void create() {
    dictionary = ([]);
    locale_paths = ({});
    current_lang = "zh_TW";
}

// 註冊語系檔的搜尋目錄
void register_locale_path(string path) {
    if (member_array(path, locale_paths) == -1) {
        locale_paths += ({ path });
    }
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

// 重新載入所有已註冊目錄的語系檔
void reload_language() {
    dictionary = ([]);
    
    // 1. 先載入英文 (en_US) 作為預設 fallback
    foreach (string path in locale_paths) {
        // 模式 A：單一檔案
        load_locale_file(path + "/en_US.yaml");
        
        // 模式 B：多檔案合併 (如 locales/en_US/core.yaml)
        string *files = get_dir(path + "/en_US/*.yaml");
        if (arrayp(files) && sizeof(files) > 0) {
            foreach (string file in files) {
                load_locale_file(path + "/en_US/" + file);
            }
        }
    }
    
    // 2. 載入目前設定的語系覆蓋 fallback
    if (current_lang != "en_US") {
        foreach (string path in locale_paths) {
            // 模式 A
            load_locale_file(path + "/" + current_lang + ".yaml");
            
            // 模式 B
            string *files = get_dir(path + "/" + current_lang + "/*.yaml");
            if (arrayp(files) && sizeof(files) > 0) {
                foreach (string file in files) {
                    load_locale_file(path + "/" + current_lang + "/" + file);
                }
            }
        }
    }
}

void set_language(string lang) {
    current_lang = lang;
    reload_language();
}

// 獨立的 ANSI 色碼替換函式
string apply_color(string text) {
    if (!text || !stringp(text)) return text;
    text = replace_string(text, "$HIG$", HIG);
    text = replace_string(text, "$HIW$", HIW);
    text = replace_string(text, "$HIR$", HIR);
    text = replace_string(text, "$HIC$", HIC);
    text = replace_string(text, "$HIY$", HIY);
    text = replace_string(text, "$NOR$", NOR);
    text = replace_string(text, "$RED$", RED);
    return text;
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
    
    return apply_color(text);
}
