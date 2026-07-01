// /runtime/services/karma_service.c
// 蜀山奧德賽 (Shushan Odyssey) 業力因果服務
// 負責監控、調配與管理玩家的業力與誓願 (Vows) 狀態

#include "/runtime/include/ansi.h"

inherit "/std/object";

private nosave mapping active_vows; // 記錄全系統已註冊的因果誓願模板

void create() {
    ::create();
    active_vows = ([
        "cleanse_mind": ([
            "name": "清淨自心",
            "desc": "在清淨之地打坐三次，觀照內心執著以消除業力。",
            "target_count": 3
        ]),
        "rescue_soul": ([
            "name": "超渡怨魂",
            "desc": "超渡游蕩在蜀山邊緣的迷途怨魂，化解世間因果。",
            "target_count": 1
        ])
    ]);
}

// 註冊發願
int register_vow(object player, string vow_id) {
    if (!player || !vow_id || !active_vows[vow_id]) return 0;
    
    mapping vows = player->query_vows() || ([]);
    if (vows[vow_id]) {
        tell_object(player, YEL + "你此前已發過此願，執念未了，無需重複發願。\n" + NOR);
        return 0;
    }
    
    vows[vow_id] = ([
        "status": "active",
        "progress": 0,
        "start_time": time()
    ]);
    player->set_vows(vows);
    
    tell_object(player, HIC + "📜 你於天道前立下宏願：【" + active_vows[vow_id]["name"] + "】\n" +
                        "  👉 願景：" + active_vows[vow_id]["desc"] + "\n" + NOR);
    return 1;
}

// 更新發願進度
void update_vow_progress(object player, string vow_id, int amount) {
    if (!player) return;
    mapping vows = player->query_vows();
    if (!vows || !vows[vow_id] || vows[vow_id]["status"] != "active") return;
    
    vows[vow_id]["progress"] += amount;
    int req = active_vows[vow_id]["target_count"] || 1;
    
    if (vows[vow_id]["progress"] >= req) {
        vows[vow_id]["status"] = "resolved";
        player->set_vows(vows);
        
        // 了結因果，大幅消除業力
        int karma_reduction = 30;
        player->add_karma(-karma_reduction);
        
        tell_object(player, HIG + "✨ 【因果了結】 你的發願【" + active_vows[vow_id]["name"] + "】已然圓滿！\n" +
                            "  🧘 心中執著消散，業力隨之消減 " + karma_reduction + " 點。\n" + NOR);
    } else {
        player->set_vows(vows);
        tell_object(player, CYN + "  Your vow progress [" + active_vows[vow_id]["name"] + "]: " + 
                            vows[vow_id]["progress"] + "/" + req + "\n" + NOR);
    }
}

// 根據玩家行為的成見 (Misconception) 判定業力懲罰
int evaluate_misconception_karma(object player, string action, string target, string misconception_id) {
    if (!player) return 0;
    
    int added_karma = 0;
    string msg = "";
    
    if (misconception_id == "ego_delusion") {
        added_karma = 15;
        msg = "你抱持強烈的自我執著強求法道，天道有感，心中業力滋長。";
    } else if (misconception_id == "force_meditation") {
        added_karma = 10;
        msg = "你強行打坐、試圖強求修為，反而落入執念障礙，業力隨之增加。";
    } else {
        added_karma = 5;
        msg = "你的偏執成見引發了因果的波動，業力微微上升。";
    }
    
    player->add_karma(added_karma);
    tell_object(player, YEL + "⚠️ " + msg + " (+" + added_karma + " 業力)\n" + NOR);
    return added_karma;
}

// 🚀 觸發天劫挑戰
void trigger_tribulation(object player) {
    if (!player) return;
    
    // 預防重複觸發
    if (player->query_temp("in_tribulation")) return;
    player->set_temp("in_tribulation", 1);
    
    int karma = player->query_karma();
    string warn_msg = "";
    string strike_msg = "";
    
    if (karma >= 100) {
        // 大乘渡劫期天劫
        warn_msg = "\n⚡ 【 ⚡ 渡劫期天劫前兆 ⚡ 】\n" +
                   "  整個世界都暗了下來。虛空撕裂，九十九道金色雷柱從天而降，天地間只剩雷光與你的心跳聲！\n";
        strike_msg = "  ⚡ 【天劫降臨】 每一道劫雷都直擊你的道心，幻化出你此生所有未了的因果：未報恩怨、塵世執著……\n";
    } else if (karma >= 80) {
        // 元嬰期中階天劫
        warn_msg = "\n⚡ 【 ⚡ 元嬰期天劫前兆 ⚡ 】\n" +
                   "  九天之上風雲驟變，隱隱有九道雷龍盤旋咆哮。你體內元嬰劇烈顫抖，仿佛元神即將被撕裂！\n";
        strike_msg = "  ⚡ 【天劫降臨】 九道不同顏色的天雷接連落下，每一道都化作你內心最深的執念幻象向你撲來！\n";
    } else {
        // 金丹期初階天劫
        warn_msg = "\n⚡ 【 ⚡ 金丹期天劫前兆 ⚡ 】\n" +
                   "  天色忽然暗沉，遠處傳來低沉雷鳴。你感到元神一陣刺痛，彷彿有無形鎖鏈正拉扯你的魂魄。\n";
        strike_msg = "  ⚡ 【天劫降臨】 一道紫色雷光撕裂夜空直劈而下！雷光中隱隱映照出你過往強求執著的畫面！\n";
    }
    
    tell_object(player, HIR + warn_msg + strike_msg + NOR + "\n");
                        
    object env = environment(player);
    player->set_temp("pre_tribulation_room", env ? base_name(env) : "/rooms/triassic_plains/room");
    
    // 搬移到天劫雷雲地標
    object cloud = load_object("/rooms/tribulation_cloud/room");
    if (cloud) {
        move_object(player, cloud);
        cloud->enter(player);
        player->force_me("look");
    }
}

// 🚀 觸發心魔試煉
void trigger_heart_demon(object player) {
    if (!player) return;
    
    if (player->query_temp("in_heart_demon")) return;
    player->set_temp("in_heart_demon", 1);
    
    tell_object(player, HIM + "\n🌀 【 🌀 心魔低語 🌀 】\n" +
                        "  你四周的虛空開始扭曲變黑，心底深處浮現無數往昔執念的畫面：\n" +
                        "  強求修行、貪戀金銀、同門仇怨……執念具現為一尊漆黑的虛影阻擋在身前！\n\n" + NOR);
                        
    object env = environment(player);
    player->set_temp("pre_demon_room", env ? base_name(env) : "/rooms/triassic_plains/room");
    
    // 搬移到心魔幻境
    object rift = load_object("/rooms/heart_demon_rift/room");
    if (rift) {
        move_object(player, rift);
        rift->enter(player);
        player->force_me("look");
    }
}
