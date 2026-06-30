#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/npc.c";

void create() {
    ::create();
    set_name(([ "en": "Kyle", "zh-TW": "教官 凱爾", "zh-CN": "教官 凯尔" ]));
    set_short(([
        "en": "$HIC$Experienced Training Instructor$NOR$" + " Kyle",
        "zh-TW": "$HIC$經驗豐富的訓練教官$NOR$" + " 凱爾",
        "zh-CN": "$HIC$经验丰富的训练教官$NOR$" + " 凯尔"
    ]));
    set_long(([
        "en": "Kyle was once a captain of the Royal Guard. After retiring, he returned to the Newbie Village to serve as an instructor.\nHe is burly with sharp eyes, and his body is covered in scars from battle.\n",
        "zh-TW": "凱爾曾是一名王國衛隊的隊長，退休後回到新手村擔任教官。\n他身材魁梧，眼神銳利，身上佈滿了戰鬥留下的傷疤。\n",
        "zh-CN": "凯尔曾是一名王国卫队的队长，退休后回到新手村担任教官。\n他身材魁梧，眼神锐利，身上布满了战斗留下的伤疤。\n"
    ]));
    set_id(({"kyle", "trainer", "instructor", "教官"}));
    set_race("human");
    set_level(25);
    set_stat(STAT_STR, 28);
    set_stat(STAT_DEX, 25);
    recalc_stats();
    hp = max_hp;

    // 🚀 重生與走動
    set_respawn(60);
    set_wander_chance(1); // 教官很嚴肅，很少走動
    set_move_range(1);

    // 🚀 新增：傳授技能
    set_skill_to_teach("sword", 40);
    set_skill_to_teach("unarmed", 40);
    set_skill_to_teach("dodge", 40);
    set_skill_to_teach("parry", 40);

    // 🚀 新增：加入公會
    add_response(({ "join", "加入" }), (:
        object tp = this_player();
        if (load_object("/secure/guild_d.c")->join_guild(tp, "fighter")) {
            return "很好！戰士的道路沒有捷徑，只有不斷的磨練。";
        }
        return "如果你連拿起重劍的力量都沒有，就別想加入我們。";
    :));

    add_response(({ "戰鬥", "fight", "combat" }), ({
        "戰鬥不是兒戲，隨時保持警惕！",
        "力量與速度的結合才是取勝的關鍵。",
        "如果你想變強，就去外面找些怪物練練手吧。"
    }));
    add_response(({ "練習", "train", "practice" }), ({
        "去攻擊那邊的木人，我看你的姿勢正不正確。",
        "勤奮的練習是不會背叛你的。"
    }));
}
