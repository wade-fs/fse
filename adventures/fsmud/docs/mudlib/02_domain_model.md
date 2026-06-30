# docs/mudlib/02_domain_model.md

# 源流福爾摩沙 — Domain Driven Design

## 文件定位

本文件是 mudlib 工程文件，不是 Canon。

Canon 定義世界觀。

本文件定義：

> Canon 如何翻譯為可運作的 LPC 物件模型。

---

## 設計原則

### Canon First

所有 Domain 決策以 Canon 為最高依據。

若 Canon 與工程慣例衝突，Canon 優先。

---

### 分散式，不是層級制

Canon 明確指出：

> 台灣是中小企業、地方主義、沒有一家獨大。

因此本 Domain 模型刻意迴避中央集權的 Manager 模式。

每個 Aggregate 自己管理自己的狀態。

Service 只做跨 Aggregate 的協調，不持有核心狀態。

---

### 記憶是第一公民

本作最核心的設計：

記憶不是副產品，不是 log，不是 metadata。

記憶是世界的主要狀態。

所有其他系統都服務於記憶的生成、保存、共振、消亡。

---

## Bounded Context 劃分

```
┌────────────────────────────────────────────────────────────┐
│                    源流福爾摩沙 MUD World                  │
│                                                            │
│  ┌──────────────┐   ┌──────────────┐   ┌────────────────┐  │
│  │  World       │   │  Memory      │   │  Profession    │  │
│  │  Context     │   │  Context     │   │  Context       │  │
│  │              │   │              │   │                │  │
│  │  World       │   │  Memory      │   │  Career        │  │
│  │  Era         │   │  Crystal     │   │  Driftwork     │  │
│  │  Site        │   │  Resonance   │   │  Achievement   │  │
│  └──────┬───────┘   └──────┬───────┘   └───────┬────────┘  │
│         │                  │                   │           │
│  ┌──────▼───────┐   ┌──────▼───────┐   ┌───────▼────────┐  │
│  │  Settlement  │   │  Footprint   │   │  Player        │  │
│  │  Context     │   │  Context     │   │  Context       │  │
│  │              │   │              │   │                │  │
│  │  Settlement  │   │  Footprint   │   │  Player        │  │
│  │  Population  │   │  Resonance   │   │  Inventory     │  │
│  │  SixDims     │   │  Atlas       │   │  Position      │  │
│  └──────┬───────┘   └──────┬───────┘   └───────┬────────┘  │
│         │                  │                   │           │
│  ┌──────▼──────────────────▼───────────────────▼────────┐  │
│  │                   Event Bus                          │  │
│  │   MemoryUnlocked / FootprintGained /                 │  │
│  │   SettlementChanged / EraShifted /                   │  │
│  │   ResonanceTriggered / OblivionRising                │  │
│  └──────────────────────────────────────────────────────┘  │
└────────────────────────────────────────────────────────────┘
```

---

## Aggregate 清單

```
World
Era
Site
Settlement
SixDimensions
Population
Memory
MemoryCrystal
Resonance
Oblivion
Specter  （失源者）
Footprint
FootprintAtlas
Career
Player
Faction
NPC
```

---

---

# Aggregate 詳細定義

---

## World

### 定義

整個福爾摩沙島嶼的頂層容器。

不是「場景」，而是世界狀態的持有者。

---

### 責任

```
持有當前 Era
持有全島文明度
判斷版本翻頁條件
觸發 EraShifted 事件
```

---

### 狀態

```yaml
world:
  id: "formosa"
  current_era: "v1.9"
  civilization_level: 42        # 全島文明度 0-100
  resonance_count: 7            # 已觸發的跨時代共振次數
  final_resonance_unlocked: false
```

---

### LPC 物件對應

```
/daemon/world_d.c
```

單一 Daemon，全服唯一。

---

### Domain Events 產出

```
EraShifted(from_era, to_era, timestamp)
FinalResonanceUnlocked()
```

---

### 版本翻頁規則

