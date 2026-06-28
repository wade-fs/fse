// /runtime/services/discovery_service.c
// FSE 領悟/發現管理器 (Discovery Service)

int discovery_completed(object player, string did) {
    return load_object("/runtime/services/factor_service.c")->factor_discovered(player, did);
}

int unlock_discovery(object player, string did) {
    return load_object("/runtime/services/factor_service.c")->discover_factor(player, did);
}
