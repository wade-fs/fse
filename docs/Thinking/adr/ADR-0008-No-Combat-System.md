# ADR-0008: PC 不使用傳統戰鬥系統

## 狀態

已採納（部分遺留程式碼尚待清理，見「待辦」）

## 背景

Prehistoric Civilization（PC）最初的雛形沿用了傳統 MUD 的怪物系統：

```
mudlib/std/monster.c
  attack / defense / exp_value / combat_interval
  combat_round()    每回合互相攻擊
  engage()          進入戰鬥
  on_death()        死亡給經驗值、排程重生
```

`predator_canyon/room.yaml` 一度也放了：

```yaml
monsters:
  - id: "/monsters/herrerasaurus/monster"
    count: 1
    respawn: true
```

意圖是讓玩家「打贏赫雷拉龍就能通過峽谷」。

## 問題

這個設計與 FSE 的核心理念直接衝突。

Concept.md 對 DW（PC 的前身命名）的定義很明確：

> 沒有武器。沒有超能力。只有觀察和判斷。
> Fail → 被發現、被攻擊
> Try → 帶著新知識再試

如果玩家可以靠數值壓制（等級夠高、裝備夠好）直接打死赫雷拉龍通過，那麼：

1. **Discovery 變成可選項，不是必經之路**
   玩家不需要理解風向、不需要學會 `stealth_camouflage`，只要練到夠高的攻擊力就能蠻幹過關。這違背「Path 由 Discovery 驅動，不是由等級驅動」的設計原則。

2. **EXP 系統重新引入了等級制**
   `monster.yaml` 的 `exp_value` 本質上是在重建傳統 RPG 的成長曲線。FSE 的 Actor 成長定義是「理解深度」，不是數字堆疊。`std/monster.c` 的 `on_death()` 開發者自己也在註解裡承認了這個矛盾：

   ```c
   // 在新 FSE 精神下，戰鬥結束僅發送事件，或扣減疲勞，不使用傳統 EXP
   killer->add_fatigue(10);
   ```

3. **戰鬥掩蓋了 Confusion**
   被打死和「困惑」是兩種完全不同的玩家心理狀態。被打死觸發的是「我數值不夠、要去刷裝備」，不是「我哪裡理解錯了」。FSE 需要的是後者。

## 決定

**PC 中所有具威脅性的生物，一律不使用 `std/monster.c` 的攻擊/防禦/經驗值模型。**

改用「環境偵測實體」模型：威脅性實體本身不主動攻擊，而是依照玩家當前的 Discovery 狀態，判定「玩家有沒有被偵測到」。

```
偵測到 → Consequence（扣血、退回前一個 Node、trigger_confusion）
沒偵測到 → 玩家安全通過，或可以進一步觀察、互動
```

`predator_canyon` 目前在 `interactions` 區塊已經實作了這個模型的正確版本（`hide downwind` → `discover_factor: stealth_camouflage`），證明這個方向是可行的，且不需要任何戰鬥回合邏輯。

## 具體做法

1. `monster.yaml` 改名為 `creature.yaml`，移除 `attack` / `defense` / `exp_value` / `combat_interval`，新增：

```yaml
creature_id: "herrerasaurus"
name: "赫雷拉龍"
detection:
  base_chance: 0.8
  reduced_by_factor:
    stealth_camouflage: 0.05   # 擁有此 Discovery 後，偵測機率大幅下降
on_detected:
  hp_change: -30
  trigger_confusion: "caught_by_predator"
  retreat_to_previous_node: true
```

2. `std/monster.c` 整份移除，新增 `std/creature.c`，只負責偵測判定，不含任何 `combat_round` 邏輯。

3. `triassic_plains` 的 `proto_chicken` 重新定位：如果牠的存在沒有對應的 Discovery 用途（例如教玩家觀察生物對氣味的反應），應移除；如果有，補上對應的 `creature.yaml` 定義，同樣不走戰鬥模型。

## 影響範圍

```
mudlib/std/monster.c                          → 刪除，改寫為 creature.c
mudlib/content/monsters/proto_chicken/         → 重新定位或移除
mudlib/content/rooms/predator_canyon/room.yaml → monsters: 欄位改為 creatures:
mudlib/cmds/player/(若有 kill.c / combat 指令)  → 移除
```

## 待辦

- [x] 撰寫 `std/presence.c` (原規劃 `creature.c`)
- [x] 將所有掠食者從 monster 遷移至 presence 結構與 yaml
- [x] 確認並移除無教學用途的裝飾性生物 `proto_chicken`
- [x] 移除或停用所有戰鬥與傷害指令
- [x] 更新 `mudlib/tests/test_pc_survival.c` 涵蓋非戰鬥偵測/避讓流程

## 補充：即死危害（Instakill）與 FSE 試錯學習之協調

在 `presence.yaml` 配置中，某些極端掠食者（例如霸王龍 `tyrannosaurus`）的偵測失敗後果被設定為 `hp_change: -100`（即死）。
這與 FSE 「失敗是用來學習（Fail to Learn）」的精神並不衝突，其背後的協調機制如下：
1. **物理合理性**：在此類壓倒性威脅面前，正面暴露等同於物理上的絕對毀滅。
2. **仍會觸發困惑**：即使一擊必殺，Presence 偵測失敗仍會向玩家標記對應的 `Confusion` 狀態（如 `caught_by_predator`）。
3. **墓地重置與反思**：配合資料驅動的階段性墓地復活（Stage Respawn），玩家在當前地質時代的起點重生並帶有該「困惑」提示。這促使玩家停止採用「強行試錯扣血」的蠻幹打法，轉而尋求徹底規避偵測（例如透過靜止偽裝 `freeze` 獲得 `tyrex_senses`）的正確發現路徑。

## 相關文件

- `docs/Part4/14-Prehistoric-Civilization.md`（案例研究：predator_canyon）
- `docs/Part3/09-Discovery-Loop.md`（Confusion 在循環中的位置）
- Concept.md（DW 的原始定義：「沒有武器，只有觀察和判斷」）
