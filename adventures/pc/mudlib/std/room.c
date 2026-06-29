// /std/room.c  (史前文明 PC)
// 房間基底：描述、出口、物件與生物容器
#include "/include/ansi.h"
inherit "/std/object";

private string short_desc;
private string long_desc;
private mapping exits;      // 方向 -> 目標房間路徑
private object *occupants;  // 所有在此房間的物件 (玩家/怪物)
private nosave mapping monster_configs;

void create() {
    ::create();
    short_desc = "一個地點";
    long_desc  = "這是一個地點。";
    exits      = ([]);
    occupants  = ({});
}

void set_short(string s)  { short_desc = s; }
string query_short()      { return short_desc; }
void set_long(string l)   { long_desc = l; }
string query_long()       { return long_desc; }

void spawn_monsters_from_config() {
    if (!monster_configs) return;
    foreach (string mid, int count in monster_configs) {
        for (int i = 0; i < count; i++) {
            object ob = clone_object(mid);
            if (ob) {
                ob->set_respawn_room(base_name(this_object()));
                move_object(ob, this_object());
                enter(ob);
            }
        }
    }
}

// 從 YAML 定義動態載入房間屬性與怪物配置 (資料驅動)
void initialize_from_yaml(string yaml_path) {
    if (file_size(yaml_path) <= 0) return;
    string raw = read_file(yaml_path);
    if (!raw) return;
    mapping data = yaml_decode(raw);
    if (!data) return;

    if (data["short"]) set_short(data["short"]);
    if (data["long"]) set_long(data["long"]);

    // 解析出口
    mapping ex = data["exits"];
    if (ex) {
        foreach (string dir, string dest in ex) {
            add_exit(dir, dest);
        }
    }

    // 解析怪物配置
    mixed monsters = data["monsters"];
    if (arrayp(monsters) && sizeof(monsters) > 0) {
        monster_configs = ([]);
        foreach (mapping m in monsters) {
            monster_configs[m["id"]] = m["count"];
        }
        // 延遲在當前 tick 結束後執行，避免 create 期間移動物件造成的時序問題
        call_out("spawn_monsters_from_config", 0);
    }
}

void add_exit(string dir, string dest) {
    exits[dir] = dest;
}
mapping query_exits() { return exits; }

// 物件進入/離開房間 (由 move_object 底層呼叫，或手動維護)
void enter(object ob) {
    if (member_array(ob, occupants) == -1) {
        occupants += ({ ob });
    }
}

void leave(object ob) {
    occupants -= ({ ob });
}

object *query_occupants() {
    // 過濾掉已被 destruct 的物件
    occupants = filter(occupants, (: objectp($1) :));
    return occupants;
}

// 回傳描述（包含出口與生物清單），供 look 指令使用
string describe(object looker) {
    string result = HIG + short_desc + NOR + "\n";
    result += long_desc + "\n";

    // 出口
    string *dirs = keys(exits);
    if (sizeof(dirs) > 0) {
        result += GRN + "出口：" + NOR + implode(dirs, "  ") + "\n";
    }

    // 在場生物
    foreach (object ob in query_occupants()) {
        if (ob == looker) continue;
        if (function_exists("query_short", ob)) {
            result += CYN + ob->query_name() + NOR + " 在這裡。\n";
        }
    }
    return result;
}
