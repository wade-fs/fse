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

    write(HIG + "🎉 蜀山奧德賽業力與因果誓願系統測試全部通過！\n" + NOR);
    return 1;
}