```
當 current_era 的 Era.completion_rate >= 70%
且 世界累計聚落完成率 >= 60%

→ 觸發 EraShifted
→ current_era 推進至下一版本
```

---

---

## Era

### 定義

一個歷史時代版本。

對應 Canon timeline 的每一個 v 號。

---

### 責任

```
持有本時代所有 Site 的參考
持有本時代的完成率
判斷核心事件是否已被見證
```

---

### 狀態

```yaml
era:
  id: "v1.9"
  name: "乙未之殤"
  time_range: "1895"
  theme: "被割讓的島嶼"
  completion_rate: 0.0          # 0.0 ~ 1.0
  key_events:
    - id: "EP006"
      name: "乙未之夜"
      witnessed: false
    - id: "EP007"
      name: "乙未戰爭"
      witnessed: false
  unlock_condition:
    previous_era: "v1.5"
    required_footprints:
      - type: "時代踏印"
        minimum: 3
```

---

### LPC 物件對應

```
/area/eras/v1_9.c
```

每個 Era 是一個獨立物件，不繼承房間。

---

### Domain Events 產出

```
EraCompleted(era_id, completion_rate)
KeyEventWitnessed(era_id, event_id, player_id)
```

---

---

## Site

### 定義

一個具體的歷史場景地點。

對應 Canon historical_events.md 的每一個事件內的地點。

與 Settlement 不同：

- Settlement 是「現代聚落的行政單位」
- Site 是「某個歷史事件發生的具體地點」

同一個地理位置可以有多個 Site，分屬不同 Era。

---

### 狀態

```yaml
site:
  id: "taipei_city_gate_v1.9"
  name: "台北城門"
  era_id: "v1.9"
  settlement_id: "taipei_city"
  coordinates: { x: 30, y: 20 }
  memory_layers:
    - era: "v1.9"
      accessible: true
    - era: "v1.5"
      accessible: false          # 需要前置踏印
  npc_refs:
    - "gu_xianrong_v1.9"
    - "city_guard_anonymous"
  memory_fragments:
    - id: "MF_taipei_gate_001"
      collected: false
    - id: "MF_taipei_gate_002"
      collected: false
  oblivion_rate: 0.15            # 0.0=完好, 1.0=完全失源
```

---

### LPC 物件對應

```
/area/sites/taipei_city_gate.c
```

繼承 `/std/room.c`，但加入記憶層邏輯。

---

---

## Settlement

### 定義

台灣行政單位。

368 個鄉鎮市區 + 22 個縣市。

---

### 責任

```
持有六維數值
持有聚落記憶完成率
判斷升級條件
持有所在 Site 的參考
持有 NPC 居民
觸發失源者生成/消滅
```

---

### 狀態

```yaml
settlement:
  id: "minxiong"
  name: "民雄鄉"
  level: "village"              # village / city3 / city2 / city1
  tier: 4                       # 1=直轄市 ... 4=村級
  six_dims:
    population: 45
    industry: 38
    culture: 52
    memory: 61                  # 最關鍵指標
    trade_route: 40
    cohesion: 55
  memory_completion: 0.61       # 與 six_dims.memory 同步
  specters_active: []           # 當前活躍失源者 ID 列表
  faction_influence:
    mazu_network: 0.4
    local_gentry: 0.3
  unlock_events:
    - era: "v0.1"
      site_id: "xilaya_plain_minxiong"
      unlocked: true
    - era: "v2.1"
      site_id: "xilai_an_minxiong_echo"
      unlocked: false
```

---

### LPC 物件對應

```
/area/settlements/minxiong.c
```

---

### SixDimensions Value Object

六維是 Settlement 的內嵌值物件，不單獨存在。

```c
// 在 settlement.c 內部管理
mapping six_dims = ([
    "population": 45,
    "industry":   38,
    "culture":    52,
    "memory":     61,
    "trade":      40,
    "cohesion":   55
]);
```

---

### 升級規則

