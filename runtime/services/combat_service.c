// /runtime/services/combat_service.c
// FSE 核心戰鬥規則計算服務

void create() {}

// 計算單次攻擊造成的實際傷害
int calculate_damage(object attacker, object defender) {
    if (!attacker || !defender) return 0;
    
    int atk = attacker->query_attack();
    int def = defender->query_defense();
    
    // 基礎隨機傷害波動
    int base_dmg = atk + random(atk / 2 + 1);
    
    // 扣除防禦力
    int actual_dmg = base_dmg - def;
    
    // 最低保證 1 點傷害
    if (actual_dmg < 1) {
        actual_dmg = 1;
    }
    
    return actual_dmg;
}
