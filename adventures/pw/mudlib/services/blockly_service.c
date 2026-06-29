// /adventures/pw/mudlib/services/blockly_service.c
// PW 專屬 Blockly 工具箱服務 (Blockly Service)
// 職責：
//   - 整合 reveal_layer + factor + stage 狀態
//   - 將當前節點的 toolbox 格式化為前端標準 JSON 協定格式
//   - 回傳 TOOLBOX_UPDATE payload，供 execute.c 推送給前端

// ──────────────────────────────────────────────
// 積木類別定義表
// 格式：block_type_name -> ({ category_name, colour, output_type })
// 所有積木都在此集中管理，節點 reveal_layer 只需指定 block type 名稱即可
// ──────────────────────────────────────────────
private mapping block_registry = ([
    // 控制流
    "controls_loop":             ({ "控制流", "#5b80a5", "statement" }),
    "controls_loop_conditional": ({ "控制流", "#5b80a5", "statement" }),
    "controls_break":            ({ "控制流", "#5b80a5", "statement" }),
    // 數學運算 / 變數
    "variables_count":   ({ "變數與運算", "#a55b80", "value" }),
    "compare_less_100":  ({ "變數與運算", "#a55b80", "value" }),
    "action_increment":  ({ "變數與運算", "#a55b80", "value" }),
    "action_assignment": ({ "變數與運算", "#a55b80", "statement" }),
]);

// ──────────────────────────────────────────────
// 固定永遠可用的基礎積木（不受 reveal_layer 管控，此為最極簡的基礎，其餘應由 reveal_layers 或 stage 動態揭露）
// ──────────────────────────────────────────────
private mixed *base_blocks = ({
    "controls_loop",
});

// 將平坦的 block_type 清單組裝為前端需要的 categories 結構
// 回傳格式：
// ({
//   ([ "name": "控制流", "colour": "#5b80a5", "blocks": ({ ([ "type": "controls_loop" ]) }) ]),
//   ...
// })
private mixed *build_categories(mixed *block_types) {
    mapping cat_map = ([]);   // category_name -> ({ block_mapping, ... })
    mixed *cat_order = ({});  // 保持插入順序

    foreach (string btype in block_types) {
        mixed *info = block_registry[btype];
        if (!info) continue;   // 未知積木類型，跳過

        string cat_name   = info[0];
        string cat_colour = info[1];

        if (!cat_map[cat_name]) {
            cat_map[cat_name] = ([ "colour": cat_colour, "blocks": ({}) ]);
            cat_order += ({ cat_name });
        }
        cat_map[cat_name]["blocks"] += ({ ([ "type": btype ]) });
    }

    mixed *categories = ({});
    foreach (string name in cat_order) {
        mapping entry = cat_map[name];
        categories += ({ ([
            "name":   name,
            "colour": entry["colour"],
            "blocks": entry["blocks"],
        ]) });
    }
    return categories;
}

// ──────────────────────────────────────────────
// 主要公開 API
// 查詢玩家在當前節點可用的 toolbox，回傳標準 TOOLBOX_UPDATE payload
// ──────────────────────────────────────────────
mapping query_player_toolbox(object player) {
    if (!player) return 0;

    object env = environment(player);
    if (!env) {
        // 測試與執行期 fallback
        string test_site = player->query_temp("current_site");
        if (test_site) {
            env = load_object("/nodes/" + test_site + "/node");
        } else {
            env = load_object("/nodes/infinite_loop_swamp/node");
        }
    }
    if (!env) return 0;

    // 從節點取得當前可用積木（以 mapping 形式：block_type -> 1）
    mapping raw_toolbox = env->resolve_toolbox(player);

    // 合併基礎積木 + 解鎖積木，去重
    mixed *unlocked = ({});
    foreach (string btype in base_blocks) {
        if (member_array(btype, unlocked) == -1)
            unlocked += ({ btype });
    }
    foreach (string btype in keys(raw_toolbox)) {
        if (raw_toolbox[btype] && member_array(btype, unlocked) == -1)
            unlocked += ({ btype });
    }

    // 取得玩家已解鎖的因素清單
    mixed *factors = ({});
    catch(factors = player->query_all_factors());
    if (!arrayp(factors)) factors = ({});

    return ([
        "toolbox": ([
            "categories":       build_categories(unlocked),
            "unlocked_factors": factors,
        ])
    ]);
}

