# 11-Reveal-as-Design-Primitive.md

# 第 10 章　Reveal 作為設計原語

Reveal Layer 是 FSE 中最重要也最優雅的設計機制之一。

它幾乎可以被視為 FSE 的「設計原語」（Design Primitive）。

---

### Reveal 的本質

**Reveal 不是「條件式顯示內容」。**

它的真正意義是：

> **理解改變感知**

當 Actor 獲得新的 Discovery 時，世界本身沒有改變，但 Actor 能「看見」的東西卻不一樣了。

---

### Reveal Layer 的核心原則

1. **世界一直都在，只是你之前看不見**
   - 歷史層、隱藏路徑、生態細節、程式概念……它們一直存在。
   - 只是 Actor 的理解深度不夠，無法感知。

2. **Reveal 由 Discovery 驅動**
   - 不是由等級、任務完成度或時間驅動。
   - 而是由真正的理解驅動。

3. **漸進式顯現**
   - 不一次性把所有東西倒給玩家。
   - 而是隨著理解的深化，逐步打開新的視野。

---

### Reveal 的多種表現形式

- **Programming World**：解鎖新概念後，Blockly 工具箱出現新的積木類別
- **Prehistoric Civilization**：理解風向後，能看到原本看不見的氣味軌跡與安全路徑
- **Formosa**：累積足夠踏印後，看見歷史層的殘影與隱藏事件

同一機制，不同呈現。

---

### Reveal 在 Runtime 中的實作

目前主要透過以下元件實現：

- `reveal_layer.c`：條件檢查引擎
- `factor_service.c`：提供 Discovery 狀態
- `progress_manager.c`：Stage 推進時的全局 Reveal
- `node.yaml` 中的 `reveal_layers` 配置

這讓 Reveal 成為一個可宣告、可組合的強大機制。

---

### Reveal 的設計價值

1. **尊重玩家的理解過程**  
   不強迫玩家接受過早的資訊。

2. **創造「啊哈！」時刻**  
   當玩家突然看見新東西時，會產生強烈的滿足感。

3. **增加重玩價值**  
   同一個世界，第二次玩會看到完全不同的內容。

4. **支援多種 Adventure**  
   無論是程式學習還是歷史探索，都能用同一套機制。

---

**本章總結**

Reveal Layer 不只是一個技術功能。

它是 FSE 對「理解如何發生」這件事最深刻的體現之一：

**世界不會因為你到來而改變。**  
**世界會因為你的理解而顯現。**

---

**下一章預告**  
第 11 章　Progressive Unlock 與 Path

我們將討論 Discovery 如何自然地開啟新的可能，以及 Progressive Unlock 的設計原則。

