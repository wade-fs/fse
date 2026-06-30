// /std/user.c  (史前文明 PC)
// 玩家載體：移除 RPG 等級，加入 FSE 狀態與感官感知
#include "/include/ansi.h"
inherit "/std/living";

private string id;
private string password_hash;
private int fatigue;          // 疲勞值 (Entropy 的一部分)
private mapping progression;  // 進度 (同步給 progress_manager)
private mapping factors;      // 已解鎖因素 (供 factor_service 讀寫)

void create() {
    ::create();
    set_name("穿越者");
    set_max_hp(100);
    set_hp(100);
    set_level(1);
    id           = "";
    password_hash = "";
    fatigue      = 0;
    progression  = ([]);
    factors      = ([]);
}

// --- 帳號 ---
void   set_id(string s)  { id = s; }
string get_id()          { return id; }
string query_entity_id() { return id ? "user:" + id : "user:unknown"; }

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

// --- 因素探索 (供 factor_service 讀寫) ---
int has_factor(string fid) {
    if (!factors) factors = ([]);
    return !undefinedp(factors[fid]);
}
void discover_factor(string fid, mapping metadata) {
    if (!factors) factors = ([]);
    factors[fid] = metadata;
    save_state();

    // 🌟 FSE 核心機制：自動檢測當前房間是否因為這個 factor 的解鎖而產生了新 Reveal 出口
    object room = environment(this_object());
    if (room && function_exists("check_new_reveals", room)) {
        string *reveals = room->check_new_reveals(this_object(), fid);
        if (sizeof(reveals) > 0) {
            foreach (string msg in reveals) {
                tell_object(this_object(), GRN + "\n【 🌲 顯現 (Reveal) 】" + msg + "\n" + NOR);
            }
        }
    }
}

// 疲勞與 Entropy 控制
int query_fatigue() { return fatigue; }
void add_fatigue(int val) { 
    fatigue += val; 
    if (fatigue < 0) fatigue = 0;
    if (fatigue > 100) {
        tell_object(this_object(), RED + "⚠️ 你感到極度疲憊，精神恍惚...\n" + NOR);
    }
}

// 玩家死亡
void on_death(string reason) {
    stop_combat();
    tell_object(this_object(), RED +
        "\n【 💀 死亡 】" + reason + "\n回到起點重新探索...\n" + NOR);
    
    // 扣減狀態，並標記 Confusion
    player_confused("death_by_predator");
    set_hp(query_max_hp());
    fatigue = 0;
    save_state();

    object room = load_object("/rooms/triassic_plains/room");
    if (room) {
        move_object(this_object(), room);
    }
}


// 標記困惑狀態 (FSE)
void player_confused(string challenge_id) {
    set_temp("is_confused", 1);
    object event_bus = load_object("/runtime/services/event_bus.c");
    if (event_bus) {
        event_bus->publish("PlayerConfused", ([
            "player": this_object(),
            "challenge_id": challenge_id,
            "node_id": environment(this_object()) ? environment(this_object())->query_short() : "triassic"
        ]));
    }
}

// --- 登入流程 ---
void get_account(string acc);
void check_password(string pwd);
void new_password(string pwd);

void logon() {
    write("==================================================\n");
    write("          歡迎來到 FSE【史前文明】生態求生！\n");
    write("     這不是一個打怪升級的遊戲。請注意感官，學習生存規則。\n");
    write("==================================================\n");
    write("請輸入玩家代號: ");
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
        write("新探索者，請設定密碼: ");
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
    write(HIG + "登入成功！世界已載入。\n" + NOR);
    _enter_world();
}

void new_password(string pwd) {
    pwd = trim(pwd);
    if (!pwd || pwd == "") { write("密碼不能為空: "); input_to("new_password"); return; }
    password_hash = pwd;
    save_state();
    set_living_name(get_id());

    object pm = load_object("/runtime/services/progress_manager.c");
    if (pm) pm->set_initial_stage(this_object(), "novice", "main");

    write(HIG + "註冊成功！世界已載入。\n" + NOR);
    _enter_world();
}

void _enter_world() {
    object room = load_object("/rooms/triassic_plains/room");
    if (room) {
        move_object(this_object(), room);
        room->enter(this_object());
        // 初始提示
        tell_object(this_object(), YEL + "\n💡 你在一片炎熱的荒原甦醒。在這裡，直接觀察 (look) 或許不是最好的主意。\n" +
            "你可以集中注意力去感知環境: focus [smell/sound/wind/ground]\n" + NOR);
    }
}


// --- 指令分派 ---
string query_role()          { return "player"; }
string *query_write_paths()  { return ({ "/" }); }

mixed process_input(string cmd) {
    cmd = trim(cmd);
    if (!cmd || cmd == "") return 0;

    string verb, arg;
    if (sscanf(cmd, "%s %s", verb, arg) != 2) {
        verb = cmd;
        arg  = "";
    }

    // 支援常用簡短指令別名映射
    if (verb == "l") verb = "look";
    if (verb == "f") verb = "focus";

    // 史前文明廢除傳統 MUD 戰鬥指令
    if (verb == "kill" || verb == "attack") {
        write("在史前文明，赤手空拳去攻擊恐龍無異於自殺。\n");
        return 1;
    }

    string cmd_path = "/cmds/" + verb + ".c";
    if (file_size(cmd_path) > 0) {
        object cmd_ob = load_object(cmd_path);
        if (cmd_ob) {
            cmd_ob->main(this_object(), arg);
            return 1;
        }
    }

    // 🌟 FSE 核心萬用實體互動處理：
    // 若找不到專屬的指令程式檔，則嘗試向當前房間詢問是否有宣告此實體互動動作 (例如: kick rocks / lick water)
    object room = environment(this_object());
    if (room && function_exists("resolve_interaction", room)) {
        if (room->resolve_interaction(this_object(), verb, arg)) {
            return 1;
        }
    }

    write("你感到迷茫，不知道如何做出這個動作。(" + verb + ")\n");
    return 0;
}

int force_me(string cmd) { return process_input(cmd); }
