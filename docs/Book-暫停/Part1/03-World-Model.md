# 04-World-Model.md

# 第 3 章　世界模型

世界不是靜態的地圖，而是一個動態的因果系統。

本章定義 FSE 中「世界」的狀態結構，以及行動如何在其中產生後果。

---

### 世界狀態定義（World State Tuple）

在 FSE 中，任何一個「情境（Situation）」都可以用以下三維元組來描述：

$$
S = (Site\_ID,\ Global\_Progress\_Stage,\ Active\_World\_Events)
$$

#### 1. 空間維度（Site_ID）
代表 Actor 當前所在的物理或概念地標。

**例子**：
- Programming World：`infinite_loop_swamp`（無窮迴圈沼澤）
- Prehistoric Civilization：`triassic_plains`（三疊紀荒原）
- Formosa：`minxiong_old_station`（民雄老車站）

#### 2. 時間 / 進度維度（Global_Progress_Stage）
代表世界整體或 Actor 個人的宏觀理解階段。

**例子**：
- Programming World：`stage_1_sequence` → `stage_2_loop`
- Formosa：`novice` → `survivor` → `witness`

#### 3. 環境維度（Active_World_Events）
代表當前正在影響世界的動態事件或危機。

**例子**：
- Programming World：`memory_leak_warning`（記憶體洩漏）
- Prehistoric Civilization：`predator_nearby`、`oxygen_critical`
- Formosa：`typhoon_warning`、`oblivion_rising`

---

### 行動與後果方程式

Actor 的任何行動（Act）都會對世界狀態 $S$ 產生影響：

$$
Act(Actor\_Context,\ S) \to Consequence
$$

**Consequence 包含兩部分**：
- **Player_Physical_Change**：對 Actor 本身的影響（記憶值下降、狀態改變等）
- **Site_State_Change**：對當前地標或世界的影響（警戒值上升、歷史層顯現等）

---

### Confusion 與 Entropy 的角色

**Confusion（困惑）** 是理解發生前的重要中間狀態。  
它代表 Actor 現有的認知模型與世界現實之間出現斷裂。

**Entropy（熵值）** 是維持世界張力的機制：
- 世界不會靜止不變
- 如果 Actor 長時間不互動，熵值會自然上升
- 導致情境逐漸模糊、記憶衰退、危機出現

這兩個機制共同確保：**理解不是一次性的，而是需要持續探索才能維持**。

---

### 世界模型的核心原則

1. **世界不會因為 Actor 的到來而改變**  
   改變的是 Actor 感知世界的方式（Reveal）。

2. **因果必須真實且可預測**  
   但不一定要立即可理解。

3. **允許失敗與重試**  
   失敗不是懲罰，而是理解的必要養分。

4. **理解具有累積性**  
   一次 Discovery 會永久改變後續的所有互動。

---

這個世界模型是 FSE 所有後續設計（Runtime 服務、Adventure 內容、Reveal 機制）的共同基礎。

---

**下一章預告**  
第 4 章　語言與表現模型

我們將討論世界如何與 Actor 溝通，以及 Reveal Layer 在其中的核心地位。

