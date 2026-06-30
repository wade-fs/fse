# 03-Four-Core-Concepts.md

# 第 2 章　四個核心概念

FSE 只定義四個最小的核心概念。它們在所有 Adventure 中都存在，只是表現形式不同。

這四個概念構成 FSE 的最小骨架：

- **World**
- **Actor**
- **Discovery**
- **Path**

---

### World（世界）

**定義**：知識內嵌其中的情境容器。它有因果、有時間、有規則，但它不開口說話。它只是讓事情發生。

World 不是背景。**World 本身就是老師。**

**必要條件**：
- 因果必須成立（行動要有真實後果）
- 知識必須內嵌於情境，而不是寫在說明文字裡
- 允許重複進入（第一次不懂，第三次可能就懂了）

**例子**：
- Programming World：程式執行的世界（無窮迴圈會真的卡死電腦）
- Prehistoric Civilization：三疊紀的生態世界（氧氣濃度、風向、氣味都是真實規則）
- Formosa：台灣歷史層疊的世界（時間不是線性的，而是多層共存）

---

### Actor（行動者）

**定義**：在 World 中移動、行動、提問的存在。

Actor 不是被動的學生。**Actor 是用行動提問的人。**

**核心行為**：
- Move（移動）
- Observe（觀察）
- Act（行動）
- Fail（承受後果）
- Try（帶著記憶再試一次）

**成長不是等級**，而是**理解深度的增加**。

**例子**：
- Programming World 中的學習者：透過寫程式、失敗、修正來成長
- Prehistoric Civilization 中的觀察者：靠感官訊號在危險環境中求生
- Formosa 中的陳進：用 island_memory 感知歷史層

---

### Discovery（發現／領悟）

**定義**：世界觀改變的那一刻。它不是獎勵，也不是解鎖通知，而是玩家內心說「原來如此」的那一秒。

**特性**：
- 不能被直接教（說出來的 Discovery 不是真正的 Discovery）
- 必須由行動引發
- 幾乎總是經過困惑（Confusion）這個必要階段
- 會永久改變後續的感知與行動

**例子**：
- Programming World：「原來迴圈需要終止條件」
- Prehistoric Civilization：「暴龍主要靠嗅覺，而不是視覺」
- Formosa：「有些歷史無法阻止，見證本身就是一種行動」

---

### Path（路徑）

**定義**：因為 Discovery 而開啟的新可能。它不是關卡解鎖，而是理解深化後，世界對 Actor 展現出的新面貌。

**特性**：
- 由 Discovery 驅動，而不是等級驅動
- 是雙向的（新情境帶來新 Discovery）
- 可以是新的地點、新的視角、新的互動方式

**例子**：
- Programming World：解鎖「迴圈終止」後，工具箱出現新的積木
- Prehistoric Civilization：理解風向後，可以安全進入原本危險的區域
- Formosa：累積足夠踏印後，看見歷史層的入口

---

### 四者之間的關係
```
World
↓ (提供情境與因果)
Actor 採取行動
↓
產生 Consequence（可能帶來 Confusion）
↓
觸發 Discovery（理解發生）
↓
開啟新的 Path (因理解而開啟的新可能)
↓
回到 World（以新的視角繼續探索）
```

這是一個**循環**，而不是線性流程。  
真正的理解世界，就是不斷走完這個循環。

---

**下一章預告**  
第 3 章　世界模型

我們將更具體地定義世界的狀態結構、行動與後果方程式，以及 Confusion 和 Entropy 在其中的角色。

