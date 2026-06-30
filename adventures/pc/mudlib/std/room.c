// /std/room.c  (史前文明 PC 房間底層類別)
// 繼承自 FSE Engine 核心並實作冒險專屬物理/難度副作用
#include "/include/ansi.h"
inherit "/std/fse_room";

float query_difficulty_modifier(string type);

void create() {
    ::create();
}

// 實作冒險專屬的後置修飾與物理副作用 (如飢渴、疲勞)
void apply_adventure_side_effects(object player, mapping act, int passed) {
    if (!player || !act) return;

    if (passed) {
        // 疲勞與體力變更 (套用難度倍率)
        int fatigue = act["fatigue"];
        if (fatigue) {
            float f_mod = query_difficulty_modifier("fatigue");
            player->add_fatigue(to_int(fatigue * f_mod));
        }

        int hp_change = act["hp_change"];
        if (hp_change) {
            if (hp_change < 0) {
                float d_mod = query_difficulty_modifier("damage");
                player->add_hp(to_int(hp_change * d_mod));
            } else {
                player->add_hp(hp_change);
            }
        }

        int hunger_change = act["hunger_change"];
        if (hunger_change && function_exists("add_hunger", player)) {
            player->add_hunger(hunger_change);
        }

        int thirst_change = act["thirst_change"];
        if (thirst_change && function_exists("add_thirst", player)) {
            player->add_thirst(thirst_change);
        }
    } else {
        // 懲罰 (套用難度倍率)
        int fail_fatigue = act["fail_fatigue"] || 5;
        float f_mod = query_difficulty_modifier("fatigue");
        player->add_fatigue(to_int(fail_fatigue * f_mod));

        int fail_hp = act["fail_hp"];
        if (fail_hp) {
            if (fail_hp < 0) {
                float d_mod = query_difficulty_modifier("damage");
                player->add_hp(to_int(fail_hp * d_mod));
            } else {
                player->add_hp(fail_hp);
            }
        }
    }
}

// 實作冒險專屬的物品生成後置修飾 (如工具耐久度受環境難度衰減)
void on_item_spawned(object ob, mapping give_item) {
    if (ob && ob->query_durability() > 0) {
        float dec_mod = query_difficulty_modifier("decay");
        int final_dur = to_int(ob->query_durability() / dec_mod);
        if (final_dur < 1) final_dur = 1;
        ob->set_durability(final_dur);
    }
}

// 🌟 PC 專屬的動態難度係數解算器
float query_difficulty_modifier(string type) {
    string mode = "normal"; // 預設難度
    
    // 1. 讀取全域 manifest.yaml 配置
    string raw = read_file("/manifest.yaml");
    mapping manifest;
    if (raw) {
        manifest = yaml_decode(raw);
        if (manifest && manifest["difficulty"] && manifest["difficulty"]["current"]) {
            mode = manifest["difficulty"]["current"];
        }
    }

    // 2. 檢查房間是否定義了覆寫難度
    mapping config = query_virtual_config();
    if (config && config["difficulty_override"]) {
        mode = config["difficulty_override"];
    }

    // 3. 取得對應難度模式下的倍率
    if (manifest && manifest["difficulty"] && manifest["difficulty"]["modes"] && manifest["difficulty"]["modes"][mode]) {
        mapping mode_cfg = manifest["difficulty"]["modes"][mode];
        string key = type + "_multiplier";
        if (!undefinedp(mode_cfg[key])) {
            return to_float(mode_cfg[key]);
        }
    }

    return 1.0;
}
