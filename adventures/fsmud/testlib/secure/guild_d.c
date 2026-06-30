// /secure/guild_d.c
// 公會守護進程 (Guild Daemon)

#include "/include/ansi.h"

inherit "/std/object";

mapping guilds;

void create() {
    ::create();
    guilds = ([
        "adventurer": ([
            "name": ([ "en": "Adventurers Guild", "zh-TW": "冒險者公會", "zh-CN": "冒险者公会" ]),
            "desc": ([ "en": "Welcome all adventurers, this is your first home.", "zh-TW": "歡迎所有志在四方的冒險者，這裡是你的第一個家。", "zh-CN": "欢迎所有志在四方的冒险者，这里是你的第一个家。" ]),
            "ranks": ({ 
                ([ "en": "Novice Adventurer", "zh-TW": "新手冒險者", "zh-CN": "新手冒险者" ]),
                ([ "en": "Official Adventurer", "zh-TW": "正式冒險者", "zh-CN": "正式冒险者" ]),
                ([ "en": "Senior Adventurer", "zh-TW": "資深冒險者", "zh-CN": "资深冒险者" ]),
                ([ "en": "Legendary Adventurer", "zh-TW": "傳奇冒險者", "zh-CN": "传奇冒险者" ])
            }),
            "req": ([ "level": 1, "int": 10, "str": 10 ])
        ]),
        "mage": ([
            "name": ([ "en": "Mages Circle", "zh-TW": "魔法師集會", "zh-CN": "魔法师集会" ]),
            "desc": ([ "en": "The hall for pursuing truth and arcane power.", "zh-TW": "追求真理與奧法力量的殿堂。", "zh-CN": "追求真理与奥法力量的殿堂。" ]),
            "ranks": ({ 
                ([ "en": "Apprentice", "zh-TW": "魔法學徒", "zh-CN": "魔法学徒" ]),
                ([ "en": "Junior Mage", "zh-TW": "初級法師", "zh-CN": "初级法师" ]),
                ([ "en": "Archmage", "zh-TW": "大法師", "zh-CN": "大法师" ]),
                ([ "en": "Sage", "zh-TW": "賢者", "zh-CN": "贤者" ])
            }),
            "req": ([ "level": 5, "int": 20 ])
        ]),
        "fighter": ([
            "name": ([ "en": "Warriors Covenant", "zh-TW": "戰士盟約", "zh-CN": "战士盟约" ]),
            "desc": ([ "en": "The home of strength and glory, hone your iron will.", "zh-TW": "力量與榮耀的歸宿，磨練鋼鐵般的意志。", "zh-CN": "力量与荣耀的归宿，磨练钢铁般的意志。" ]),
            "ranks": ({ 
                ([ "en": "Trainee Fighter", "zh-TW": "見習鬥士", "zh-CN": "见习斗士" ]),
                ([ "en": "Brave Warrior", "zh-TW": "勇猛戰士", "zh-CN": "勇猛战士" ]),
                ([ "en": "Battle Commander", "zh-TW": "戰場統帥", "zh-CN": "战场统帅" ]),
                ([ "en": "War God", "zh-TW": "戰神", "zh-CN": "战神" ])
            }),
            "req": ([ "level": 5, "str": 20 ])
        ])
    ]);
}

mapping query_guild_info(string gid) { return guilds[gid]; }

int join_guild(object me, string gid) {
    mapping info = guilds[gid];
    if (!info) return 0;

    if (me->query_guild()) {
        if (me->query_guild() == gid) return 1; // 🚀 靜默處理：如果已經在該公會，直接回傳成功
        write(_t("guild_already_joined") + "\n");
        return 0;
    }

    mapping req = info["req"];
    string guild_name = to_string(select_lang(info["name"]));

    if (me->query_level() < req["level"]) {
        string err = to_string(_t("level_low_err"));
        err = replace_string(err, "$guild", guild_name);
        write(err + "\n");
        return 0;
    }
    if (req["int"] && me->query_stat("int") < req["int"]) {
        string err = to_string(_t("stat_low_err"));
        err = replace_string(err, "$stat", to_string(_t("stat_int")));
        err = replace_string(err, "$guild", guild_name);
        write(err + "\n");
        return 0;
    }
    if (req["str"] && me->query_stat("str") < req["str"]) {
        string err = to_string(_t("stat_low_err"));
        err = replace_string(err, "$stat", to_string(_t("stat_str")));
        err = replace_string(err, "$guild", guild_name);
        write(err + "\n");
        return 0;
    }

    me->set_guild(gid);
    me->set_guild_rank(to_string(select_lang(info["ranks"][0])));
    me->set_guild_exp(0);

    string success = to_string(_t("guild_join_success"));
    success = replace_string(success, "$guild", guild_name);
    write("$HIW$" + success + "$NOR$" + "\n");

    string rank_msg = to_string(_t("guild_rank_is"));
    rank_msg = replace_string(rank_msg, "$rank", to_string(me->query_guild_rank()));
    write(rank_msg + "\n");
    
    me->save();
    return 1;
}

int promote_member(object me) {
    string gid = me->query_guild();
    if (!gid) return 0;
    
    mapping info = guilds[gid];
    mixed *ranks = info["ranks"];
    
    // 找出目前職位在數組中的索引
    int current_idx = -1;
    for (int i = 0; i < sizeof(ranks); i++) {
        if (select_lang(ranks[i]) == me->query_guild_rank()) {
            current_idx = i;
            break;
        }
    }

    if (current_idx == -1 || current_idx >= sizeof(ranks) - 1) {
        write(_t("guild_max_rank") + "\n");
        return 0;
    }

    int req_exp = (current_idx + 1) * 1000;
    if (me->query_guild_exp() < req_exp) {
        string err = _t("guild_exp_low");
        err = replace_string(err, "$req", sprintf("%d", req_exp));
        write(err + "\n");
        return 0;
    }

    string next_rank = select_lang(ranks[current_idx + 1]);
    me->set_guild_rank(next_rank);
    
    string success = _t("guild_promote_success");
    success = replace_string(success, "$rank", next_rank);
    write("$HIY$" + success + "$NOR$" + "\n");
    
    me->save();
    return 1;
}

mapping query_all_guilds() { return guilds; }
