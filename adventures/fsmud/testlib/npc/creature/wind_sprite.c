// npc/creature/wind_sprite.c - 風之精靈（天空，Lv2，被動）
#include "/include/config.h"
inherit "/std/npc.c";

void create() {
    ::create();
    set_name(([ "en": "Wind Sprite", "zh-TW": "風之精靈", "zh-CN": "风之精灵" ]));
    set_short(([ "en": "An ethereal wind sprite", "zh-TW": "一隻飄逸的風之精靈", "zh-CN": "一只飘逸的风之精灵" ]));
    set_long(([ "en": "This sprite's translucent body sways with the wind. One can hear it emitting a soft hum, as if recounting tales of the sky.\n", "zh-TW": "這隻精靈半透明的身軀隨風搖曳，能聽見它發出輕柔的哼鳴聲，彷彿在訴說著天空的故事。\n", "zh-CN": "这只精灵半透明的身躯随风摇曳，能听见它发出轻柔的哼鸣声，仿佛在诉说著天空的故事。\n" ]));
    set_id(({"wind_sprite", "風之精靈", "精靈", "風精靈"}));

    level    = 2;
    stat_str = 5;
    stat_dex = 20;
    stat_int = 14;
    stat_con = 5;
    recalc_stats();
    hp = max_hp;

    set_habitat(HABITAT_SKY);
    set_behaviour(BEHAV_PASSIVE);

    set_exp_reward(40);
    set_gold_reward(5);
    set_respawn(120);

    add_response("天空", ({
        "天空廣闊無垠，自由才是最美麗的事物。",
        "在雲朵之上，你可以看到不一樣的世界。",
        "如果你也有翅膀，就能感受到風的輕撫。"
    }));
    add_response("風", ({
        "風無所不在，它帶著各地的消息飄來飄去。",
        "聽聽風的聲音，它會告訴你遠方的故事。",
        "有時候風很溫柔，有時候它也會展現強大的一面。"
    }));
    add_response("飛行", ({
        "若你想飛翔，就必須先學會相信自己！",
        "飛行是靈魂的渴望，不只是翅膀的作用。",
        "當你穿過雲層時，那種感覺是無與倫比的。"
    }));
}
