// /runtime/core/node.c
// FSE 核心通用節點基底 (Node Base)
// 支援完全資料驅動，從 YAML 檔案讀取配置與邏輯
#include "/runtime/include/ansi.h"

inherit "/runtime/core/entity";
inherit "/runtime/core/virtual_object";
inherit "/std/reveal_layer.c";

void create() {
    ::create();
    set_entity_type("node");
    
    // 初始化虛擬路徑與 YAML 設定
    setup_virtual("nodes", "node.yaml");
    
    mapping config = query_virtual_config();
    if (config) {
        if (config["node_id"]) {
            set_entity_id("node:" + config["node_id"]);
        }
        
        if (arrayp(config["reveal_layers"])) {
            foreach (mapping layer in config["reveal_layers"]) {
                add_reveal_layer(layer);
            }
        }
    }
}

mapping query_node_config() { return query_virtual_config(); }
string query_node_dir() { return query_virtual_dir(); }

int receive_execution(object player, mapping ast) {
    return load_object("/runtime/services/node_executor.c")->execute_challenges(this_object(), player, ast);
}