```
village → city3：
  memory >= 60 且 culture >= 50 且 cohesion >= 50

city3 → city2：
  memory >= 70 且 industry >= 60 且 trade >= 55
  且 至少一條跨聚落商路已建立

city2 → city1：
  固定六座，不可升級產生
```

---

### 失源者生成規則

```
當 memory < 30
  → 生成 Specter，型態由最低的子項目決定：
    memory.history_events < 0.3  → 失史者
    memory.oral_tradition < 0.3  → 失語者
    memory.named_persons < 0.3   → 失名者
    memory.sites < 0.3           → 失鄉者
```

---

### Domain Events 產出

```
SettlementMemoryChanged(settlement_id, old_val, new_val)
SettlementUpgraded(settlement_id, from_level, to_level)
SpecterSpawned(settlement_id, specter_id, specter_type)
SpecterResolved(settlement_id, specter_id)
```

---

---

## Memory

### 定義

Canon 定義了五層記憶：

```
島嶼記憶 > 時代記憶 > 聚落記憶 > 家族記憶 > 個人記憶
```

Memory 是這五層的統一 Aggregate。

---

### 狀態

```yaml
memory:
  id: "MEM_minxiong_sugarmill_worker_1920"
  layer: "personal"             # personal / family / settlement / era / island
  era_id: "v2.1"
  settlement_id: "minxiong"
  title: "糖廠工人阿福的最後一班"
  description: "..."
  quality: "記錄"               # 殘片/記錄/傳承/史詩/源流
  collected_by: []              # player_id list
  fragments:
    - id: "MF_001"
      type: "oral"              # oral / document / artifact / site
      collected: false
      collector: null
    - id: "MF_002"
      type: "artifact"
      collected: true
      collector: "chen_jin"
  completion_rate: 0.5
  resonance_targets:            # 可與哪些 Memory 產生共振
    - "MEM_xilaya_plain_sugarland"
    - "MEM_janan_era_sugar_industry"
```

---

### LPC 物件對應

```
/lib/memory.c          # Memory prototype
/data/memories/*.yaml  # 資料（由 yaml_daemon 載入）
```

---

---

## MemoryCrystal

### 定義

玩家從歷史層帶回的具體道具。

是 Memory 的「攜帶式快照」。

---

### 狀態

```yaml
memory_crystal:
  id: "MC_wushe_dakis_loom_001"
  memory_id: "MEM_wushe_dakis_loom"
  quality: "史詩"
  era_id: "v2.2"
  obtained_by: "chen_jin"
  obtained_at: "v2.2_site_mahebo"
  description: "Dakis出發前最後一次觸摸織布機的記憶碎片"
  can_share: true               # 是否可以傳遞給其他玩家/NPC
```

---

### LPC 物件對應

```
/area/lm/memory_crystal.c      # 道具物件，可攜帶
```

繼承 `/std/object.c`。

---

---

## Resonance

### 定義

跨 Memory 的共振事件。

是本作最重要的系統性獎勵機制。

---

### 責任

```
追蹤共振條件是否滿足
觸發共振效果
產出 ResonanceTriggered 事件
```

---

### 狀態

```yaml
resonance:
  id: "RES_janan_plain"
  name: "嘉南平原共振"
  type: "regional"              # personal / regional / era / island
  required_memories:
    - "MEM_xilaya_cengwen_river"
    - "MEM_zhengchenggong_luermen"
    - "MEM_qing_reclamation_janan"
    - "MEM_japanese_sugar_janan"
    - "MEM_postwar_janan_farm"
  triggered: false
  triggered_by: null            # player_id
  effects:
    - type: "unlock_site"
      target: "site_xilaya_hidden_altar"
    - type: "settlement_memory_boost"
      target: "minxiong"
      amount: 15
    - type: "npc_appear"
      target: "NPC_xilaya_elder_ghost"
    - type: "footprint_grant"
      target_player: "triggered_by"
      footprint: "FP_janan_source"
```

---

### LPC 物件對應

```
/daemon/resonance_d.c          # 全服唯一，監聽 MemoryUnlocked 事件
```

---

---

## Oblivion（遺忘值）

