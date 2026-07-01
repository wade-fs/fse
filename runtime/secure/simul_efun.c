// /runtime/secure/simul_efun.c
// 全域模擬內建函式 (SimulEfuns) - 整合版本 (PC/PW/FSMUD)

#include "/runtime/include/ansi.h"

// === 基礎工具 (base_name) ===
string base_name(object ob) {
    if (!ob) return "";
    string file = file_name(ob);
    int hash = strsrch(file, "#");
    if (hash != -1) return substr(file, 0, hash);
    return file;
}

// === 寫入日誌檔案 ===
void log_file(string file, string message) {
    if (!stringp(file) || !stringp(message)) return;
    if (file_size("/log") != -2) {
        mkdir("/log");
    }
    write_file("/log/" + file, message);
}

// === 統一錯誤捕捉安全呼叫 ===
varargs mixed safe_call(object ob, string func, mixed arg1, mixed arg2, mixed arg3) {
    mixed res;
    mixed err;
    if (!ob || !func) return 0;
    err = catch(res = call_other(ob, func, arg1, arg2, arg3));
    if (err) {
        log_file("sys_error.log", sprintf("[%s] 執行 %s->%s 時發生錯誤: %s\n",
            ctime(time()), file_name(ob), func, err));
    }
    return res;
}

// === 取得物件名稱 ===
string get_name(object ob) {
    if (!ob) return "無名物";
    
    // 嘗試使用 safe_call 呼叫物件的 query_name() 或 query("name")
    mixed name = safe_call(ob, "query_name");
    if (!name) name = safe_call(ob, "query", "name");
    
    if (stringp(name)) return name;
    
    // 或是 query_id()
    mixed ids = safe_call(ob, "query_id");
    if (stringp(ids)) return ids;
    if (arrayp(ids) && sizeof(ids) > 0) return ids[0];
    
    // 最後手段：使用檔名
    string fname = object_name(ob);
    int pos = strsrch(fname, "#");
    if (pos != -1) fname = substr(fname, 0, pos);
    string *parts = explode(fname, "/");
    return sizeof(parts) > 0 ? parts[sizeof(parts)-1] : fname;
}

// === 根據指定語系選擇字串 ===
varargs string select_lang(mixed data, string lang) {
    if (stringp(data)) return data;
    if (!mapp(data)) return to_string(data);

    if (!lang || lang == "0") {
        object ob = previous_object();
        // 優先從呼叫者取得語系，使用 safe_call 避免崩潰
        if (ob) lang = safe_call(ob, "query_lang");
        
        // 如果呼叫者沒語系，試試 this_player()
        if (!lang || lang == "0") {
            object tp = this_player();
            if (tp) lang = safe_call(tp, "query_lang");
        }
    }

    if (!lang || lang == "0") lang = "en";

    if (data[lang]) return data[lang];
    if (data["en"]) return data["en"];

    mixed ks = keys(data);
    if (sizeof(ks) > 0) return data[ks[0]];
    return "None";
}

// === 翻譯輔助函式 ===
string _t(string key) {
    string lang = "en";
    object ob = previous_object();

    if (ob) lang = safe_call(ob, "query_lang");
    
    if (!lang || lang == "0") {
        object tp = this_player();
        if (tp) lang = safe_call(tp, "query_lang");
    }
    
    if (!lang || lang == "0") lang = "en";

    object lang_d = load_object("/services/language_d.c");
    if (lang_d) {
        return safe_call(lang_d, "translate", key, lang);
    }
    return key;
}

// === 強化版路徑解析 ===
varargs string resolv_path(string path, object user) {
    if (!user) user = this_player();
    if (!user) return path; // 沒人就回傳原路徑

    if (!path || path == "") {
        string c = safe_call(user, "query_cwd");
        return stringp(c) ? c : "/";
    }
    if (path[0] == '/') return path;
    
    // 如果不以 . 開頭，補上 ./ 使其符合 efun resolve_path 的相對路徑格式
    if (path[0] != '.') path = "./" + path;
    
    string cwd = safe_call(user, "query_cwd");
    if (!stringp(cwd) || cwd == "") cwd = "/";
    if (cwd[strlen(cwd)-1] != '/') cwd += "/";
    
    return resolve_path(path, cwd);
}

// === 陣列工具 ===
mixed *shuffle(mixed *arr) {
    if (!arrayp(arr)) return ({});
    mixed *tmp = copy(arr);
    int i = sizeof(tmp);
    while (i > 0) {
        int j = random(i);
        i--;
        mixed t = tmp[i];
        tmp[i] = tmp[j];
        tmp[j] = t;
    }
    return tmp;
}

mixed element_of(mixed *arr) {
    if (!arrayp(arr) || sizeof(arr) == 0) return 0;
    return arr[random(sizeof(arr))];
}

mixed random_element(mixed *arr) {
    return element_of(arr);
}

// 取得中文化的時間
string chinese_time() {
    return ctime(time());
}

// 訊息發送封裝
varargs void message(string category, string msg, mixed target, mixed exclude) {
    if (objectp(target)) {
        tell_object(target, msg);
    } else if (arrayp(target)) {
        foreach (object ob in target) {
            if (objectp(ob)) tell_object(ob, msg);
        }
    } else if (!target) {
        // 預設發送給目前房間
        object env = environment(this_player());
        if (env) tell_room(env, msg, exclude);
    }
}
