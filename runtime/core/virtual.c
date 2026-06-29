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
    int offset = (sizeof(parts) > 0 && parts[0] == "") ? 1 : 0;
    
    if (sizeof(parts) >= (3 + offset)) {
        string prefix = parts[offset]; 
        if (virtual_rules[prefix]) {
            return clone_object(virtual_rules[prefix]);
        }
    }
    return 0;
}