### 定義

與 Settlement.memory 相對的衰減機制。

不是單獨 Aggregate，而是 Settlement 的衍生計算。

但因為邏輯複雜，獨立為 Value Object。

---

### 計算規則

```
Oblivion.rate = 1.0 - Settlement.memory_completion

當 Oblivion.rate > 0.70：
  → 觸發 OblivionRising 事件
  → Specter 生成速度加快

當 Oblivion.rate > 0.90：
  → 觸發 SettlementAtRisk 事件
  → 聚落升級被鎖定
  → 所有踏印效果減半
```

---

---

## Specter（失源者）

### 定義

遺忘的具象。

不是怪物，是一種「缺失狀態的實體化」。

---

### 型態（繼承自 Canon）

```
失名者    → 個人記憶消失
失鄉者    → 聚落記憶消失
失語者    → 語言/技藝記憶消失
失史者    → 歷史事件記憶消失
```

---

### 狀態

```yaml
specter:
  id: "SP_minxiong_sugarmill_001"
  type: "失名者"
  settlement_id: "minxiong"
  linked_memory_id: "MEM_minxiong_worker_anonymous"
  oblivion_depth: 0.75          # 遺忘深度，越高越難解除
  resolved: false
  resolution_method:            # 解除條件
    type: "memory_restoration"
    required_actions:
      - "collect_oral_fragment"
      - "submit_to_settlement_archive"
```

---

### 解除規則

```
Specter 不能被「戰鬥」消滅。

必須由修復師職涯行動：
  1. 找到對應 Memory 的碎片
  2. 完成修復動作
  3. Memory.completion_rate 超過閾值
  → 觸發 SpecterResolved
```

---

### LPC 物件對應

```
/area/specters/specter_base.c       # 基礎型
/area/specters/lost_name.c          # 失名者
/area/specters/lost_place.c         # 失鄉者
/area/specters/lost_tongue.c        # 失語者
/area/specters/lost_history.c       # 失史者
```

---

---

## Footprint（踏印）

### 定義

玩家與土地產生連結的唯一成長指標。

對應 Canon footprint_system.md 的六大踏印類別。

---

### 型態

```
地理踏印    → GeoFootprint
人情踏印    → RelationFootprint
技藝踏印    → CraftFootprint
聚落踏印    → SettlementFootprint
時代踏印    → EraFootprint
源流踏印    → SourceFootprint
```

---

### 狀態

```yaml
footprint:
  id: "FP_minxiong_geo_001"
  type: "地理踏印"
  subtype: null
  name: "打貓踏印"
  era_id: null                  # 地理踏印不限時代
  settlement_id: "minxiong"
  quality: 2                    # 1=初識 2=熟悉 3=深入 4=源流
  obtained_by: "chen_jin"
  resonance_weight: 1.0         # 參與共振計算的權重
```

---

### 特殊踏印

```yaml
# 義氣踏印（Canon 特別指出最稀有）
footprint:
  id: "FP_yiwei_war_001"
  type: "時代踏印"
  subtype: "義氣踏印"
  name: "乙未踏印（初階）"
  era_id: "v1.9"
  quality: 3
  rarity: "rare"
```

---

### LPC 物件對應

```
/lib/footprint.c               # Footprint prototype
```

不是房間，不是道具，是純資料結構。

存在 Player 的 footprint_atlas 裡。

---

---

## FootprintAtlas（踏印圖鑑）

### 定義

Player 持有的所有踏印的集合。

也是共振條件的計算來源。

---

### 狀態

```yaml
footprint_atlas:
  player_id: "chen_jin"
  footprints:
    - id: "FP_minxiong_geo_001"
      quality: 2
    - id: "FP_xilaya_001"
      quality: 3
  total_geo: 12
  total_relation: 8
  total_craft: 5
  total_settlement: 7
  total_era: 4
  total_source: 1
  resonance_eligible:           # 滿足共振條件的 Resonance ID 列表
    - "RES_janan_plain"
```

---

---

## Career（職涯）

