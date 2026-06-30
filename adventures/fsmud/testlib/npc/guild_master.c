#include "/include/config.h"
#include "/include/ansi.h"
inherit "/std/npc.c";

void create() {
    ::create();
    set_name(([ "en": "Guild Master Edmund", "zh-TW": "公會會長 艾德蒙", "zh-CN": "公会会长 艾德蒙" ]));
    set_short(([ "en": "$HIW$Adventurer Guild Master$NOR$" + " Edmund", "zh-TW": "$HIW$冒險者公會會長$NOR$" + " 艾德蒙", "zh-CN": "$HIW$冒险者公会会长$NOR$" + " 艾德蒙" ]));
    set_long(([
        "en": "An experienced human warrior who is the head of this guild.\nHe wears a polished silver armor, and his eyes show authority.\n",
        "zh-TW": "一位看起來經驗豐富的人類戰士，他是這間公會的負責人。\n他穿著一套擦得發亮的銀色鎧甲，眼神中透著威嚴。\n",
        "zh-CN": "一位看起来经验丰富的人类战士，他是这间公会的负责人。\n他穿着一套擦得发亮的银色铠甲，眼神中透着威严。\n"
    ]));
    set_id(({"edmund", "guild master", "master", "會長"}));
    set_race("human");
    set_level(30);
    set_stat(STAT_STR, 25);
    set_stat(STAT_CON, 25);
    recalc_stats();
	set_move_range(2);
	set_wander_chance(2); // 調低機率，免得老是不在公會
	set_respawn(60);
	set_behaviour(BEHAV_PASSIVE|BEHAV_WANDER);

    // 🚀 新增：傳授技能
    set_skill_to_teach("unarmed", 20);
    set_skill_to_teach("magic", 20);

	set_flee_hp_pct(15);
	set_flee_chance(80);

    hp = max_hp;

    add_response("job", ({ 
        ([ "en": "There are wolves in the eastern plains recently, you can check it out.", "zh-TW": "最近東邊的草原有狼群出沒，你可以去看看。", "zh-CN": "最近东边的草原有狼群出没，你可以去看看。" ]),
        ([ "en": "I heard some strange caves appeared at the foot of the northern mountains, maybe you can investigate.", "zh-TW": "聽說北方的山腳下出現了一些奇怪的洞穴，或許你可以去調查一下。", "zh-CN": "听说北方的山脚下出现了一些奇怪的洞穴，或许你可以去调查一下。" ]),
        ([ "en": "The guild is currently short on herbs, it would be great if you could collect some.", "zh-TW": "公會現在正缺少一些藥草，如果你能去採些回來就太好了。", "zh-CN": "公会现在正缺少一些药草，如果你能去采些回来就太好了。" ])
    }));
    add_response("工作", ({ 
        ([ "en": "There are wolves in the eastern plains recently, you can check it out.", "zh-TW": "最近東邊的草原有狼群出沒，你可以去看看。", "zh-CN": "最近东边的草原有狼群出没，你可以去看看。" ]),
        ([ "en": "I heard some strange caves appeared at the foot of the northern mountains, maybe you can investigate.", "zh-TW": "聽說北方的山腳下出現了一些奇怪的洞穴，或許你可以去調查一下。", "zh-CN": "听说北方的山脚下出现了一些奇怪的洞穴，或许你可以去调查一下。" ]),
        ([ "en": "The guild is currently short on herbs, it would be great if you could collect some.", "zh-TW": "公會現在正缺少一些藥草，如果你能去採些回來就太好了。", "zh-CN": "公会现在正缺少一些药草，如果你能去采些回来就太好了。" ])
    }));
    add_response("guild", ({
        ([ "en": "This place provides a venue for adventurers to rest and exchange information.", "zh-TW": "這裡提供冒險者休息與交流的場所。", "zh-CN": "这里提供冒险者休息与交流的场所。" ]),
        ([ "en": "The guild is your strongest support, no matter what difficulties you encounter, you can come back for help.", "zh-TW": "公會是你最強大的後盾，不論你遇到什麼困難，都可以回來尋求協助。", "zh-CN": "公会是你最强大的后盾，不论你遇到什么困难，都可以回来寻求协助。" ]),
        ([ "en": "Want to join the guild? As long as you have a brave heart!", "zh-TW": "想加入公會嗎？只要你有顆勇敢的心！", "zh-CN": "想加入公会吗？只要你有颗勇敢的心！" ])
    }));
    add_response("公會", ({
        ([ "en": "This place provides a venue for adventurers to rest and exchange information.", "zh-TW": "這裡提供冒險者休息與交流的場所。", "zh-CN": "这里提供冒险者休息与交流的场所。" ]),
        ([ "en": "The guild is your strongest support, no matter what difficulties you encounter, you can come back for help.", "zh-TW": "公會是你最強大的後盾，不論你遇到什麼困難，都可以回來尋求協助。", "zh-CN": "公会是你最强大的后盾，不论你遇到什么困难，都可以回來尋求協助。" ]),
        ([ "en": "Want to join the guild? As long as you have a brave heart!", "zh-TW": "想加入公會嗎？只要你有顆勇敢的心！", "zh-CN": "想加入公会吗？只要你有颗勇敢的心！" ])
    }));
    add_response("help", ({
        ([ "en": "Newbies can go west to Thorin's forge for a weapon, or south to the apothecary for potions.", "zh-TW": "新手的話，可以先去[鐵匠鋪|go west]找索林買把武器，或者去[藥劑店|go south]備些藥水。", "zh-CN": "新手的话，可以先去[铁匠铺|go west]找索林买把武器，或者去[药剂店|go south]备些药水。" ]),
        ([ "en": "Remember, timely retreat is also a part of adventure.", "zh-TW": "記住，適時的撤退也是冒險的一部分。", "zh-CN": "记住，适时的撤退也是冒险的一部分。" ]),
        ([ "en": "Talk more with others, you will reap unexpected information.", "zh-TW": "多和其他人交談，你會收穫意想不到的情報。", "zh-CN": "多和其他人交谈，你会收获意想不到的情报。" ])
    }));
    add_response("幫助", ({
        ([ "en": "Newbies can go west to Thorin's forge for a weapon, or south to the apothecary for potions.", "zh-TW": "新手的話，可以先去[鐵匠鋪|go west]找索林買把武器，或者去[藥劑店|go south]備些藥水。", "zh-CN": "新手的话，可以先去[铁匠铺|go west]找索林买把武器，或者去[药剂店|go south]备些药水。" ]),
        ([ "en": "Remember, timely retreat is also a part of adventure.", "zh-TW": "記住，適時的撤退也是冒險的一部分。", "zh-CN": "记住，适时的撤退也是冒险的一部分。" ]),
        ([ "en": "Talk more with others, you will reap unexpected information.", "zh-TW": "多和其他人交談，你會收穫意想不到的情報。", "zh-CN": "多和其他人交谈，你会收获意想不到的情报。" ])
    }));
    add_response("newbie", ({
        ([ "en": "This is Newbie Village, the starting point for all great adventures.", "zh-TW": "這裡是新手村，是所有偉大冒險的起點。", "zh-CN": "这里是新手村，是所有伟大冒险的起点。" ]),
        ([ "en": "Don't worry, everyone starts as a newbie.", "zh-TW": "別擔心，每個人都是從新手開始的。", "zh-CN": "别担心，每个人都是从新手开始的。" ]),
        ([ "en": "Make good use of the resources here to improve yourself!", "zh-TW": "好好利用這裡的資源提升自己吧！", "zh-CN": "好好利用这里的资源提升自己吧！" ])
    }));
    add_response("新手", ({
        ([ "en": "This is Newbie Village, the starting point for all great adventures.", "zh-TW": "這裡是新手村，是所有偉大冒險的起點。", "zh-CN": "这里是新手村，是所有伟大冒险的起点。" ]),
        ([ "en": "Don't worry, everyone starts as a newbie.", "zh-TW": "別擔心，每個人都是從新手開始的。", "zh-CN": "别担心，每个人都是从新手开始的。" ]),
        ([ "en": "Make good use of the resources here to improve yourself!", "zh-TW": "好好利用這裡的資源提升自己吧！", "zh-CN": "好好利用这里的资源提升自己吧！" ])
    }));
    add_response("adventure", ({
        ([ "en": "Adventure is always full of surprises and dangers, remember to always be prepared.", "zh-TW": "冒險總是充滿驚喜與危險，記得隨時準備好你的裝備。", "zh-CN": "冒险总是充满惊喜与危险，记得随时准备好你的装备。" ]),
        ([ "en": "True adventurers never fear unknown challenges!", "zh-TW": "真正的冒險家從不畏懼未知的挑戰！", "zh-CN": "真正的冒险家從不畏惧未知的挑战！" ]),
        ([ "en": "I once rolled in a dragon's nest too...", "zh-TW": "想當初，我也曾經在巨龍的巢穴裡打過滾...", "zh-CN": "想当初，我也曾经在巨龙的巢穴里打过滚..." ])
    }));
    add_response("冒險", ({
        ([ "en": "Adventure is always full of surprises and dangers, remember to always be prepared.", "zh-TW": "冒險總是充滿驚喜與危險，記得隨時準備好你的裝備。", "zh-CN": "冒险总是充满惊喜与危险，记得随时准备好你的装备。" ]),
        ([ "en": "True adventurers never fear unknown challenges!", "zh-TW": "真正的冒險家從不畏懼未知的挑戰！", "zh-CN": "真正的冒险家從不畏惧未知的挑战！" ]),
        ([ "en": "I once rolled in a dragon's nest too...", "zh-TW": "想當初，我也曾經在巨龍的巢穴裡打過滾...", "zh-CN": "想当初，我也曾经在巨龙的巢穴里打过滚..." ])
    }));
    add_response("news", ({
        ([ "en": "I heard a group of powerful adventurers just returned from the north with a lot of good things.", "zh-TW": "聽說最近有批厲害的冒險者剛從北方回來，帶回了不少好東西。", "zh-CN": "听说最近有批厉害的冒险者刚从北方回来，带回了不少好东西。" ]),
        ([ "en": "I heard that an unprecedented giant slime appeared in the forest near Newbie Village.", "zh-TW": "聽說新手村附近的森林裡出現了前所未見的巨大史萊姆。", "zh-CN": "听说新手村附近的森林里出现了前所未见的巨大史莱姆。" ]),
        ([ "en": "The tavern is especially crowded lately, probably because the newly arrived ale is very good.", "zh-TW": "最近酒館裡的人特別多，大概是因為新運來的麥酒很好喝吧。", "zh-CN": "最近酒馆里的人特别多，大概是因为新運來的麦酒很好喝吧。" ])
    }));
    add_response("消息", ({
        ([ "en": "I heard a group of powerful adventurers just returned from the north with a lot of good things.", "zh-TW": "聽說最近有批厲害的冒險者剛從北方回來，帶回了不少好東西。", "zh-CN": "听说最近有批厉害的冒险者刚从北方回来，带回了不少好东西。" ]),
        ([ "en": "I heard that an unprecedented giant slime appeared in the forest near Newbie Village.", "zh-TW": "聽說新手村附近的森林裡出現了前所未見的巨大史萊姆。", "zh-CN": "听说新手村附近的森林里出现了前所未见的巨大史莱姆。" ]),
        ([ "en": "The tavern is especially crowded lately, probably because the newly arrived ale is very good.", "zh-TW": "最近酒館裡的人特別多，大概是因為新運來的麥酒很好喝吧。", "zh-CN": "最近酒馆里的人特别多，大概是因为新運來的麦酒很好喝吧。" ])
    }));
    add_response("skill", ({
        ([ "en": "Skills are the key to your survival in the wilderness.", "zh-TW": "技能是你在荒野中生存的關鍵。", "zh-CN": "技能是你在荒野中生存的关键。" ]),
        ([ "en": "Practice more, your skills will become more and more proficient.", "zh-TW": "多加練習，你的技巧會越來越純熟的。", "zh-CN": "多加练习，你的技巧会越来越纯熟的。" ]),
        ([ "en": "Although I mainly teach combat skills, knowledge in other fields is equally important.", "zh-TW": "雖然我主要教授戰鬥技巧，但其他領域的知識也同樣重要。", "zh-CN": "虽然我主要教授战斗技巧，但其他领域的知识也同样重要。" ])
    }));
    add_response("技能", ({
        ([ "en": "Skills are the key to your survival in the wilderness.", "zh-TW": "技能是你在荒野中生存的關鍵。", "zh-CN": "技能是你在荒野中生存的关键。" ]),
        ([ "en": "Practice more, your skills will become more and more proficient.", "zh-TW": "多加練習，你的技巧會越來越純熟的。", "zh-CN": "多加练习，你的技巧会越来越纯熟的。" ]),
        ([ "en": "Although I mainly teach combat skills, knowledge in other fields is equally important.", "zh-TW": "雖然我主要教授戰鬥技巧，但其他領域的知識也同樣重要。", "zh-CN": "虽然我主要教授战斗技巧，但其他领域的知识也同样重要。" ])
    }));
    add_response("地圖", ([ "en": "Newbie Village is composed of an 8x8 area. We are now at (4,5). Central Square is to the south at (4,4).", "zh-TW": "新手村由 8x8 的區域組成。我們現在在 (4,5)。中央廣場在正南方 (4,4)。", "zh-CN": "新手村由 8x8 的区域组成。我们现在在 (4,5)。中央广场在正南方 (4,4)。" ]));

    // 任務互動：使用全新的 Closure 語法，支援型別宣告與多行語句
    add_response(({ "quest", "任務" }), (:
        mapping qdata = this_player()->query_quest("newbie_badge");
        if (qdata) {
            if (qdata["status"] == "active")
                return ([ "en": "You are already performing the 'Newbie Badge' quest, go prove your courage!", "zh-TW": "你已經在進行『新手證明』任務了，快去證明你的勇氣吧！", "zh-CN": "你已经也在进行『新手证明』任务了，快去证明你的勇气吧！" ]);
            return ([ "en": "You have already gained recognition as an adventurer. Now you can go to [Kyle|go west] and see if there are any newbie missions.", "zh-TW": "你已經獲得了冒險者的認可。現在可以去[教官|go west]那裡看看有沒有什麼新兵任務。", "zh-CN": "你已经获得了冒险者的认可。现在可以去[教官|go west]那里看看有没有什么新兵任务。" ]);
        }
        load_object("/secure/quest_d.c")->accept_quest(this_player(), "newbie_badge");
        return ([ "en": "Great! As long as you shout 'I have courage', I will give you this badge.", "zh-TW": "很好，有志氣！只要你大聲喊出『我有勇氣』，我就把這枚徽章送給你。", "zh-CN": "很好，有志气！只要你大声喊出『我有勇气』，我就把這枚徽章送給你。" ]);
    :));

    add_response(({ "report", "回報" }), (:
        mapping qwolf = this_player()->query_quest("wolf_hunter");
        if (qwolf && qwolf["status"] == "active") {
            mapping info = load_object("/secure/quest_d.c")->query_quest_info("wolf_hunter");
            if (qwolf["progress"]["count"] >= info["goal"]["count"]) {
                load_object("/secure/quest_d.c")->complete_quest(this_player(), "wolf_hunter");
                return ([ "en": "Well done! Those annoying wolves are finally subdued. Here is your reward.", "zh-TW": "做得好！那些討厭的野狼終於被制伏了。這是你的獎勵。", "zh-CN": "做得好！那些讨厌的野狼终于被制伏了。这是你的奖励。" ]);
            }
            return ([ "en": "Wolves are still out there, go complete the quest!", "zh-TW": "野狼還在草原上出沒呢，快去完成任務吧！", "zh-CN": "野狼还在草原上出没呢，快去完成任务吧！" ]);
        }
        return ([ "en": "You have nothing to report to me right now.", "zh-TW": "你目前沒有什麼可以向我回報的。", "zh-CN": "你目前没有什么可以向我回报的。" ]);
    :));

    add_response(({ "hunt", "打獵", "獵狼" }), (:
        if (this_player()->query_quest("wolf_hunter")) return ([ "en": "You have already taken the wolf hunting quest.", "zh-TW": "你已經領過獵狼任務了。", "zh-CN": "你已经领过猎狼任务了。" ]);
        load_object("/secure/quest_d.c")->accept_quest(this_player(), "wolf_hunter");
        return ([ "en": "Great, go eliminate 3 hungry wolves and come back to 'report' to me.", "zh-TW": "很好，去消滅 3 隻飢餓的野狼，回來向我『回報』。", "zh-CN": "很好，去消灭 3 只饥饿的野狼，回来向我『回报』。" ]);
    :));

    add_response(({ "join", "加入" }), (:
        string g = this_player()->query_guild();
        if (g) {
            mixed ginfo = load_object("/secure/guild_d.c")->query_guild_info(g);
            string gname = to_string(select_lang(ginfo["name"]));
            return ([ "en": "You are already a member of " + gname + ".", "zh-TW": "你已經是 " + gname + " 的成員了。", "zh-CN": "你已经是 " + gname + " 的成员了。" ]);
        }
        load_object("/secure/guild_d.c")->join_guild(this_player(), "adventurer");
        return ([ "en": "Excellent! From today on, you are a member of our Adventurers Guild.", "zh-TW": "很好！從今天起，你就是我們冒險者公會的一員了。", "zh-CN": "很好！从今天起，你就是我们冒险者公会的一员了。" ]);
    :));

    add_response(({ "wolf", "野狼" }), ([ "en": "Recently the eastern plains are infested with wolves. If you want to practice, you can come to me for the 'wolf hunt' quest.", "zh-TW": "最近東邊的草原野狼氾濫，如果你想練手，可以找我承接『獵狼』任務。", "zh-CN": "最近东边的草原野狼泛滥，如果你想练手，可以找我承接『猎狼』任务。" ]));
}

// 攔截玩家大喊「我有勇氣」
void catch_tell(string msg) {
    ::catch_tell(msg);

    object tp = this_player();
    if (!tp || !userp(tp)) return;

    if (strsrch(msg, "我有勇氣") != -1) {
        mapping qdata = tp->query_quest("newbie_badge");
        if (qdata && qdata["status"] == "active") {
            load_object("/secure/quest_d.c")->complete_quest(tp, "newbie_badge");
            say(query_name() + " 欣慰地點點頭，將一枚徽章遞給了 " + tp->query_name() + "。\n");
        }
    }
}

