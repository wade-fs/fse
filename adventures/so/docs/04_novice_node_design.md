# The Novice Node Design (最小集合設計白皮書)
# 最小集合設計：靜慮崖 (Meditation Cliff)

此文件詳細記錄了《蜀山遊記》第一個階段「初入山門（Novice Stage）」的起點關卡——「靜慮崖」的觀察、預測、評估與演變機制。

---

## 一、 場景情境與感官觀察 (Observe)

### 1. 物理與心境描述 (Scene Description)
修行者置身於一處孤懸的峭壁。此處松濤如海，冷風刺骨。地表上留有一塊平整的青石（打坐處）。此時風勢極強，隱隱有靈氣流動。

### 2. 玩家能收集的 Evidence (Observations)
玩家透過觀察（focus 或者是 sensory_signals）可以收集到以下訊號：
* `howling_wind` (狂風呼嘯的風聲)
* `physical_tension` (寒風吹拂下，身體產生的緊繃與寒冷感)
* `unstable_spiritual_current` (若隱若現的混亂靈氣暗流)

---

## 二、 玩家的預測與行動 (Predict)

玩家的第一步是嘗試與靈氣溝通。他的 Action 就是他的認知預測。

* **行動 A: 強行納氣 (Force Meditate)**
  * 指令: `meditate` 或者是 `gather_qi`。
  * 玩家隱含的預測: *「我只要用力呼吸、強行引導，就能將這股不穩定的靈氣吸入體內。」*

* **行動 B: 數息靜慮 (Relaxed Breathing)**
  * 指令: `observe_breath` 或者是 `relax`。
  * 玩家隱含的預測: *「我需要先放鬆身體、屏除執念，與環境的風勢與靈氣同頻。」*

---

## 三、 規則評估 (Evaluate)

Reality Resolver 針對這個 Challenge (`first_meditation`) 比對 `spiritual.resonance` 法則：

### 1. 完全理解 (Understanding)
* **條件**：玩家執行 `relax` 或 `observe_breath`，且已感知到 `physical_tension` 與 `howling_wind` 這兩個 Evidence。
* **判定**：
  * 對齊客觀規律：放鬆身體能消解心境障礙，專注呼吸能平息妄念，從而與狂風中的靈氣共鳴。
  * 結果：`Understanding: stillness_resonance`。

### 2. 局部偏誤 (Misunderstanding)
* **條件**：玩家執行 `meditate`，但已經感知到 `physical_tension`（知道身體很冷很緊繃，但仍試圖強行引導）。
* **判定**：
  * 玩家知道身體狀態，但忽略了「強求」會導致氣血逆流的規則。
  * 結果：`Misunderstanding: body_tension_ignored`（忽略身心共鳴的盲點）。

### 3. 完全成見/迷思 (Misconception)
* **條件**：玩家直接執行 `meditate` 或 `gather_qi`，且完全沒有去觀察（observations 列表為空，或者完全忽略了風速與緊繃感）。
* **判定**：
  * 玩家抱持著傳統網遊「打坐就能增加經驗值/法力」的成見，無視環境規律強求修行。
  * 結果：`Misconception: power_by_force` (以為力量可以強求)。

---

## 四、 世界演變 (Evolve)

根據 Evaluate 評估出的認知狀態，世界（Spiritual Reality）給予不同的因果演化回饋：

### 1. 處於 Understanding 時：
* **心境與記憶更新**：
  * 解鎖 Factor `stillness_resonance`。
  * 玩家 observations 注入 `spiritual_current_aligned`。
* **Reveal Update**：
  * 靜慮崖原本模糊的描述改變。玩家現在能「看見」原本看不見的「靈氣潮汐在松濤間有規律地流動 (spiritual_currents)」。
* **World Consequence**：
  * 靈力值大漲 (`spiritual_energy +20`)。
  * 狂風在玩家的感知中變得溫和。

### 2. 處於 Misunderstanding 時：
* **心境更新**：
  * 玩家臨時狀態寫入 `failure_history/body_tension_ignored`。
* **World Consequence**：
  * 靈力不增反減 (`spiritual_energy -5`)，氣血微動。
  * **產生新的 Observation** 訊號：`chest_tightness` (胸口發悶)，作為下一次 Observe 的線索。

### 3. 處於 Misconception 時：
* **心境更新**：
  * 玩家臨時狀態寫入 `failure_history/power_by_force`。
* **World Consequence**：
  * 業力大漲 (`karma +5`)。走火入魔邊緣。
  * 靜慮崖上狂風暴作，玩家被強勁的風勢吹倒，受到精神反噬。
  * **產生新的 Observation** 訊號：`mind_chaos` (神識混亂)。
  * **引發 Confusion**：*「大氣粗暴，強求無功。你需要先平息你的呼吸與妄念。也許你該先觀察你的身體狀態 (focus body)？」*