### 定義

對應 Canon profession_system.md 的六大職涯。

不是固定職業，是累積的行為記錄。

---

### 狀態

```yaml
career:
  player_id: "chen_jin"
  tracks:
    地誌師:
      rank: "路上行者"           # 初識踏查者 / 路上行者 / 地方熟手 / 地誌師 / 島嶼踏查者
      action_count: 47
      sub_tracks:
        踏查者: { count: 30, unlocked: true }
        古道人: { count: 12, unlocked: true }
        地景記錄者: { count: 5, unlocked: false }
        聚落調查師: { count: 0, unlocked: false }
    匠師:
      rank: "學徒"
      action_count: 18
    修復師:
      rank: "記憶守護者"
      action_count: 65
    說書人:
      rank: "會說故事的人"
      action_count: 33
    行商:
      rank: "行商"
      action_count: 21
    耕作者:
      rank: "鄰居"
      action_count: 14
  cross_career_titles:           # 跨職涯加乘稱謂
    - "踏查誌作者"               # 地誌師 + 說書人
```

---

### LPC 物件對應

```
/lib/career.c                   # Career prototype
```

存在 Player 物件內部。

---

---

## Player

### 定義

玩家角色。

注意：Player 在本作不是「英雄」，而是「容器」。

---

### 狀態

```yaml
player:
  id: "chen_jin"
  name: "陳進"
  talent: "island_memory"       # 唯一特殊天賦，全玩家共用
  current_site_id: "taipei_city_gate_v1.9"
  current_era_id: "v1.9"
  position: { x: 30, y: 20 }
  inventory:
    memory_crystals:
      - "MC_wushe_dakis_loom_001"
    items:
      - "block_stone_001"
  footprint_atlas_id: "chen_jin"
  career_id: "chen_jin"
  faction_memberships:
    - faction_id: "mazu_network"
      reputation: 42
    - faction_id: "local_gentry_chiayi"
      reputation: 28
  temp:
    lm_return_room: "/area/newbie/room_0_0.c"
    last_location: "/area/sites/taipei_city_gate.c"
```

---

### LPC 物件對應

```
/std/player.c                   # 繼承 ES2 mudlib player
```

新增欄位透過 `query_temp` / `set_temp` 或 save 欄位擴充。

---

---

## Faction（勢力）

### 定義

對應 Canon factions.md。

不是陣營，是關係網絡。

玩家可同時加入多個。

---

### 型態

```
歷史家族        → 板橋林家、鹿港辜家
地方商會        → 鹿港商會、北部行棧
宗教信仰體系    → 媽祖網絡、王爺祭典、城隍系統
原住民族群      → 西拉雅聯盟、賽德克部落
文化保存組織    → 文協後裔、修復師公會
抗爭記憶後裔    → 義民後裔、二二八紀念會
```

---

### 狀態

```yaml
faction:
  id: "mazu_network"
  name: "媽祖信眾聯盟"
  type: "宗教信仰體系"
  settlements:
    - id: "beigang"
      influence: 0.9
    - id: "dajia"
      influence: 0.85
    - id: "minxiong"
      influence: 0.4
  era_active:
    - "v1.0"
    - "v1.2"
    - "v1.5"
    - "v1.9"
    - "v2.0"
    - "v2.1"
    - "v3.0"
    - "v4.0"
  reputation_tiers:
    stranger:  { min: 0,   label: "陌生人" }
    acquaint:  { min: 20,  label: "香客" }
    trusted:   { min: 50,  label: "爐主" }
    core:      { min: 80,  label: "媽祖使者" }
```

---

---

## NPC

### 定義

世界中的非玩家角色。

分為三類：

```
歷史人物     → 有史料依據，行為受 Canon 限制
無名之人     → Canon 核心，大多數任務的來源
高齡耆老     → 有生命值倒數，口述採集的緊迫性來源
```

---

### 狀態