// ──────────────────────────────────────────────
// 格式化標準 JSON 協定訊息：TOOLBOX_UPDATE
// 注意：前綴 __JSON_MSG__ 讓 hub.go OutputCallback 以原始 JSON 傳送給前端
// ──────────────────────────────────────────────
string format_toolbox_update(object player) {
    mapping payload = query_player_toolbox(player);
    if (!payload) return 0;

    mapping msg = ([
        "type":    "TOOLBOX_UPDATE",
        "payload": payload,
    ]);
    return "__JSON_MSG__" + json_encode(msg);
}

// ──────────────────────────────────────────────
// 格式化標準 JSON 協定訊息：EXECUTION_RESULT
// 注意：前綴 __JSON_MSG__ 讓 hub.go OutputCallback 以原始 JSON 傳送給前端
// ──────────────────────────────────────────────
string format_execution_result(int success, string message, int memory_cost,
                               mixed *new_factors, object player) {
    // 若成功，附上最新 toolbox delta（完整 toolbox 方便前端直接套用）
    mapping toolbox_delta = 0;
    if (success && player) {
        toolbox_delta = query_player_toolbox(player);
    }

    mapping payload = ([
        "success":     success,
        "message":     message || "",
        "memory_cost": memory_cost || 0,
    ]);
    if (arrayp(new_factors) && sizeof(new_factors) > 0) {
        payload["new_factors"] = new_factors;
    }
    if (toolbox_delta) {
        payload["toolbox_delta"] = toolbox_delta;
    }

    mapping msg = ([
        "type":    "EXECUTION_RESULT",
        "payload": payload,
    ]);
    return "__JSON_MSG__" + json_encode(msg);
}

// ──────────────────────────────────────────────
// 格式化標準 JSON 協定訊息：WORLD_STATE
// 注意：前綴 __JSON_MSG__ 讓 hub.go OutputCallback 以原始 JSON 傳送給前端
// ──────────────────────────────────────────────
string format_world_state(object player) {
    if (!player) return 0;

    object env = environment(player);
    if (!env) {
        string test_site = player->query_temp("current_site");
        if (test_site) {
            env = load_object("/nodes/" + test_site + "/node");
        } else {
            env = load_object("/nodes/infinite_loop_swamp/node");
        }
    }
    string current_node = "unknown";
    string current_node_name = "";
    string current_node_desc = "";
    string challenge_desc = "";
    if (env) {
        mapping cfg = 0;
        catch(cfg = env->query_node_config());
        if (cfg) {
            if (cfg["node_id"]) current_node = cfg["node_id"];
            current_node_name = cfg["name"] || "";
            current_node_desc = cfg["description"] || "";
            
            // 讀取第一個挑戰的描述
            mixed *chals = cfg["challenges"];
            string node_dir = env->query_node_dir();
            if (arrayp(chals) && sizeof(chals) > 0 && node_dir) {
                string cid = chals[0]["id"];
                string chal_path = sprintf("%schallenges/%s.yaml", node_dir, cid);
                if (file_size(chal_path) > 0) {
                    string raw_chal = read_file(chal_path);
                    if (raw_chal) {
                        mapping chal_data = 0;
                        catch(chal_data = yaml_decode(raw_chal));
                        if (chal_data && chal_data["description"]) {
                            challenge_desc = chal_data["description"];
                        }
                    }
                }
            }
        }
    }

    int memory_val = 100;
    catch(memory_val = player->query_physical_state("memory"));
    mixed *factors = ({});
    catch(factors = player->query_all_factors());

    mapping msg = ([
        "type": "WORLD_STATE",
        "payload": ([
            "current_node":      current_node,
            "current_node_name": current_node_name,
            "current_node_desc": current_node_desc,
            "challenge_desc":    challenge_desc,
            "memory":            memory_val,
            "unlocked_factors":  arrayp(factors) ? factors : ({}),
        ]),
    ]);
    return "__JSON_MSG__" + json_encode(msg);
}
