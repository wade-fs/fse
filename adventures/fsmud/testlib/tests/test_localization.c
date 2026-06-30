#include "/include/ansi.h"
inherit "/std/test_case.c";

void run_tests(object me) {
    start_test("廣播訊息本地化驗證 (NPC 姓名)");

    object room = load_object("/area/newbie/room_4_4.c");
    object player_tw = clone_object("/std/user.c");
    object player_en = clone_object("/std/user.c");
    object npc = clone_object("/npc/guard.c");

    player_tw->set_id("tester_tw");
    player_tw->set_lang("zh-TW");
    player_tw->enable_commands();
    move_object(player_tw, room);

    player_en->set_id("tester_en");
    player_en->set_lang("en");
    player_en->enable_commands();
    move_object(player_en, room);

    // 我們無法直接抓取 tell_object 的輸出，但我們可以檢查 broadcast_event 的邏輯
    // 透過呼叫 language_d 直接測試其字串處理
    object lang_d = load_object("/secure/language_d.c");
    
    // 我們需要模擬 broadcast_event 的內部邏輯，因為它直接 tell_object
    // 這裡我們手動驗證 query_localized_name
    
    string npc_name_tw = npc->query_localized_name("zh-TW");
    string npc_name_en = npc->query_localized_name("en");

    assert_equal("守衛 葛羅格", npc_name_tw, "繁中玩家應看到中文名字");
    assert_equal("Orc Guard Grog", npc_name_en, "英文玩家應看到英文名字");

    // 清理
    destruct(player_tw);
    destruct(player_en);
    destruct(npc);

    report_results();
}
