# 劉家勢力任務設計：劉家地契委託 (Liu Family Deed Recovery)

本任務為《源流福爾摩沙》第三個勢力任務，主要引導玩家前往「民雄鬼屋」（劉家古厝）進行探索，深化與在地仕紳「劉家」勢力的互動與聲望提升。

---

## 🎯 任務基本資訊 (Quest Metadata)

- **任務 ID**: `liu_deed_recovery`
- **任務名稱**: 
  - `zh-TW`: "劉家地契委託"
  - `en`: "Liu Family Deed Recovery"
- **發接任務 NPC**: 劉家後人 (Liu Clan Descendant) at `ghost_house` (民雄鬼屋 / 劉家洋樓)
- **接取限制 (Prerequisites)**:
  - 等級需求：`7` 級以上。
  - 前置任務：需完成 `sugar_railway_repair` (糖鐵搶修委託)。
- **任務目標 (Goal)**:
  - 收集 3 張「劉家地契」 (`liu_deed`)。
- **任務獎勵 (Reward)**:
  - 經驗值 (EXP): `700` 點。
  - 金幣 (Gold): `300` 銅幣。
  - 勢力聲望: 「劉家」聲望增加 `30` 點。
  - 獎勵道具: 「劉家家徽指環」 (`/item/liu_family_ring.c`)。

---

## 🗺️ 任務執行流程 (Gameplay Flow)

1. **接取任務**:
   - 玩家達到等級 7 且完成前置任務後，在民雄鬼屋向劉家後人接取任務。
2. **尋找隱密地磚夾層**:
   - 單人時在洋樓廢墟中無法發現隱蔽的夾層。
   - 玩家必須與同伴組隊（擁有 Leader/組隊狀態），且身上有此任務時，在民雄鬼屋 look 會觸發條件顯現：
     > **【線索顯現】** 你們在倒塌的書架後方摸索，發現了一個隱密的地磚夾層，裡面似乎藏有一些泛黃的文書契約。
3. **搜集與完成**:
   - 取得 3 張「劉家地契」。
   - 返回交付給劉家後人，扣除地契，獲得指環、劉家聲望與經驗值獎勵。

---

## 🛠️ 技術實作配置對照

### 1. 任務 YAML 定義 `/world/quests/liu_deed_recovery.yaml`
```yaml
id: "liu_deed_recovery"
name:
  zh-TW: "劉家地契委託"
  en: "Liu Family Deed Recovery"
desc:
  zh-TW: "劉家古厝的後人表示洋樓深處有一處隱蔽的夾層，存放著失落的家族房地契約。你必須與同伴組隊前往民雄鬼屋，尋回 3 張散落的地契。"
  en: "The descendents of the Liu family say a hidden compartment in the mansion contains lost property deeds. Form a party, head to the Ghost House, and retrieve 3 lost deeds."
level: 7
prereq_quests:
  - sugar_railway_repair
goal:
  type: "item"
  target: "liu_deed"
  count: 3
reward:
  exp: 700
  gold: 300
  item: "/item/liu_family_ring.c"
  faction:
    id: "liu_clan"
    reputation: 30
```
