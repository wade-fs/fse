// /std/user.c  (史前文明 PC)
// 玩家載體：繼承 living，加入登入、經驗值、升級、存檔、指令分派
#include "/include/ansi.h"
inherit "/std/living";

private string id;
private string password_hash;
private int exp;              // 當前經驗值
private int exp_to_next;      // 升下一級所需經驗值
private mapping progression;  // 進度 (同步給 progress_manager)

void create() {
    ::create();
    set_name("冒險者");
    set_max_hp(50);
    set_attack(5);
    set_defense(2);
    set_level(0);   // 初始 Level 0，打過一隻後升到 1
    id           = "";
    password_hash = "";
    exp          = 0;
    exp_to_next  = 20;
    progression  = ([]);
}

// --- 帳號 ---
void   set_id(string s)  { id = s; }
string get_id()          { return id; }

void save_state() {
    if (!id || id == "") return;
    if (file_size("/data/state/players/") < 0) mkdir("/data/state/players/");
    save_object("/data/state/players/" + id);
}

void restore_state() {
    if (!id || id == "") return;
    if (file_size("/data/state/players/" + id + ".o") > 0) {
        restore_object("/data/state/players/" + id);
    }
    if (!progression) progression = ([]);
}

// --- 進度 (供 progress_manager 讀寫) ---
mapping query_progression()   { if (!progression) progression = ([]); return progression; }
void    set_progression(mapping p) { progression = p; save_state(); }

// --- 經驗值與升級 ---
varargs void gain_exp(int amount, object from_monster) {
    exp += amount;
    string from_name = from_monster ? from_monster->query_name() : "未知";
    tell_object(this_object(), sprintf(
        HIG + "✦ 你獲得了 %d 點經驗值（來自 %s）。" + NOR +
        " 當前經驗：%d / %d\n", amount, from_name, exp, exp_to_next));

    // 使用 progress_manager 完成挑戰並檢查升階
    object pm = load_object("/runtime/services/progress_manager.c");
    if (pm) {
        // 升到1級需完成 "first_kill" 任務
        if (query_level() < 1 && from_monster) {
            pm->complete_player_quest(this_object(), "first_kill", "main");
        }
    }

    // 簡單 EXP 升級
    while (exp >= exp_to_next) {
        exp -= exp_to_next;
        int new_level = query_level() + 1;
        set_level(new_level);
        exp_to_next = exp_to_next * 2;  // 每級翻倍
        int new_max_hp = query_max_hp() + 10;
        set_max_hp(new_max_hp);
        set_hp(new_max_hp);  // 升級時補滿 HP
        set_attack(query_attack() + 2);
        set_defense(query_defense() + 1);
        tell_object(this_object(), sprintf(
            HIG + BOLD_YEL + "\n★ 恭喜！你升到了第 %d 級！\n" + NOR +
            "  HP: %d  攻擊: %d  防禦: %d\n\n",
            new_level, new_max_hp, query_attack(), query_defense()));
    }
    save_state();
}

// 玩家死亡
void on_death(object killer) {
    stop_combat();
    tell_object(this_object(), RED +
        "\n你被 " + (killer ? killer->query_name() : "未知") +
        " 殺死了！\n回到起點重新來過...\n" + NOR);
    // 復活：回到起點，HP 恢復一半
    set_hp(query_max_hp() / 2);
    save_state();
    // 回到初始房間
    object room = load_object("/rooms/triassic_plains.c");
    if (room) room->enter(this_object());
}

// --- 登入流程 ---
void get_account(string acc);
void check_password(string pwd);
void new_password(string pwd);

void logon() {
    write("歡迎來到【史前文明】三疊紀求生冒險！\n");
    write("請輸入帳號: ");
    input_to("get_account");
}

void get_account(string acc) {
    acc = trim(acc);
    if (!acc || acc == "") { write("請輸入帳號: "); input_to("get_account"); return; }
    set_id(acc);
    if (file_size("/data/state/players/" + acc + ".o") > 0) {
        restore_state();
        write("請輸入密碼: ");
        input_to("check_password");
    } else {
        write("新帳號，請設定密碼: ");
        input_to("new_password");
    }
}

void check_password(string pwd) {
    pwd = trim(pwd);
    if (pwd != password_hash) {
        write("密碼錯誤！請重新輸入: ");
        input_to("check_password");
        return;
    }
    set_living_name(get_id());
    write(HIG + "登入成功！歡迎回來，" + get_id() + "！\n" + NOR);
    _enter_world();
}

void new_password(string pwd) {
    pwd = trim(pwd);
    if (!pwd || pwd == "") { write("密碼不能為空: "); input_to("new_password"); return; }
    password_hash = pwd;
    save_state();
    set_living_name(get_id());

    // 新玩家：由 progress_manager 設定初始 stage
    object pm = load_object("/runtime/services/progress_manager.c");
    if (pm) pm->set_initial_stage(this_object(), "novice", "main");

    write(HIG + "註冊成功！歡迎加入，" + get_id() + "！\n" + NOR);
    _enter_world();
}

// 進入世界
void _enter_world() {
    object room = load_object("/rooms/triassic_plains.c");
    if (room) {
        room->enter(this_object());
        tell_object(this_object(), room->describe(this_object()));
    }
}

// --- 指令分派 ---
string query_role()          { return "god"; }
string *query_write_paths()  { return ({ "/" }); }

mixed process_input(string cmd) {
    cmd = trim(cmd);
    if (!cmd || cmd == "") return 0;

    string verb, arg;
    if (sscanf(cmd, "%s %s", verb, arg) != 2) {
        verb = cmd;
        arg  = "";
    }

    object cmd_ob = load_object("/cmds/" + verb + ".c");
    if (cmd_ob) {
        cmd_ob->main(this_object(), arg);
        return 1;
    }
    write("什麼？(" + verb + ")\n");
    return 0;
}

int force_me(string cmd) { return process_input(cmd); }
