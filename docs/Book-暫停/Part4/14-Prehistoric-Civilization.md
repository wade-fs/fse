# 14-Prehistoric-Civilization.md

# 第 14 章　Prehistoric Civilization

Prehistoric Civilization（PC，史前文明）是 FSE 的第二個重要 Adventure。

它把**環境訊號作為主要 Input Medium**，探索在極端自然環境中「理解如何幫助生存」的主題。

---

### PC 的設計目標

PC 試圖回答一個問題：

**在一個充滿未知危險的世界中，理解如何幫助 Actor 活下去？**

在這裡，知識不是來自書本，而是來自對環境的敏銳觀察與反覆試錯。

---

### PC 中的核心概念映射

| FSE 核心概念 | PC 中的表現形式 |
|---|---|
| World | 三疊紀的危險生態世界 |
| Node | 不同地形與生態地標（荒原、河谷、森林） |
| Actor | 穿越到史前時代的觀察者 |
| Discovery | 對生態規則的領悟（如風向、氣味、視力） |
| Path | 新的安全路徑、觀察技巧、生存策略 |
| Input Medium | 環境訊號（風向、氣味、聲音、震動） |

---

### 案例研究：predator_canyon

理論說明 PC 怎麼運作不如直接看一個完整的 Node。

`predator_canyon`（捕食者峽谷）是目前 PC 裡最成熟的 Discovery Loop 實例，值得逐步拆解。

#### 情境（World）

```yaml
short: "捕食者峽谷"
long: |
  你來到了一片狹窄而深邃的紅砂岩峽谷。
  兩側是近乎垂直的峭壁，遮擋了大部分光線。
  風在峽谷狹窄的通道中呼嘯而過，發出尖銳的口哨聲。
  此地充滿了未知的壓迫感，每一步都必須極為小心。
```

這個房間不直接告訴玩家「這裡危險」，而是用感官描述讓玩家自己感覺到壓迫感。

#### 環境訊號（Sensory Signals）

```yaml
sensory_signals:
  smell: "風中傳來極度腥臭的野獸體味，混雜著腐肉的臭氣。"
  sound: "岩壁上方不時傳來碎石滾落的沙沙聲，伴隨著低沉且帶有威脅的喉音。"
  wind:
    default_msg: "強烈而冰冷的風正從峽谷出口（你前進的方向）直吹向你。"
    discovery:
      set_temp: "observed_wind"
      msg: |
        【 🔍 發現 】風正朝著你吹來（逆風）。
        逆風前進可以使你的氣味不被前方的野獸察覺！
        你可以嘗試在逆風處隱蔽偽裝前進（hide downwind）。
```

注意 `wind` 欄位的結構：玩家第一次 `focus wind` 只會看到 `default_msg`，純粹的環境描述。

只有當玩家主動聚焦、持續觀察，系統才在 `discovery` 區塊給出關鍵線索，並設定 `observed_wind` 暫存狀態。

這正是 Reveal 機制的最小實例：**不是玩家做了什麼就直接告訴答案，而是讓玩家自己先「看見」線索的存在**。

#### 行動與後果（Action & Consequence）

```yaml
interactions:
  - action: "hide"
    target: "downwind"
    prerequisites:
      temp_state: "observed_wind"
    success_msg: |
      你立刻俯下身子，利用逆風的優勢，將全身塗滿紅砂泥土以遮蓋體味，
      藉著岩石陰影匍匐前進。
      那頭巨大的捕食者在上方峭壁聳了聳鼻子，疑惑地往另一個方向走開了。
      💡 成功避開威脅！
    discover_factor: "stealth_camouflage"
    complete_quest: "first_escape"
    fatigue: 15
```

玩家必須先觀察到 `observed_wind`（前置條件），才能執行 `hide downwind`。

成功之後，獲得的不是經驗值，而是 `stealth_camouflage`——一個 Factor（Discovery）。

#### Path 開啟

