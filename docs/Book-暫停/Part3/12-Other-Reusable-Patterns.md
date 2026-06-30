# 13-Other-Reusable-Patterns.md

# 第 12 章　其他可重用模式

除了前面介紹的核心循環與機制之外，FSE 還發展出幾個重要的可重用模式，幫助我們更有效地建造世界。

---

### 模式一：Entropy（熵值）管理

**目的**：維持探索的張力，避免理解一勞永逸。

**核心想法**：
- 理解不是永久的資產，而是需要持續維護的狀態
- 如果 Actor 長時間不與某個情境互動，相關的理解會逐漸衰退
- 這迫使玩家「重返」世界，產生新的 Discovery

**實作方式**：
- `progress_manager.c` 中的 Entropy 機制
- 定期輕微增加熵值
- 當熵值過高時觸發 Confusion 或 Specter（混亂實體）

---

### 模式二：Virtual Object（虛擬物件）

**目的**：實現高度資料驅動，減少硬編碼。

**核心想法**：
- 大部分 Node、Monster、Room 不需要寫成獨立的 `.c` 檔案
- 透過 `compile_object()` + YAML 動態生成虛擬物件
- 內容創作者可以主要透過 YAML 來定義世界

**目前實作**：
- `virtual_object.c`
- `node.c` 的虛擬路徑支援
- 未來可擴展到 Monster、Item、Dialogue 等

---

### 模式三：Event-Driven Causality（事件驅動因果）

**目的**：解耦系統，維持世界因果的靈活性。

**核心元件**：
- `event_bus.c`：通用事件總線
- 各服務透過訂閱 / 發布事件來互動
- 讓 Discovery、Progress、Reveal、Entropy 等機制自然連動

**優點**：
- 容易新增新機制
- 避免緊耦合
- 世界因果關係清晰可追蹤

---

### 模式四：Confusion as Feature

**目的**：把困惑從負面體驗轉化為理解的催化劑。

**實作重點**：
- 在行動失敗時明確記錄 Confusion 狀態
- 根據先前 Confusion 來調整後續 Discovery 的難度或提示
- 把「困惑 → 領悟」的過程變得有敘事張力

---

### 這些模式的共同精神

所有模式都服務於同一個目標：

**讓理解在世界中自然地、反覆地發生。**

它們不是孤立的技術功能，而是 FSE 哲學在不同層面的具體體現。

---

**本章總結**

FSE 的強大之處，不在於單一功能，而在於這些模式之間的相互配合。

當 Entropy、Virtual Object、EventBus、Reveal Layer、Discovery Loop 等機制共同運作時，一個世界就真正「活」了起來。

---

**下一章預告**  
Part 4　世界實例

從下一章開始，我們將進入具體的 Adventure 案例，首先是 Programming World。

