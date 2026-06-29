// /std/monster.c  (史前文明 PC)
// 怪物基底：繼承 living，加入 AI 戰鬥回合與死亡/重生處理
#include "/include/ansi.h"
inherit "/std/living";
inherit "/runtime/core/virtual_object";

private int exp_value;         // 擊殺獲得的經驗值
private string respawn_room;   // 重生後回到的房間路徑
private int respawn_delay;     // 重生延遲（秒）
private int combat_interval;   // 戰鬥回合間隔（秒）

void create() {
    ::create();
    exp_value       = 10;
    respawn_room    = "";
    respawn_delay   = 30;
    combat_interval = 2;
    
    // 初始化虛擬路徑與 YAML 設定
    setup_virtual("monsters", "monster.yaml");
    
    mapping data = query_virtual_config();
    if (data) {
        if (data["name"]) set_name(data["name"]);
        if (data["max_hp"]) set_max_hp(data["max_hp"]);
        if (data["attack"]) set_attack(data["attack"]);
        if (data["defense"]) set_defense(data["defense"]);
        if (data["level"]) set_level(data["level"]);
        if (data["exp_value"]) set_exp_value(data["exp_value"]);
        if (data["respawn_delay"]) set_respawn_delay(data["respawn_delay"]);
        if (data["combat_interval"]) set_combat_interval(data["combat_interval"]);
        if (data["short_desc"]) set_short(data["short_desc"]);
        if (data["long_desc"]) set_long(data["long_desc"]);
    }
}

void set_exp_value(int v)       { exp_value = v; }
int  query_exp_value()          { return exp_value; }
void set_respawn_room(string r) { respawn_room = r; }
string query_respawn_room()     { return respawn_room; }
void set_respawn_delay(int d)   { respawn_delay = d; }
void set_combat_interval(int i) { combat_interval = i; }



// 怪物 AI 戰鬥心跳（每回合）
void combat_round() {
    if (!query_in_combat()) return;
    object target = query_combat_target();
    if (!target || !objectp(target) || !target->is_alive()) {
        stop_combat();
        return;
    }
    // 怪物攻擊玩家
    do_attack(target);

    // 若玩家死亡
    if (!target->is_alive()) {
        stop_combat();
        object room = environment(this_object());
        if (room) {
            foreach (object ob in room->query_occupants()) {
                tell_object(ob, RED + query_name() + " 擊倒了 " + target->query_name() + "！\n" + NOR);
            }
        }
        target->on_death(this_object());
        return;
    }
    // 繼續下一回合
    call_out("combat_round", combat_interval);
}

// 被玩家開始戰鬥
void engage(object attacker) {
    start_combat(attacker);
    // 啟動 AI 回合
    call_out("combat_round", combat_interval);
}

// 怪物死亡處理
void on_death(object killer) {
    stop_combat();
    object room = environment(this_object());
    if (room) {
        foreach (object ob in room->query_occupants()) {
            tell_object(ob, HIG + query_name() + " 倒下了！\n" + NOR);
        }
        room->leave(this_object());
    }
    // 通知殺手
    if (killer && objectp(killer)) {
        killer->gain_exp(exp_value, this_object());
    }
    // 排程重生
    if (respawn_room != "") {
        call_out("respawn", respawn_delay);
    }
}

// 怪物重生
void respawn() {
    // 重置 HP
    set_hp(query_max_hp());
    // 回到房間
    object room = load_object(respawn_room);
    if (room) {
        move_object(this_object(), room);
        room->enter(this_object());
        foreach (object ob in room->query_occupants()) {
            if (ob != this_object()) {
                tell_object(ob, MAG + "一隻 " + query_name() + " 出現了！\n" + NOR);
            }
        }
    }
}
