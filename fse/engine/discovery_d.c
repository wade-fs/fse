// /fse/engine/discovery_d.c
// FSE (Flow State Engine) 領悟/發現管理器相容層 (Discovery Daemon Compatibility Wrapper)

inherit "/fse/std/object";

int discovery_completed(object player, string did) {
    return load_object("/fse/engine/factor_d.c")->factor_discovered(player, did);
}

int unlock_discovery(object player, string did) {
    return load_object("/fse/engine/factor_d.c")->discover_factor(player, did);
}
