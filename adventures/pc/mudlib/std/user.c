// /std/user.c  (史前文明 PC)
// 玩家載體：移除 RPG 等級，加入 FSE 狀態與感官感知
#include "/runtime/include/ansi.h"
inherit "/std/living";

private string id;
private string password_hash;
private int fatigue;          // 疲勞值 (Entropy 的一部分)
private int hunger;           // 飢餓度 (0 - 100, 0 = 飽, 100 = 瀕死)
private int thirst;           // 口渴度 (0 - 100, 0 = 飽, 100 = 瀕死)
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
    hunger       = 0;
    thirst       = 0;
    progression  = ([]);
    factors      = ([]);
    set_heart_beat(1); // 啟動心跳機制
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

void remove_factor(string fid) {
    if (!factors) factors = ([]);
    map_delete(factors, fid);
    save_state();
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

// 飢餓與口渴控制
int query_hunger() { return hunger; }
void set_hunger(int val) { hunger = val; if (hunger < 0) hunger = 0; if (hunger > 100) hunger = 100; }
void add_hunger(int val) {
    hunger += val;
    if (hunger < 0) hunger = 0;
    if (hunger > 100) hunger = 100;
}

int query_thirst() { return thirst; }
void set_thirst(int val) { thirst = val; if (thirst < 0) thirst = 0; if (thirst > 100) thirst = 100; }
void add_thirst(int val) {
    thirst += val;
    if (thirst < 0) thirst = 0;
    if (thirst > 100) thirst = 100;
}

// 覆寫生命值變更以實作「新手村不致死保護區」
void add_hp(int val) {
    object room = environment(this_object());
    int target_hp = query_hp() + val;
    
    if (room) {
        string rpath = base_name(room);
        // 新手村 4 個房間：荒原、森林、黑曜石礦脈 (dark_cave 也包含)、捕食者峽谷
        if (strsrch(rpath, "triassic_plains") != -1 ||
            strsrch(rpath, "fern_forest") != -1 ||
            strsrch(rpath, "dark_cave") != -1 ||
            strsrch(rpath, "predator_canyon") != -1) {
            
            if (target_hp <= 0) {
                set_hp(1);
                tell_object(this_object(), YEL + "⚠️ 新手保護機制生效：你在安全區內力竭，生命值降為 1，並未死亡。\n" + NOR);
                return;
            }
        }
    }
    ::add_hp(val);
}

// 生存背景消耗心跳循環 (發呆致死機制)
void heart_beat() {
    object room = environment(this_object());
    if (!room) return;

    string rpath = base_name(room);
    
    // 如果不在新手保護區內，則啟動飢餓與口渴的流逝
    if (strsrch(rpath, "triassic_plains") == -1 &&
        strsrch(rpath, "fern_forest") == -1 &&
        strsrch(rpath, "dark_cave") == -1 &&
        strsrch(rpath, "predator_canyon") == -1) {
        
        int hb_count = query_temp("hb_count") + 1;
        set_temp("hb_count", hb_count);
        
        // 為了測試與體驗，每 15 個心跳（30秒）增加飢餓與口渴
        if (hb_count >= 15) {
            set_temp("hb_count", 0);
            
            // 讀取當前房間的環境消耗加成 (預設為 1.0)
            float hunger_mult = 1.0;
            float thirst_mult = 1.0;
            
            if (function_exists("query_virtual_config", room)) {
                mapping r_cfg = room->query_virtual_config();
                if (r_cfg && r_cfg["environmental_multipliers"]) {
                    mapping mults = r_cfg["environmental_multipliers"];
                    if (!undefinedp(mults["hunger_decay"])) hunger_mult = to_float(mults["hunger_decay"]);
                    if (!undefinedp(mults["thirst_decay"])) thirst_mult = to_float(mults["thirst_decay"]);
                }
            }
            
            add_hunger(to_int(1 * hunger_mult));
            add_thirst(to_int(2 * thirst_mult)); // 口渴基礎速度是飢餓的兩倍
            
            if (hunger >= 80) {
                tell_object(this_object(), RED + "⚠️ 你感到極度飢餓，胃部劇烈抽搐...\n" + NOR);
            }
            if (thirst >= 80) {
                tell_object(this_object(), RED + "⚠️ 你喉嚨乾裂得像火燒，極度渴望水分...\n" + NOR);
            }
            
            // 當達到 100 時扣減 HP
            if (hunger >= 100 || thirst >= 100) {
                tell_object(this_object(), HIR + "⚠️ 你因為極度飢餓與脫水，體力正在急速流失！\n" + NOR);
                add_hp(-10); // 每次扣 10 點 HP
            }
        }
    } else {
        // 在新手保護區內，飢餓度與口渴度不增加
        set_temp("hb_count", 0);
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
    hunger = 0;
    thirst = 0;
    save_state();

    string spawn_room = "/rooms/triassic_plains/room";
    object pm = load_object("/runtime/services/progress_manager.c");
    if (pm) {
        string node = pm->query_spawn_node(this_object(), "main");
        if (node && node != "") {
            spawn_room = node;
        }
    }
    object room = load_object(spawn_room);
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
    // TODO: 留待日後實作「身世」選擇架構 stub
    // 依據玩家當前 Stage 所在的 spawn_node 決定不同的房間起點 (完全資料驅動)
    string spawn_room = "/rooms/triassic_plains/room";
    object pm = load_object("/runtime/services/progress_manager.c");
    if (pm) {
        string node = pm->query_spawn_node(this_object(), "main");
        if (node && node != "") {
            spawn_room = node;
        }
    }
    object room = load_object(spawn_room);
    if (room) {
        move_object(this_object(), room);
        room->enter(this_object());
        
        if (spawn_room == "/rooms/triassic_plains/room") {
            tell_object(this_object(), YEL + "\n💡 你在一片炎熱的荒原甦醒。在這裡，直接觀察 (look) 或許不是最好的主意。\n" +
                "你可以集中注意力去感知環境: focus [smell/sound/wind/ground]\n" + NOR);
        } else {
            tell_object(this_object(), YEL + "\n💡 你在一片安全區外的史前環境中甦醒。注意周遭環境，小心生存！\n" +
                "你可以集中注意力去感知環境: focus [smell/sound/wind/ground]\n" + NOR);
        }
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

    // 支援中文輸入正規化與同義詞對照轉換服務
    object loc_svc = load_object("/runtime/services/localization_service.c");
    if (loc_svc) {
        mapping trans = loc_svc->translate_input(verb, arg);
        if (trans) {
            verb = trans["action"];
            arg  = trans["target"];
        }
    }

    // 支援常用簡短指令別名映射
    if (verb == "l") verb = "look";
    if (verb == "f") verb = "focus";
    if (verb == "i") verb = "inventory";

    // 史前文明廢除傳統 MUD 戰鬥指令
    if (verb == "kill" || verb == "attack") {
        write("在史前文明，赤手空拳去攻擊恐龍無異於自殺。\n");
        return 1;
    }

    string cmd_path = "/cmds/player/" + verb + ".c";
    if (file_size(cmd_path) <= 0) {
        cmd_path = "/cmds/admin/" + verb + ".c";
    }
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
