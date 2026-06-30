# 新手引導任務設計：老站長的心願 (Old Station Master's Wish)

這是《源流福爾摩沙》玩家進入遊戲後遇到的首個任務，旨在帶領玩家熟悉 Mud 地景探索、獲得踏印以及回報任務的基礎玩法循環。

---

## 🎯 任務基本資訊 (Quest Metadata)

- **任務 ID**: `old_station_master_wish`
- **任務名稱**: 
  - `zh-TW`: "老站長的心願"
  - `en`: "Old Station Master's Wish"
- **發接任務 NPC**: 老站長 (Old Station Master) at `minxiong_old_station` (民雄老車站月台)
- **解鎖條件**:
  - 玩家探訪過民雄鬼屋（獲得鬼屋踏印 `ghost_house_minxiong`），老站長 NPC 便會出現在月台。
- **任務目標 (Goal)**:
  - 前往民雄老街或市集尋找糖鐵遺址，獲得「糖鐵踏印」 (`sugar_railway_minxiong`)。
- **任務獎勵 (Reward)**:
  - 經驗值 (EXP): `150` 點。
  - 金幣 (Gold): `80` 銅幣。
  - 獎勵道具: 「落灰的乘車券」 (`/item/old_station_ticket.c`)。

---

## 🗺️ 任務執行流程 (Gameplay Flow)

1. **觸發與接取**:
   - 玩家抵達民雄老車站月台。如果玩家曾去過民雄鬼屋（擁有鬼屋踏印），老站長將現身。
   - 玩家點擊或輸入指令向老站長詢問「任務」，接取任務。
2. **探索目標**:
   - 玩家移動前往「民雄市集」 (`minxiong_market`) 進行探索。
   - 進入市集時，玩家將自動獲得聚落踏印「糖鐵踏印」 (`sugar_railway_minxiong`)。
3. **回報完成**:
   - 玩家返回民雄老車站月台，向老站長回報任務。
   - 系統扣除或驗證踏印完成度，發放 150 EXP、80 銅幣與乘車券，任務標記為 `completed`。

---

## 🛠️ 技術實作配置對照

### 1. 任務 YAML 定義 `/world/quests/old_station_master_wish.yaml`
```yaml
id: "old_station_master_wish"
name:
  en: "Old Station Master's Wish"
  zh-TW: "老站長的心願"
desc:
  en: "Find the trace of the sugar railway in Minxiong Old Street to get the footprint and report back."
  zh-TW: "在民雄老街尋找糖鐵遺址，獲得「糖鐵踏印」，然後向他報告。"
level: 1
goal:
  type: "footprint"
  target: "sugar_railway_minxiong"
  count: 1
reward:
  exp: 150
  gold: 80
  item: "/item/old_station_ticket.c"
```
