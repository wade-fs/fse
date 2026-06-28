// /std/reveal_layer.c
// 條件顯示與過濾層系統 (PW 專屬過濾)
private nosave mixed *reveal_layers;

void create() {
    reveal_layers = ({});
}

void add_reveal_layer(mapping layer) {
    if (!layer["condition"]) layer["condition"] = "all";
    if (!layer["checks"])    layer["checks"]    = ({});
    reveal_layers += ({ layer });
}

private int check_single(object player, mapping chk) {
    string type = chk["type"];

    if (type == "stage_completed")
        return load_object("/runtime/services/progress_service.c")->stage_completed(chk["stage"]);

    if (type == "discovery_completed")
        return load_object("/runtime/services/discovery_service.c")->discovery_completed(player, chk["discovery_id"]);

    if (type == "factor_discovered")
        return load_object("/runtime/services/factor_service.c")->factor_discovered(player, chk["factor_id"]);

    return 0;
}

private int check_layer(object player, mapping layer) {
    mixed *checks = layer["checks"];
    string cond   = layer["condition"];

    if (!checks || !sizeof(checks)) return 1;

    if (cond == "any") {
        foreach (mapping chk in checks)
            if (check_single(player, chk)) return 1;
        return 0;
    }

    foreach (mapping chk in checks)
        if (!check_single(player, chk)) return 0;
    return 1;
}

mapping resolve_toolbox(object player) {
    mapping toolbox = ([ "basic_sequence": 1 ]); 

    foreach (mapping layer in reveal_layers) {
        if (!check_layer(player, layer)) continue;

        if (layer["reveal_block"]) {
            toolbox[layer["reveal_block"]] = 1;
        }
    }

    return toolbox;
}

mixed *query_reveal_layers() { return copy(reveal_layers); }
