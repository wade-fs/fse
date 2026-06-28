// /secure/valid.c
// 極簡系統權限放行
inherit "/std/object";

void create() {
    ::create();
}

mixed valid_read(string path, object user, string func) {
    return 1; // 預設全部允許讀取
}

mixed valid_write(string path, object user, string func) {
    return 1; // 預設全部允許寫入 (最小閉環簡化)
}