```yaml
reveal_exits:
  desert:
    dest: "/rooms/desert_canyon/room"
    requires_factor: "stealth_camouflage"
    reveal_msg: |
      避開了掠食者，前方露出了通往未知深處的「荒野峽谷 (desert)」通道！
```

沒有 `stealth_camouflage`，`desert` 這個出口在地圖上根本不存在。

理解深了，世界才展示新的面貌——這就是 Path，不是用等級或鑰匙開的門。

---

### 重要修正：威脅不是用「殺死」解決的

PC 最初的雛形裡，`predator_canyon` 放了一隻 `herrerasaurus`（赫雷拉龍），並沿用了傳統 MUD 的 `monster.c`：`attack` / `defense` / `exp_value` / `combat_round`。

這個設計後來被推翻了，原因記錄在 [ADR-0008](../adr/ADR-0008-No-Combat-System.md)。

簡單說：**捕食者不是用來打的，是用來教會玩家「理解環境」這件事有多重要的存在。**

`predator_canyon` 現在的正確玩法完全不涉及戰鬥：

```
玩家進入 → 感受到壓迫感（smell / sound）
玩家 focus wind → 看到風向線索 → observed_wind
玩家 hide downwind → 成功避開 → discover_factor: stealth_camouflage
新路徑 desert 開啟
```

如果玩家沒有先觀察風向就貿然前進，後果不是「打輸了」，而是被偵測到、受傷、退回上一個 Node、重新觀察——這才是 Confusion → Retry 的真實循環。

---

### PC 的特色設計

#### 1. 環境訊號作為主要輸入

玩家不下達「kill」這類具象指令，而是透過 `focus smell`、`focus wind`、`hide downwind` 等感官與行動指令與世界互動。世界回傳的是需要被解讀的描述，不是數值。

#### 2. 失敗作為理解的催化劑

逆風走錯方向 → 被偵測 → 受傷 → 困惑 → 重試，注意到風向訊號 → 領悟「逆風才能藏匿氣味」。

這比系統直接告訴玩家「要逆風走」有效得多。

#### 3. 漸進式生存階段（Progression）

```yaml
# day_zero.yaml
stage_id: "day_zero"
spawn_node: "/rooms/desert_canyon/room"
requires:
  quests: ["survive_day_zero"]
next: "first_night"
```

PC 用 `progression/` 定義階段性的生存壓力（第零天、第一夜、新手期、生存者），每個階段對應不同的環境挑戰與 Discovery 需求，而非傳統 RPG 的等級制。

#### 4. Entropy 與生存壓力

長時間不探索會增加飢餓、疲勞等熵值，迫使玩家持續與環境互動，維持 Discovery Loop 的張力。

---

### PC 對 FSE 的貢獻

- 驗證**環境訊號作為 Input Medium**的可行性
- 驗證 Reveal 機制可以用在**感官線索的漸進揭露**上（`sensory_signals.*.discovery`），不只是文字描述或地圖路徑
- 證明**威脅性實體**完全可以脫離戰鬥系統，改用「偵測判定 + Discovery 條件」運作
- 深化 Confusion → Discovery 的敘事張力
- 為未來更多自然科學類 Adventure 建立模板

---

### PC 的未來方向

- 把 `herrerasaurus` 等威脅性實體從殘留的 `std/monster.c` 遷移到新的「環境偵測實體」模型（見 ADR-0008）
- 加入更多中生代生態概念（食物鏈、共生、氣候變化）
- 發展感官聚焦與環境解讀的進階系統
- 探索「理解如何幫助群體生存」的社會層面

---

**本章總結**

Prehistoric Civilization 證明 FSE 不只適合程式或歷史主題。

`predator_canyon` 是目前最乾淨的證據：只要情境夠豐富、因果夠清楚、Reveal 機制設計得當，連「危險」本身都不需要透過戰鬥來表達。

只要有豐富的情境、清晰的因果、以及足夠的探索空間，任何領域都能成為「讓理解自然發生」的世界。

---

**下一章預告**
第 15 章　Formosa 與未來世界

我們將討論如何把 Formosa 作為大型 Adventure 導入，以及 FSE 的長期願景。
