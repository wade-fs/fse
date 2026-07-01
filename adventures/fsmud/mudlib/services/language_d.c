// /daemon/language_d.c
// 語系守護進程 (Language Daemon) - 輕量代理，底層委派給 FSE /runtime/services/i18n_service.c
//
// 職責：
//   - 作為 fsmud 既有程式碼（呼叫 LANGUAGE_D->translate() 等）的代理與轉發者
//   - 內部不再維護龐大的硬編碼翻譯字典，在啟動時自動將翻譯委派至 FSE i18n 服務。

#include "/runtime/include/ansi.h"

inherit "/std/object";

void create() {
    ::create();
    
    // 註冊本 Adventure 的語系目錄到 FSE 的 i18n_service 服務
    object i18n_svc = load_object("/runtime/services/i18n_service.c");
    if (i18n_svc) {
        i18n_svc->register_locale_path("/content/locales");
        i18n_svc->reload_language();
    }
}

// 供 MUD 既有程式碼調用，將請求轉發至 FSE 的 i18n_service
string translate(string key, string lang) {
    object i18n_svc = load_object("/runtime/services/i18n_service.c");
    if (!i18n_svc) return key;

    // 將 MUD 以前的語系代碼對齊
    if (lang == "zh-TW") lang = "zh_TW";
    else if (lang == "zh-CN") lang = "zh_CN";

    // 暫時將全域當前語系切換為目標語系以進行 translate
    // （若 i18n_svc 未來支援 translate(key, vars, lang)，可直接呼叫，目前它是使用全域狀態）
    string old_lang = i18n_svc->query_current_lang ? i18n_svc->query_current_lang() : "zh_TW";
    
    // 注意：因 runtime /runtime/services/i18n_service.c 的變數是全域的
    i18n_svc->set_language(lang);
    string res = i18n_svc->translate(key);
    i18n_svc->set_language(old_lang);
    
    return res;
}

// 廣播事件訊息給房間內的所有玩家 (自動按玩家語系翻譯)
void broadcast_event(object room, string key, mapping params) {
    if (!room) return;
    
    object *inv = all_inventory(room);
    if (!inv || sizeof(inv) == 0) return;

    // 方向本地化字典
    mapping dir_map = ([
        "north": ([ "en": "the north", "zh_TW": "北方", "zh_CN": "北方" ]),
        "south": ([ "en": "the south", "zh_TW": "南方", "zh_CN": "南方" ]),
        "east":  ([ "en": "the east",  "zh_TW": "東方", "zh_CN": "东方" ]),
        "west":  ([ "en": "the west",  "zh_TW": "西方", "zh_CN": "西方" ]),
        "up":    ([ "en": "above",     "zh_TW": "上方", "zh_CN": "上方" ]),
        "down":  ([ "en": "below",     "zh_TW": "下方", "zh_CN": "下方" ]),
        "here":  ([ "en": "somewhere", "zh_TW": "附近", "zh_CN": "附近" ])
    ]);
    
    foreach (object ob in inv) {
        if (!userp(ob) || !is_interactive(ob)) continue;
        
        string lang = ob->query_lang();
        if (!lang) lang = "en";
        if (lang == "zh-TW") lang = "zh_TW";
        else if (lang == "zh-CN") lang = "zh_CN";
        
        string msg = translate(key, lang);
        if (msg == key) continue;
        
        // 替換參數
        if (mapp(params)) {
            string *ks = keys(params);
            foreach (string p_key in ks) {
                mixed p_val = params[p_key];
                
                // 支援物件本地化名稱
                if (objectp(p_val)) {
                    p_val = p_val->query_localized_name(lang);
                } else if (p_key == "$dir" && stringp(p_val) && dir_map[p_val]) {
                    p_val = dir_map[p_val][lang] ? dir_map[p_val][lang] : dir_map[p_val]["en"];
                }
                
                msg = replace_string(msg, p_key, to_string(p_val));
            }
        }
        
        tell_object(ob, msg + "\n");
    }
}
