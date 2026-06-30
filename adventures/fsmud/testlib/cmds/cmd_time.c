// /cmds/cmd_time.c
#include "/include/ansi.h"

inherit "/std/object";

int main(object me, string verb, string arg) {
    object nature_d = load_object("/secure/nature_d.c");
    write(_t("current_game_time") + "：" + nature_d->query_weather_string() + "\n");
    write(_t("total_game_minutes") + "：" + nature_d->query_game_time() + "\n");
    return 1;
}

string help() {
    return "查看目前的遊戲時間與天氣。\n";
}
