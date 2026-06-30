// /std/presence.c  (史前文明 PC)
// 環境偵測與交互實體 (Presence) 基底
#include "/runtime/include/ansi.h"
inherit "/std/living";
inherit "/runtime/core/virtual_object";

private string presence_id;
private string short_desc;
private string long_desc;
private string respawn_room;
private int respawn_delay;
private string *detection_factors;
private mapping failure_consequence;
private string *identifiable_by;
private string unknown_name;

void create() {
    ::create();
    respawn_room = "";
    respawn_delay = 30;
    detection_factors = ({});
    failure_consequence = ([]);
    identifiable_by = ({});
    unknown_name = "一個隱約的生物身影";
    
    // 初始化虛擬路徑與 YAML 設定
    setup_virtual("presence", "presence.yaml");
    
    mapping data = query_virtual_config();
    if (data) {
        if (data["presence_id"]) presence_id = data["presence_id"];
        if (data["name"]) set_name(data["name"]);
        if (data["short_desc"]) set_short(data["short_desc"]);
        if (data["long_desc"]) set_long(data["long_desc"]);
        if (data["detection_factors"]) detection_factors = data["detection_factors"];
        if (data["failure_consequence"]) failure_consequence = data["failure_consequence"];
        if (data["respawn_delay"]) respawn_delay = data["respawn_delay"];
        if (data["identifiable_by"]) identifiable_by = data["identifiable_by"];
        if (data["unknown_name"]) unknown_name = data["unknown_name"];
    }
}

void set_short(string s) { short_desc = s; }
string query_short()     { return short_desc; }
void set_long(string l)  { long_desc = l; }
string query_long()      { return long_desc; }

void set_respawn_room(string r) { respawn_room = r; }
string query_respawn_room()     { return respawn_room; }

string *query_identifiable_by_factors() { return identifiable_by; }
string query_unknown_name()             { return unknown_name; }

// 檢查是否擁有所有偵測防護因子
int _player_has_required_factors(object player) {
    if (!detection_factors || sizeof(detection_factors) == 0) return 1;
    foreach (string f in detection_factors) {
        if (!player->has_factor(f)) {
            return 0;
        }
    }
    return 1;
}

// 檢查偵測
void check_detection(object player) {
    if (!player) return;
    
    if (_player_has_required_factors(player)) {
        // 玩家已經有對應 Discovery，安全通過
        return;
    }
    
    // 沒有 → 觸發 Consequence
    int hp_chg = -30;
    string conf = "caught_by_predator";
    
    if (failure_consequence) {
        if (!undefinedp(failure_consequence["hp_change"])) {
            hp_chg = failure_consequence["hp_change"];
        }
        if (!undefinedp(failure_consequence["trigger_confusion"])) {
            conf = failure_consequence["trigger_confusion"];
        }
    }
    
    tell_object(player, RED + "\n⚠️ 你被 " + query_name() + " 發現了！\n" + NOR);
    player->add_hp(hp_chg);
    player->player_confused(conf);
}

void on_death(object killer) {
    object room = environment(this_object());
    if (room) {
        foreach (object ob in room->query_occupants()) {
            tell_object(ob, HIG + query_name() + " 消失了。\n" + NOR);
        }
        room->leave(this_object());
    }
    if (stringp(respawn_room) && respawn_room != "" && respawn_delay > 0) {
        call_out("respawn", respawn_delay);
    }
}

void respawn() {
    if (!stringp(respawn_room) || respawn_room == "") return;
    set_hp(query_max_hp());
    object room = catch(load_object(respawn_room));
    if (room) {
        move_object(this_object(), room);
        room->enter(this_object());
        foreach (object ob in room->query_occupants()) {
            if (ob != this_object()) {
                tell_object(ob, MAG + "一隻 " + query_name() + " 出現了。\n" + NOR);
            }
        }
    }
}
