// /secure/crafting_d.c
// 合成守護進程 (Crafting Daemon)
// 負責處理 鑲嵌 (Socketing)、鍛造 (Forging) 與 製藥 (Alchemy) 的配方與邏輯

#include "/include/ansi.h"

mapping recipes;

inherit "/std/object";

void create() {
    ::create();
    recipes = ([
        // ── 製藥 (Alchemy) ──
        "health_potion": ([
            "type": "alchemy",
            "name": "初級恢復藥水",
            "materials": ([ "/item/material/slime_jelly.c": 2, "/item/material/wood.c": 1 ]),
            "result": "/item/consumable/health_potion.c",
            "msg": "你將黏液與草藥混合攪拌，瓶中泛起了紅色的微光。"
        ]),

        // ── 鍛造 (Forging) ──
        "crab_shield": ([
            "type": "forge",
            "name": "加固木盾",
            "materials": ([ "/item/material/crab_shell.c": 2, "/item/material/wood.c": 1 ]),
            "result": "/item/armour/reinforced_shield.c",
            "msg": "你利用堅硬的螃蟹殼加固了護盾，防禦力大幅提升。"
        ]),

        // ── 鑲嵌 (Socketing) ──
        "socket_fire": ([
            "type": "socket",
            "name": "火焰鑲嵌",
            "materials": ([ "/item/gem/fire_shard.c": 1 ]),
            "msg": "裝備散發出熾熱的紅光！",
            "apply_func": "apply_socket_fire"
        ])
    ]);
}

// 鑲嵌邏輯實作
void apply_socket_fire(object target) {
    target->set_element("fire");
    target->set_attack(target->query_attack() + 5);

    // 這裡暫時維持簡單的字串串接，實務上可根據語系修改
    object tp = this_player();
    string lang = "en";
    if (tp) {
        lang = tp->query_lang();
    }
    
    if (lang == "zh-TW") target->set_name("火焰之" + target->query_name());
    else if (lang == "zh-CN") target->set_name("火焰之" + target->query_name());
    else target->set_name("Flame " + target->query_name());
}

mapping query_recipe(string id) { return recipes[id]; }

// 核心邏輯：嘗試合成
mixed do_craft(object me, string type, object *items) {
    if (!items || sizeof(items) < 2) return "你至少需要兩樣東西才能進行合成。";

    // 1. 收集投入物品的分布 (改用 base_name)
    mapping inputs = ([]);
    for (int i = 0; i < sizeof(items); i++) {
        object ob = items[i];
        string n = base_name(ob);
        int count = inputs[n];
        if (!count) {
            inputs[n] = 1;
        } else {
            inputs[n] = count + 1;
        }
    }

    // 2. 遍歷配方尋找匹配
    mixed ks = keys(recipes);
    for (int i = 0; i < sizeof(ks); i++) {
        string rid = ks[i];
        mapping r = recipes[rid];
        if (r["type"] != type) continue;

        mapping req = r["materials"];
        if (!req) continue;

        int match = 1;
        mixed req_ks = keys(req);
        
        for (int j = 0; j < sizeof(req_ks); j++) {
            string rname = req_ks[j];
            if (inputs[rname] < req[rname]) {
                match = 0;
                break;
            }
        }

        if (match) {
            // 檢查是否是鑲嵌 (Socket) 模式
            if (type == "socket") {
                object base = 0;
                for (int k = 0; k < sizeof(items); k++) {
                    object ob = items[k];
                    string itype = ob->query_item_type();
                    if (itype == "weapon" || itype == "armour") {
                        base = ob;
                        break;
                    }
                }
                if (!base) return "鑲嵌需要一個裝備作為基底。";
                
                if (r["apply_func"]) {
                    call_other(this_object(), r["apply_func"], base);
                    for (int k = 0; k < sizeof(items); k++) {
                        if (items[k] != base) destruct(items[k]);
                    }
                    write("$HIG$鑲嵌成功！$NOR$" + r["msg"] + "\n");
                    return 1;
                }
            }

            // 一般合成模式
            for (int k = 0; k < sizeof(items); k++) {
                destruct(items[k]);
            }
            
            object res_ob = clone_object(r["result"]);
            if (res_ob && !errorp(res_ob)) {
                write("$HIG$合成成功！$NOR$" + r["msg"] + "\n");
                move_object(res_ob, me); // 直接移動
                return 1;
            } else {
                write("$HIR$合成失敗：無法產生產物物件。\n$NOR$");
                return 0;
            }
        }
    }

    return "你弄了半天，結果什麼都沒發生，材料也差點報廢了。";
}

mapping query_all_recipes() { return recipes; }
