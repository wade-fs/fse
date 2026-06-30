// /tests/test_combat_classes.c
inherit "/std/test_case";

void run_tests(object me) {
    start_test("戰鬥體系與職業分化驗證");

    object me = clone_object("/std/user.c");
    me->set_id("tester");
    object target = clone_object("/npc/slime.c");
    
    me->set_name("測試鬥士");
    me->set_guild("fighter");
    me->set_level(10);
    me->set_skill("bash", 10); // 學會重擊
    me->set_hp(100);
    me->set_mp(50);
    
    object room = load_object("/area/newbie/room_4_4.c");
    move_object(me, room);
    move_object(target, room);

    object combat_d = load_object("/daemon/combat_d.c");

    // 1. 測試職業招式施展 (Bash)
    int res = combat_d->do_perform(me, "bash", target->query_key_id());
    assert_equal(1, res, "鬥士應該能成功施展 Bash");
    assert_true(target->query_hp() < target->query_max_hp(), "目標應該受到傷害");
    assert_equal(35, me->query_mp(), "法力應該正確扣除 (50 - 15)");

    // 2. 測試職業限制
    me->set_guild("mage");
    res = combat_d->do_perform(me, "bash", target->query_key_id());
    assert_equal(0, res, "法師不應該能施展鬥士招式");

    // 3. 測試法師招式 (Fireball)
    me->set_hp(100);
    me->set_mp(100);
    me->set_stat("int", 20);
    me->set_skill("fireball", 10);
    res = combat_d->do_perform(me, "fireball", target->query_key_id());
    assert_equal(1, res, "法師應該能成功施展 Fireball");

    report_results();
    destruct(me);
    destruct(target);
}
