# 廟委勢力任務設計：廟委鎮煞委託 (Temple Council Exorcism)

這是《源流福爾摩沙》首個勢力/派系任務，也是引導玩家體驗組隊探險、地標 Reveal Layer (條件顯示層) 機制，以及累積陣營聲望的示範模板。

---

## 🎯 任務基本資訊 (Quest Metadata)

- **任務 ID**: `temple_exorcism`
- **任務名稱**: 
  - `zh-TW`: "廟委鎮煞委託"
  - `en`: "Temple Council Exorcism"
- **發接任務 NPC**: 廟祝阿榮伯 (Keeper Ah-Rong) at `dashiye_temple` (大士爺廟)
- **接取限制 (Prerequisites)**:
  - 等級需求：`5` 級以上。
  - 前置任務：需完成 `old_station_master_wish` (老站長的心願)。
- **任務目標 (Goal)**:
  - 與同伴組隊進入大士爺廟，取得 3 枚散落在地下陣法的「鎮符石」 (`ward_stone`)。
- **任務獎勵 (Reward)**:
  - 經驗值 (EXP): `500` 點。
  - 金幣 (Gold): `200` 銅幣。
  - 勢力聲望: 「大士爺廟廟委會」聲望增加 `30` 點。
  - 獎勵道具: 「大士爺護符」 (`/item/temple_amulet.c`)。

---

## 🗺️ 任務執行流程 (Gameplay Flow)

1. **接取任務**:
   - 玩家達到等級 5 且完成新手任務後，在大士爺廟向廟祝阿榮伯接取任務。
2. **尋找地下陣法秘境**:
   - 單人時在大士爺廟無法察覺任何端倪。
   - 玩家必須與同伴組隊（擁有 Leader/組隊狀態），且身上有此任務時，在大士爺廟 look 會觸發條件顯現：
     > **【線索顯現】** 你與隊友們互換了個眼神，都察覺供桌下方有塊石磚輕微鬆動，縫隙中透出一絲陰冷、詭異的青光。 (輸入 `enter 地下陣法` 即可進入秘境)
3. **搜集與完成**:
   - 進入地下陣法搜集 3 枚「鎮符石」。
   - 返回大士爺廟交付給廟祝，扣除符石，獲得大士爺護符、廟委聲望與經驗值獎勵。

---

## 🛠️ 技術實作配置對照

### 1. 任務 YAML 定義 `/world/quests/temple_exorcism.yaml`
```yaml
id: "temple_exorcism"
name:
  zh-TW: "廟委鎮煞委託"
  en: "Temple Council Exorcism"
desc:
  zh-TW: "廟祝說大士爺廟底下的鎮煞石陣出現了裂縫，三尊走陰的怨靈趁機竄出。你必須與同伴組隊前往地下陣法，尋得三枚散落的「鎮符石」後，回報廟祝。"
  en: "The temple keeper says the ward stone formation beneath the temple has cracked. Three vengeful spirits have escaped into the underground chamber. Form a party, retrieve the three Ward Stones, and report back."
level: 5
prereq_quests:
  - old_station_master_wish
goal:
  type: "item"
  target: "ward_stone"
  count: 3
reward:
  exp: 500
  gold: 200
  item: "/item/temple_amulet.c"
  faction:
    id: "dashiye_temple_council"
    reputation: 30
```
