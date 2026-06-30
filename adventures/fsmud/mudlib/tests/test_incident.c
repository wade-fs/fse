// /tests/test_incident.c
inherit "/std/test_case.c";
#include "/include/formosa.h"
string query_role() { return "god"; }

void run_tests(object caller) {
    start_test("事件調查系統 (Incident System) 測試");
    
    string old_era = TIMELINE_D->query_current_era();
    TIMELINE_D->set_current_era("v1_0");
    
    object me = clone_object("/std/user.c");
    me->set("id", "tester");
    me->set("name", "Tester");
    
    object incident_d = find_object("/daemon/incident_d") || load_object("/daemon/incident_d.c");
    
    // 測試 1: 事件載入
    mapping incs = incident_d->query_incidents();
    assert_true(sizeof(incs) > 0, "成功載入 incident");
    
    // 測試 2: 觸發線索
    incident_d->check_trigger(me, "quest", "sugar_railway_repair", nil);
    mapping clues = me->query("investigation_clues");
    assert_true(clues && clues["derailment_report"] != nil, "成功透過 quest trigger 獲取線索");
    
    // 測試 3: 收集全部線索並結案
    incident_d->check_trigger(me, "site_look", "site:minxiong_sugarcane_field", nil);
    incident_d->check_trigger(me, "npc_ask", "npc:old_stationmaster", "糖廠秘密");
    incident_d->check_trigger(me, "memory", "memory:record_extra_pos_75", nil);
    
    clues = me->query("investigation_clues");
    assert_true(sizeof(clues) >= 4, "成功收集所有線索");
    
    // 測試 4: 結案
    int res = incident_d->resolve_incident(me, "minxiong_sugar_incident");
    assert_true(res != 0, "成功結案推演");
    
    mapping resolved = me->query("resolved_incidents");
    assert_true(resolved && resolved["minxiong_sugar_incident"] != nil, "結案標記已存入");
    
    if(old_era) {
        TIMELINE_D->set_current_era(old_era);
    }
    
    report_results();
}