```yaml
npc:
  id: "NPC_elder_matagi_oral"
  name: "馬卡達"
  type: "elder"                  # historical / anonymous / elder
  settlement_id: "xingang"
  era_id: "v0.1"
  vitality: 78                   # 高齡耆老專屬，0=消逝，記憶永久失落
  vitality_decay_rate: 0.05      # 每遊戲日衰減
  oral_memories:
    - memory_id: "MEM_xilaya_ritual_song"
      collected: false
  dialogue_unlocks:
    - condition: { player_reputation: 30 }
      content: "第一次婉拒"
    - condition: { player_reputation: 60, item_provided: "ritual_offering" }
      content: "第二次條件交換"
    - condition: { player_reputation: 80 }
      content: "完整歌謠"
```

---

### 高齡耆老機制

```
vitality 每遊戲日衰減 decay_rate

當 vitality <= 0：
  NPC 消逝
  未收集的 oral_memories 標記為 "永久失落"
  觸發 OralMemoryLost(npc_id, memory_ids)
  聚落 memory 值下降

這是本作最緊張的機制之一。
```

---

---

# Domain Service 清單

---

## MemoryService

```
職責：Memory 的收集、完成率計算、品質判定

主要方法：
  collect_fragment(player, memory_id, fragment_id) → result
  calculate_completion(memory_id) → float
  upgrade_quality(memory_id) → MemoryCrystal
  check_resonance_eligibility(player_id) → resonance_ids[]
```

---

## ResonanceService

```
職責：監聽 MemoryUnlocked 事件，判斷共振條件，觸發共振效果

主要方法：
  check_all_resonances(player_id)
  trigger_resonance(resonance_id, player_id)
  apply_resonance_effects(resonance)
```

---

## SettlementService

```
職責：六維數值計算、升級判斷、Specter 生命週期

主要方法：
  update_six_dims(settlement_id, delta_map)
  check_upgrade_condition(settlement_id) → bool
  spawn_specter(settlement_id, specter_type)
  resolve_specter(specter_id, player_id)
```

---

## FootprintService

```
職責：踏印授予、品質判定、Atlas 更新

主要方法：
  grant_footprint(player_id, footprint_spec) → footprint
  upgrade_footprint_quality(player_id, footprint_id)
  calculate_resonance_weight(player_id) → float
```

---

## CareerService

```
職責：職涯行動計數、等級推進、跨職涯稱謂解鎖

主要方法：
  record_action(player_id, career_track, action_type)
  check_rank_up(player_id, career_track) → bool
  check_cross_career_titles(player_id) → titles[]
```

---

## TimelineService

```
職責：版本翻頁條件計算、EraShifted 觸發

主要方法：
  calculate_era_completion(era_id) → float
  check_version_advance() → bool
  advance_era(world_id)
```

---

---

# Domain Event 清單

```
MemoryFragmentCollected(player_id, memory_id, fragment_id)
MemoryCompleted(player_id, memory_id, quality)
MemoryCrystalCreated(player_id, crystal_id)

FootprintGained(player_id, footprint_id, footprint_type)
FootprintUpgraded(player_id, footprint_id, new_quality)

ResonanceTriggered(resonance_id, player_id, resonance_type)
FinalResonanceUnlocked(player_id)

SettlementMemoryChanged(settlement_id, old_val, new_val)
SettlementUpgraded(settlement_id, from_level, to_level)
SpecterSpawned(settlement_id, specter_id, specter_type)
SpecterResolved(settlement_id, specter_id, resolver_player_id)
OblivionRising(settlement_id, oblivion_rate)

EraKeyEventWitnessed(era_id, event_id, player_id)
EraCompleted(era_id, completion_rate)
EraShifted(from_era_id, to_era_id)

OralMemoryLost(npc_id, lost_memory_ids[])
NpcElderExpired(npc_id, settlement_id)

CareerRankUp(player_id, career_track, new_rank)
CrossCareerTitleUnlocked(player_id, title)

PlayerEnteredHistoryLayer(player_id, era_id, site_id)
PlayerLeftHistoryLayer(player_id, era_id, site_id)
```

---

---

