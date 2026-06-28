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
