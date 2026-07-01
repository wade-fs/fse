// /std/living.c (蜀山遊記 SO 生物基底類別)
// 職責：所有有名字、有描述、可被感知的存在體的最小共同介面。
// 刻意不包含 hp/level 等 MUD 戰鬥數值——
// 若某個子類需要「可受傷」，由子類自行宣告或由 room.c 的副作用處理。
#include "/runtime/include/ansi.h"
inherit "/std/object";

private string name;
private string short_desc;
private string long_desc;

void create() {
    ::create();
    name       = "未知存在";
    short_desc = "一個無從辨識的存在。";
    long_desc  = "你感知不到它清晰的輪廓。";
}

void   set_name(string n)  { name = n; }
string query_name()        { return name; }
void   set_short(string s) { short_desc = s; }
string query_short()       { return short_desc; }
void   set_long(string l)  { long_desc = l; }
string query_long()        { return long_desc; }
