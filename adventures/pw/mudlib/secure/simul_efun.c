// /secure/simul_efun.c
// 極簡 SimulEfun

string base_name(object ob) {
    if (!ob) return "";
    string file = file_name(ob);
    int hash = strsrch(file, "#");
    if (hash != -1) return substr(file, 0, hash);
    return file;
}

// 寫入日誌檔
void log_file(string file, string text) {
    write_file("/log/" + file, text);
}

// 統一錯誤捕捉並寫入日誌
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
