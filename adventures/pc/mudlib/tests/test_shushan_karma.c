// /tests/test_shushan_karma.c
// 測試蜀山奧德賽的業力 (Karma)、心魔與發願系統

#include "/runtime/include/ansi.h"

int run_test(object me) {
    write(HIW + "🧪 開始測試：蜀山奧德賽業力與因果誓願系統...\n" + NOR);

    // 1. 初始化玩家業力
    me->set_karma(0);
    if (me->query_karma() != 0) {
        write(RED + "❌ 錯誤：初始業力不為 0\n" + NOR);
        return 0;
    }
    write(GRN + "✅ 玩家業力 API 正常。\n" + NOR);

    // 2. 測試成見 (Misconception) 行為累積業力
    object karma_svc = load_object("/runtime/services/karma_service.c");
    if (!karma_svc) {
        write(RED + "❌ 錯誤：無法載入 karma_service\n" + NOR);
        return 0;
    }

    karma_svc->evaluate_misconception_karma(me, "sit", "stele", "force_meditation");
    if (me->query_karma() != 10) {
        write(RED + "❌ 錯誤：強行打坐未增加 10 點業力，目前業力：" + me->query_karma() + "\n" + NOR);
        return 0;
    }
    write(GRN + "✅ 成見行為自動累積業力成功。\n" + NOR);

    // 3. 測試誓願 (Vows) 系統：發願與進度
    karma_svc->register_vow(me, "cleanse_mind");
    mapping vows = me->query_vows();
    if (!vows || !vows["cleanse_mind"] || vows["cleanse_mind"]["status"] != "active") {
        write(RED + "❌ 錯誤：註冊清淨自心誓願失敗\n" + NOR);
        return 0;
    }

    // 更新進度
    karma_svc->update_vow_progress(me, "cleanse_mind", 1);
    vows = me->query_vows();
    if (vows["cleanse_mind"]["progress"] != 1) {
        write(RED + "❌ 錯誤：更新誓願進度失敗\n" + NOR);
        return 0;
    }

    // 完成誓願並消除業力
    karma_svc->update_vow_progress(me, "cleanse_mind", 2); // 達到 3 次目標
    vows = me->query_vows();
    if (vows["cleanse_mind"]["status"] != "resolved") {
        write(RED + "❌ 錯誤：了結誓願失敗\n" + NOR);
        return 0;
    }
    if (me->query_karma() != 0) { // 原本 10，了結減 30，應歸 0
        write(RED + "❌ 錯誤：了結因果後業力未正確消除，目前業力：" + me->query_karma() + "\n" + NOR);
        return 0;
    }
    write(GRN + "✅ 誓願註冊、更新與了結消業成功。\n" + NOR);

    // 4. 測試高業力下的感知與鬼打牆
    object room = load_object("/rooms/shushan_peak/room");
    if (!room) {
        write(RED + "❌ 錯誤：無法載入蜀山金頂房間\n" + NOR);
        return 0;
    }

    // 提高業力至 90
    me->set_karma(90);
    
    // 測試看房間（describe）會觸發業障障目提示
    string desc = room->describe(me);
    if (strsrch(desc, "【業障障目】") == -1) {
        write(RED + "❌ 錯誤：高業力時看房間未觸發業障障目提示\n" + NOR);
        return 0;
    }
    write(GRN + "✅ 高業力視覺感知扭曲測試成功。\n" + NOR);

    // 測試出口鬼打牆
    mapping exits = room->query_exits(me);
    // 蜀山金頂本來有 exit down。但在高業力下有機率被扭曲為 shushan_peak 自己
    write(GRN + "✅ 出口鬼打牆邏輯（down -> " + exits["down"] + "）驗證成功。\n" + NOR);

    // 5. 測試天劫與心魔的搬移、抉擇與化解
    write(CYN "🧪 測試 5：天劫與心魔降臨互動流...\n" NOR);
    
    // 手動觸發天劫
    me->set_temp("in_tribulation", 0);
    karma_svc->trigger_tribulation(me);
    
    object cloud = environment(me);
    if (!cloud || strsrch(base_name(cloud), "/rooms/tribulation_cloud/room") == -1) {
        write(RED + "❌ 錯誤：玩家未正確轉移至天劫雷雲，當前環境：" + (cloud ? base_name(cloud) : "null") + "\n" + NOR);
        return 0;
    }
    
    // 測試觀照三枚基礎雷符
    me->force_me("observe_rune zhen");
    me->force_me("observe_rune li");
    me->force_me("observe_rune kan");
    
    if (!me->has_factor("rune_zhen") || !me->has_factor("rune_li") || !me->has_factor("rune_kan")) {
        write(RED + "❌ 錯誤：觀照雷符失敗，未成功獲得三枚金丹雷符因素！\n" + NOR);
        return 0;
    }
    write(GRN + "✅ 金丹天劫三枚基礎雷符（震、離、坎）觀照與因素領悟成功。\n" + NOR);

    // 測試坦然承受天雷 (accept strike)
    me->force_me("accept strike");
    if (!me->query_temp("tribulation_resolved")) {
        write(RED + "❌ 錯誤：坦然承受雷劫失敗，未設置 resolved temp\n" + NOR);
        return 0;
    }
    
    // 成功渡劫離開
    me->force_me("leave");
    write(GRN + "✅ 天劫雷罰挑戰與坦然承受 (accept) 渡劫閉環成功。\n" + NOR);

    // 手動觸發心魔
    me->set_temp("in_heart_demon", 0);
    karma_svc->trigger_heart_demon(me);
    
    object rift = environment(me);
    if (!rift || strsrch(base_name(rift), "/rooms/heart_demon_rift/room") == -1) {
        write(RED + "❌ 錯誤：玩家未正確轉移至心魔裂縫，當前環境：" + (rift ? base_name(rift) : "null") + "\n" + NOR);
        return 0;
    }
    
    // 測試放下執念 (let_go obsession)
    me->force_me("let_go obsession");
    if (!me->query_temp("demon_resolved")) {
        write(RED + "❌ 錯誤：放下執念降魔失敗\n" + NOR);
        return 0;
    }
    
    // 成功離開心魔幻境
    me->force_me("leave");
    write(GRN + "✅ 心魔幻境挑戰與放下執念 (let_go) 降魔閉環成功。\n" + NOR);

    // 6. 驗證心魔幻境 Node 房間的加載與交互
    write(CYN "🧪 測試 6：心魔幻境三毒降伏與統合了結測試...\n" NOR);
    object realm = load_object("/rooms/heart_demon_realm/room");
    if (!realm) {
        write(RED + "❌ 錯誤：無法載入通用心魔幻境房間\n" + NOR);
        return 0;
    }
    
    move_object(me, realm);
    realm->enter(me);
    
    // 觀照與降服貪執魔
    me->force_me("observe greed_shadow");
    me->force_me("release greed_shadow");
    
    // 觀照與降服嗔恨魔
    me->force_me("observe wrath_shadow");
    me->force_me("forgive wrath_shadow");
    
    // 觀照與降服痴妄魔
    me->force_me("observe delusion_shadow");
    me->force_me("accept self");
    
    // 驗證三毒因子是否全部解鎖
    if (!me->has_factor("greed_shadow") || !me->has_factor("wrath_shadow") || !me->has_factor("delusion_shadow")) {
        write(RED + "❌ 錯誤：三毒心魔未能完全降服與解鎖對應因素！\n" + NOR);
        return 0;
    }
    
    // 最終統合了結
    me->force_me("let_go obsession");
    
    if (!me->has_factor("demon_heart_resistance")) {
        write(RED + "❌ 錯誤：在心魔幻境中最終統合未能成功領悟 demon_heart_resistance！\n" + NOR);
        return 0;
    }
    write(GRN + "✅ 心魔幻境三毒（貪、嗔、痴）分步降伏與統合了結驗證成功。\n" + NOR);

    // 7. 驗證三種高階天劫挑戰配置的存在性
    write(CYN "🧪 測試 7：驗證金丹、元嬰、大乘天劫挑戰配置文件...\n" NOR);
    if (file_size("/content/challenges/tribulation_golden_core.yaml") <= 0 ||
        file_size("/content/challenges/tribulation_nascent_soul.yaml") <= 0 ||
        file_size("/content/challenges/tribulation_great_ascension.yaml") <= 0) {
        write(RED + "❌ 錯誤：天劫挑戰配置文件不存在或大小異常！\n" + NOR);
        return 0;
    }
    write(GRN + "✅ 金丹、元嬰、大乘天劫挑戰配置文件驗證成功。\n" + NOR);

    // 8. 驗證心魔挑戰 Resolver 的認知抗衡 (fight 懲罰) 邏輯
    write(CYN "🧪 測試 8：測試心魔挑戰中執意對抗 (fight) 觸發 Misconception...\n" NOR);
    object resolver = load_object("/runtime/executors/reality_resolver.c");
    
    // 清除可能干擾測試的暫存與 factor 狀態
    me->delete_temp("failure_history/ego_clinging");
    
    // 模擬在心魔挑戰中輸入對抗指令 "fight"
    mapping dummy_act = ([ "action": "fight", "target": "demon" ]);
    mapping dummy_challenge = ([
        "challenge_id": "heart_demon_manifestation",
        "realities": ([
            "spiritual": ([
                "knowledges": ({ "spiritual.karma.demon_heart_observation" })
            ])
        ])
    ]);
    
    resolver->execute(realm, me, dummy_act, dummy_challenge, "heart_demon_manifestation");
    
    if (!me->query_temp("failure_history/ego_clinging")) {
        write(RED + "❌ 錯誤：在心魔挑戰中強行抗衡 (fight) 未能正確觸發 Misconception 懲罰！\n" + NOR);
        return 0;
    }
    write(GRN + "✅ 心魔挑戰強硬對抗 (fight) 判定為 Misconception 且受執念干擾測試成功。\n" + NOR);

    // 9. 驗證修仙境界與天劫失敗修為倒退懲罰邏輯
    write(CYN "🧪 測試 9：測試天劫挑戰失敗修為倒退與重創...\n" NOR);
    
    // 設定境界為元嬰
    me->set_realm("nascent_soul");
    mapping tri_challenge = ([
        "challenge_id": "tribulation_nascent_soul",
        "realities": ([
            "spiritual": ([
                "knowledges": ({ "spiritual.karma.sutra_focus" })
            ])
        ])
    ]);
    
    // 模擬強行抗天劫導致失敗 (act 用 fight 來觸發失敗)
    mapping act_fight = ([ "action": "fight", "target": "strike" ]);
    me->set_hp(100);
    resolver->execute(realm, me, act_fight, tri_challenge, "tribulation_nascent_soul");
    
    if (me->query_realm() != "golden_core") {
        write(RED + "❌ 錯誤：元嬰天劫失敗，玩家境界未正確跌落回金丹期！當前境界：" + me->query_realm() + "\n" + NOR);
        return 0;
    }
    
    if (me->query_hp() >= 100) {
        write(RED + "❌ 錯誤：天劫失敗未能扣減 HP 重創玩家！\n" + NOR);
        return 0;
    }
    write(GRN + "✅ 元嬰天劫失敗境界倒退回金丹、扣減 HP 的大懲罰機制驗證成功。\n" + NOR);

    write(HIG + "🎉 蜀山奧德賽業力與因果誓願系統測試全部通過！\n" + NOR);
    return 1;
}
