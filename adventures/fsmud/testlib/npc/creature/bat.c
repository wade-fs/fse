// npc/creature/bat.c - 蝙蝠（洞穴怪）
#include "/include/config.h"
inherit "/std/npc.c";

void create() {
    ::create();
    set_name(([ "en": "Bat", "zh-TW": "蝙蝠", "zh-CN": "蝙蝠" ]));
    set_short(([ "en": "a cave bat", "zh-TW": "一隻洞穴蝙蝠", "zh-CN": "一只洞穴蝙蝠" ]));
    set_long(([
        "en": "A small bat flapping its wings frantically.
",
        "zh-TW": "一隻拍打著翅膀的小蝙蝠。
",
        "zh-CN": "一只拍打着翅膀的小蝙蝠。
"
    ]));
    set_id(({"bat","蝙蝠"}));

    level    = 2;
    stat_str = 6;
    stat_dex = 8;
    stat_con = 4;
    recalc_stats();
    hp = max_hp;

    set_exp_reward(40);
    set_gold_reward(2);
    set_respawn(20);
    set_aggro_msg("蝙蝠尖叫著向你俯衝而來！
");
}
