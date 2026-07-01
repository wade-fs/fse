# Reality Resolver Technical Specification
# Reality Resolver 設計與規範說明

此文件保存了 FSE 第二代 `Reality Resolver` 執行判定器與宣告式 YAML 的結構設計。此設計保證 Runtime 核心不含任何 Adventure 特有邏輯，所有規律皆被提升為 Reality 庫與 Knowledge。

---

## 1. 概念階層結構 (Concept Hierarchy)

```
        Adventure (蜀山遊記 SO)
           │
     Challenge Definition (YAML)
           │
    ┌──────┼──────┐
    ▼      ▼      ▼
Natural  Social  Spiritual Realities (並行評估)
    │      │      │
    └──────┼──────┘
           ▼
    Reality Resolver (Runtime 統一執行器)
           │
    Understanding Evaluation (Misconception / Misunderstanding / Understanding)
           │
    Reveal / Memory / Discovery (認知更新)
           │
    World Consequence & Evolve (世界因果演變)
```

---

## 2. 宣告式 YAML 結構定義

### A. 全域 Reality 庫宣告 (例如：`/runtime/realities/natural/laws/fire.yaml`)

```yaml
law: fire
knowledges:
  east_wind_enables_fire_attack:
    description: "火借東風，方可破敵"
    
    observe:
      # Reality 向世界發出的感知訊號，取決於玩家解鎖的 Evidence
      - signal: "smoke_drifts_east"
        always_visible: true
      - signal: "enemy_ships_chained"
        requires_observation: "ship_position_noticed"
      - signal: "dry_reeds_on_shore"
        requires_observation: "terrain_awareness"

    predict:
      # 理解此 Knowledge 的玩家能預見之現象
      - "fire_spreads_in_wind_direction"
      - "chained_ships_cannot_escape"

    evaluate:
      # 行動驗證
      required_observations:
        - "smoke_drifts_east"
        - "enemy_ships_chained"
        - "dry_reeds_on_shore"
      aligned_action: "fire_attack"
      
      misunderstanding_patterns:
        - has: ["smoke_drifts_east", "enemy_ships_chained"]
          missing: ["dry_reeds_on_shore"]
          yields: "misunderstanding.fire_attack_premature"
        - has: ["smoke_drifts_east"]
          missing: ["enemy_ships_chained", "dry_reeds_on_shore"]
          yields: "misconception.wind_alone_sufficient"

    evolve:
      # 根據 Evaluate 結果，驅動世界狀態演變
      understanding:
        world_change: "fire_spreads_to_enemy_fleet"
        new_signals: ["fleet_in_chaos", "power_balance_shifted"]
      misunderstanding:
        world_change: "fire_contained_or_partial"
        new_signals: ["why_did_fire_stop", "unexpected_resistance"]
      misconception:
        world_change: "fire_extinguished_or_backfires"
        new_signals: ["smoke_blows_back", "disaster"]
```

### B. 冒險 Challenge 宣告 (引用 Reality 庫)

```yaml
challenge_id: "battle_of_chibi"
realities:
  - natural
  - social
  - strategic
knowledges:
  - natural.fire.east_wind_enables_fire_attack
  - social.loyalty.alliance_internal_tension
  - strategic.timing.overextension_fatal
```

---

## 3. Reality Resolver (Runtime) 處理流程

當玩家發出行動時，`reality_resolver.c` 執行以下流程：

1. **Parse & Map**: 讀取 challenge YAML 引用的 realities 與 knowledges。
2. **Scan Player Evidence**: 查詢玩家當前的 `observations` (Evidence 列表，取代二元標記)。
3. **Pattern Matching**:
   * 比對 Knowledge 的 `required_observations`，判斷是否滿足 `Understanding`。
   * 若不滿足，比對 `misunderstanding_patterns` 看是否匹配已知的局部盲點。
   * 若均不匹配，則判定為帶有成見的 `Misconception`。
4. **State Transition (認知更新)**: 
   * 解鎖新的 Reveal 視角。
   * 將此次的領悟寫入玩家的 `Memory`（永久儲存）。
5. **Causality Evolution (世界演化)**:
   * 根據 `evolve` 區塊，修改當前 Site_ID 的物理/精神狀態（如靈力變動、風向改變、NPC 態度）。
   * 將演化後產生的 `new_signals` 寫入玩家的 `observations` 列表，供下一次行動決策使用。
