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
            return clone_object(virtual_rules[part]);
        }
    }
    return 0;
}
