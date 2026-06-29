// /monsters/proto_chicken.c  (史前文明 PC)
// 始祖小雞類 (Eoraptor-like) — 三疊紀最小的雙足肉食性生物
// 牠是玩家從 Level 0 → Level 1 的第一個挑戰
#include "/include/ansi.h"
inherit "/std/monster";

void create() {
    ::create();
    set_name("始祖小雞");
    set_short("一隻始祖小雞");
    set_long(
        "這是一隻體型如雞的早期恐龍，學名類似始盜龍（Eoraptor）。\n"
        "牠只有約半公尺高，全身覆蓋著鱗片，後腿強而有力，前肢短小。\n"
        "雖然看起來很像放大版的雞，但牠的牙齒鋒利，動作相當敏捷。\n"
        "在三疊紀的食物鏈中，對於手無寸鐵的人類來說，這已經是致命的威脅。\n"
    );
    set_max_hp(15);
    set_attack(3);
    set_defense(1);
    set_level(1);
    set_exp_value(20);     // 擊殺獲得 20 EXP，玩家需 20 EXP 升級
    set_respawn_delay(30); // 30 秒後重生
    set_combat_interval(2); // 每 2 秒一回合
}
