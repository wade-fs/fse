// /runtime/services/localization_service.c
// 中文指令正規化與同義詞對照轉換服務
#include "/runtime/include/ansi.h"

private mapping verb_map; // alias -> standard_verb
private mapping noun_map; // alias -> standard_noun
private string *sorted_verb_aliases; // 用於最長匹配的前置過濾

void create() {
    verb_map = ([]);
    noun_map = ([]);
    sorted_verb_aliases = ({});

    string v_raw = read_file("/content/locales/zh-TW/verbs.yaml");
    if (v_raw) {
        mapping v_cfg = yaml_decode(v_raw);
        if (v_cfg) {
            foreach (string std_v, mapping data in v_cfg) {
                if (data["aliases"]) {
                    foreach (string alias in data["aliases"]) {
                        verb_map[alias] = std_v;
                        if (member_array(alias, sorted_verb_aliases) == -1) {
                            sorted_verb_aliases += ({ alias });
                        }
                    }
                }
            }
        }
    }

    string n_raw = read_file("/content/locales/zh-TW/nouns.yaml");
    if (n_raw) {
        mapping n_cfg = yaml_decode(n_raw);
        if (n_cfg) {
            foreach (string std_n, mapping data in n_cfg) {
                if (data["aliases"]) {
                    foreach (string alias in data["aliases"]) {
                        noun_map[alias] = std_n;
                    }
                }
            }
        }
    }

    // 將別名按長度降序排序，確保進行最長匹配（例如先匹配「點燃」而非「點」）
    sorted_verb_aliases = sort_array(sorted_verb_aliases, "sort_by_length_desc", this_object());
}

int sort_by_length_desc(string a, string b) {
    return strlen(b) - strlen(a);
}

// 將輸入的動詞與受詞轉換為系統內部的英文 Action 和 Target
mapping translate_input(string verb, string arg) {
    string action = verb;
    string target = arg;

    // 1. 處理無空格的中文複合輸入 (例如: "點燃木柴" / "看壁畫")
    if (arg == "" || !arg) {
        foreach (string valias in sorted_verb_aliases) {
            if (strsrch(verb, valias) == 0) {
                action = verb_map[valias];
                target = replace_string(verb, valias, "", 1);
                break;
            }
        }
    } else {
        // 有空格，獨立對動詞進行映射
        if (verb_map[verb]) {
            action = verb_map[verb];
        }
    }

    // 2. 處理受詞同義詞轉換 (可能是多個空格分隔的詞，例如 "rocks water")
    if (target != "") {
        string *parts = explode(target, " ");
        string *translated = ({});
        foreach (string part in parts) {
            part = trim(part);
            if (part == "") continue;
            if (noun_map[part]) {
                translated += ({ noun_map[part] });
            } else {
                translated += ({ part }); // 找不到，保留原樣
            }
        }
        target = implode(translated, " ");
    }

    return ([
        "action" : action,
        "target" : target
    ]);
}