# Repository 對應

```
WorldRepository        → /daemon/world_d.c
EraRepository          → /daemon/era_d.c
SiteRepository         → /daemon/site_d.c  （載入 /area/sites/*.c）
SettlementRepository   → /daemon/settlement_d.c （載入 yaml）
MemoryRepository       → /daemon/memory_d.c （載入 /data/memories/*.yaml）
FootprintRepository    → 存在 Player save 欄位內
CareerRepository       → 存在 Player save 欄位內
FactionRepository      → /daemon/faction_d.c
NpcRepository          → /daemon/npc_d.c
```

---

---

# YAML 資料目錄結構

```
/data/
├── worlds/
│   └── formosa.yaml
├── eras/
│   ├── v0_1.yaml
│   ├── v0_2.yaml
│   ├── v1_0.yaml
│   ├── v1_2.yaml
│   ├── v1_5.yaml
│   ├── v1_9.yaml
│   ├── v2_0.yaml
│   ├── v2_1.yaml
│   ├── v2_2.yaml
│   ├── v3_0.yaml
│   ├── v3_1.yaml
│   ├── v3_2.yaml
│   └── v4_0.yaml
├── settlements/
│   ├── minxiong.yaml
│   ├── chiayi_city.yaml
│   ├── tainan.yaml
│   ├── taipei.yaml
│   └── ...（368 + 22 筆）
├── memories/
│   ├── xilaya_river.yaml
│   ├── gu_xianrong_gate.yaml
│   ├── wushe_dakis_loom.yaml
│   └── ...
├── resonances/
│   ├── janan_plain.yaml
│   ├── mountain_civilization.yaml
│   └── ...
├── factions/
│   ├── mazu_network.yaml
│   ├── lin_family.yaml
│   └── ...
└── npcs/
    ├── matagi_oral.yaml
    ├── taha_xingang.yaml
    └── ...
```

---

---

# 與 world.c / center_world.c 的關係

現有的 `world.c` 實作的是 **2D Minecraft 空間引擎**：

```
world.c 管理：
  blocks mapping       → 2D 地塊
  player_pos mapping   → 玩家座標
  npc_pos mapping      → NPC 座標
  import_map()         → Emoji 地圖匯入
  dig/place/move       → 空間操作 API
```

本 Domain 模型的 Settlement / Site 是**語意層**，不是座標層。

兩者的關係：

```
Settlement / Site
       ↓ 指向
  world.c 的某個 World 物件
       ↓ 持有
  blocks / player_pos 等空間資料
```

建議新增 `query_domain_id()` 方法：

```c
// 在 world.c 加入
string domain_id;   // 對應 Settlement 或 Site 的 id
void set_domain_id(string id) { domain_id = id; }
string query_domain_id() { return domain_id; }
```

`center_world.c` 繼承 `world.c`，對應的 Domain 物件是：

```yaml
site:
  id: "center_world_plaza"
  name: "中央創界・晶石廣場"
  era_id: null                  # 非歷史層，現代空間
  settlement_id: null           # 非聚落，遊戲內設施
```

---

---

# 下一步文件

本文件完成後，建議依序撰寫：

```
03_world_engine.md    → world.c 擴充方案，Site 與 Settlement 的空間繫結
04_event_system.md    → Event Bus 實作，LPC call_out vs daemon 訂閱
05_data_storage.md    → YAML ↔ LPC mapping，save_object 與 yaml 混用策略
```

---

## 文件與 Canon 對應表

| Domain 物件 | Canon 來源 |
|---|---|
| World, Era | timeline.md |
| Site | historical_events.md |
| Settlement, SixDimensions | settlement_and_population_system.md |
| Memory, MemoryCrystal, Resonance | memory_system.md |
| Oblivion, Specter | memory_system.md（失源者章節）|
| Footprint, FootprintAtlas | footprint_system.md |
| Career | profession_system.md |
| Faction | factions.md |
| NPC | historical_events.md（關鍵NPC）|
| Player | novel_bible.md（主角定位）|
