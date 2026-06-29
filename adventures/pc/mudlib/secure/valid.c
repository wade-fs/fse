// /secure/valid.c  (史前文明 PC)
// 極簡系統權限放行
inherit "/std/object";

void create() {
    ::create();
}

mixed valid_read(string path, object user, string func) {
    return 1;
}

mixed valid_write(string path, object user, string func) {
    return 1;
}
