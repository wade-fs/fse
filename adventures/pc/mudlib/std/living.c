// /std/living.c  (史前文明 PC)
// 所有生命體（玩家、怪物）的共用基底
// 提供：名稱、HP、攻擊、防禦、戰鬥狀態
#include "/include/ansi.h"
inherit "/std/object";

private string name;         // 顯示名稱
private string short_desc;   // 簡短描述 (look 時用)
private string long_desc;    // 詳細描述 (look <名稱> 時用)

private int hp;
private int max_hp;
private int attack;          // 攻擊力
private int defense;         // 防禦力
private int level;

private object combat_target; // 當前戰鬥對象
private int in_combat;        // 是否在戰鬥中

void create() {
    ::create();
    name      = "未知生物";
    short_desc = "一個未知的生物。";
    long_desc  = "一個未知的生物，你看不出它的特徵。";
    hp        = 10;
    max_hp    = 10;
    attack    = 1;
    defense   = 0;
    level     = 1;
    in_combat = 0;
}

// --- 名稱與描述 ---
void     set_name(string n)        { name = n; }
string   query_name()              { return name; }
void     set_short(string s)       { short_desc = s; }
string   query_short()             { return short_desc; }
void     set_long(string l)        { long_desc = l; }
string   query_long()              { return long_desc; }

// --- 屬性 ---
void set_hp(int v)     { hp = v; }
int  query_hp()        { return hp; }
void set_max_hp(int v) { max_hp = v; hp = v; }
int  query_max_hp()    { return max_hp; }
void add_hp(int val) {
    hp += val;
    if (hp < 0) hp = 0;
    if (hp > max_hp) hp = max_hp;
    // 檢查玩家是否因扣血而死亡
    if (hp == 0 && function_exists("on_death", this_object())) {
        this_object()->on_death("在惡劣的史前環境中力竭身亡");
    }
}
void add_max_hp(int val) {
    max_hp += val;
    if (max_hp < 1) max_hp = 1;
    if (hp > max_hp) hp = max_hp;
}
void set_attack(int v) { attack = v; }
int  query_attack()    { return attack; }
void set_defense(int v){ defense = v; }
int  query_defense()   { return defense; }
void set_level(int v)  { level = v; }
int  query_level()     { return level; }

int  is_alive()        { return hp > 0; }

// --- 戰鬥狀態 ---
object query_combat_target() { return combat_target; }
int    query_in_combat()     { return in_combat; }

// 開始攻擊指定目標
void start_combat(object target) {
    if (!target || !target->is_alive()) return;
    combat_target = target;
    in_combat     = 1;
}

// 停止戰鬥
void stop_combat() {
    combat_target = 0;
    in_combat     = 0;
}

// 受到傷害，回傳實際受到的傷害值
int receive_damage(int dmg) {
    if (hp <= 0) return 0; // 已經死亡，不再受傷
    int actual = dmg - defense;
    if (actual < 1) actual = 1;
    hp -= actual;
    if (hp < 0) hp = 0;
    return actual;
}

// 受到已計算的直接傷害，繞過防禦力減免
int receive_damage_raw(int actual) {
    if (hp <= 0) return 0;
    if (actual < 1) actual = 1;
    hp -= actual;
    if (hp < 0) hp = 0;
    return actual;
}

// 一回合戰鬥結算 (攻擊方呼叫)
varargs void do_attack(object target) {
    if (!is_alive()) {
        stop_combat();
        return;
    }
    if (!target || !target->is_alive()) {
        stop_combat();
        return;
    }
    
    int actual;
    object combat_svc = load_object("/runtime/services/combat_service.c");
    if (combat_svc) {
        int dmg = combat_svc->calculate_damage(this_object(), target);
        actual = target->receive_damage_raw(dmg);
    } else {
        int dmg = attack + random(attack / 2 + 1);
        actual = target->receive_damage(dmg);
    }

    tell_object(this_object(), sprintf(
        YEL + "你攻擊 %s，造成 %d 點傷害。" + NOR + " (對方剩餘 HP: %d/%d)\n",
        target->query_name(), actual, target->query_hp(), target->query_max_hp()));

    if (target->is_alive()) {
        tell_object(target, sprintf(
            RED + "%s 攻擊你，造成 %d 點傷害。" + NOR + " (你的 HP: %d/%d)\n",
            query_name(), actual, target->query_hp(), target->query_max_hp()));
    }
}
