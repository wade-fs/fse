// /runtime/core/virtual.c
// 通用虛擬物件 (Data-Driven) 編譯器/管理器

mapping virtual_rules = ([
    "nodes": "/std/node.c"
]);

void register_virtual_rule(string prefix, string std_file) {
    virtual_rules[prefix] = std_file;
}

object compile_virtual_object(string file) {
    if (sscanf(file, "%s.c", file)) {}
    
    string *parts = explode(file, "/");
    if (!parts || sizeof(parts) == 0) return 0;
    
    // 遍歷路徑的各個節點，只要匹配 prefix 規則 (例如 rooms, monsters, nodes) 即可
    foreach (string part in parts) {
        if (virtual_rules[part]) {
            object ob = clone_object(virtual_rules[part]);
            if (ob) {
                string cfg_file = (part == "rooms") ? "room.yaml" : "node.yaml";
                if (function_exists("setup_virtual_by_path", ob)) {
                    ob->setup_virtual_by_path(file, part, cfg_file);
                }
                
                // 重新載入配置後，重新對齊 entity_id
                if (function_exists("query_virtual_config", ob)) {
                    mapping config = ob->query_virtual_config();
                    if (config && config["node_id"] && function_exists("set_entity_id", ob)) {
                        ob->set_entity_id(config["node_id"]);
                    }
                }
            }
            return ob;
        }
    }
    return 0;
}
