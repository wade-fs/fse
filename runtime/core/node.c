// /runtime/core/node.c
// FSE 核心通用節點基底 (Node Base)
// 支援完全資料驅動，從 YAML 檔案讀取配置與邏輯
#include "/runtime/include/ansi.h"

inherit "/runtime/core/entity";
inherit "/std/reveal_layer.c";

private string node_dir;
private mapping node_config;

void create() {
    ::create();
    set_entity_type("node");
    
    // 從檔名推導所在目錄。例如虛擬路徑為: /nodes/counter_valley/node -> /content/nodes/counter_valley/
    string fname = file_name(this_object());
    string *parts = explode(fname, "/");
    if (sizeof(parts) >= 2) {
        string node_id = parts[sizeof(parts)-2];
        node_dir = "/content/nodes/" + node_id + "/";
        
        // 載入該節點的 YAML 設定
        string node_yaml = read_file(node_dir + "node.yaml");
        if (node_yaml) {
            mixed err = catch(node_config = yaml_decode(node_yaml));
            if (err) {
                log_file("sys_error.log", sprintf("[%s] node.c yaml_decode error (%s): %s\n", ctime(time()), node_dir, err));
            }
        }
        
        if (node_config) {
            if (node_config["node_id"]) {
                set_entity_id("node:" + node_config["node_id"]);
            }
            
            // 載入 reveal_layers 設定
            if (arrayp(node_config["reveal_layers"])) {
                foreach (mapping layer in node_config["reveal_layers"]) {
                    add_reveal_layer(layer);
                }
            }
        }
    }
}

mapping query_node_config() { return node_config; }
string query_node_dir() { return node_dir; }

int receive_execution(object player, mapping ast) {
    return load_object("/runtime/services/node_executor.c")->execute_challenges(this_object(), player, ast);
}

