# 12-Progressive-Unlock-and-Path.md

# 第 11 章　Progressive Unlock 與 Path

Discovery 不只是獲得新知識。  
它更重要的是**開啟新的可能**。

這就是 Progressive Unlock 與 Path 的核心意義。

---

### Path 的定義

**Path 是因為 Discovery 而開啟的新可能。**

它不是傳統意義上的「關卡解鎖」，而是：

> **理解深化後，世界對 Actor 展現出的新面貌。**

---

### Progressive Unlock 的設計原則

1. **由 Discovery 驅動，而非等級驅動**
   - 不是「達到 10 級就解鎖」
   - 而是「真正理解某個概念後，新的路徑自然出現」

2. **漸進而非一次性**
   - 理解一點，就 Reveal 一點
   - 避免認知過載

3. **雙向強化**
   - 新 Path 帶來新情境
   - 新情境帶來新 Discovery
   - 形成正向循環

4. **可感知的改變**
   - 玩家應該能明顯感覺到「世界因為我的理解而不同了」

---

### Path 的多種形式

- **新的地點 / 區域**（空間 Path）
- **新的互動方式 / 工具**（能力 Path）
- **新的視角 / 歷史層**（感知 Path）
- **新的挑戰 / 問題**（探索 Path）
- **新的 NPC 反應 / 對話**（社會 Path）

---

### 在 FSE 中的實作

- `reveal_layer.c`：條件式內容顯現
- `progress_manager.c`：Stage 推進與 Path 開啟
- `factor_service.c`：Discovery 作為解鎖條件
- `node.yaml` 中的 `reveal_layers` 配置

這些機制讓 Progressive Unlock 成為可宣告、可組合的強大功能。

---

### Progressive Unlock 的設計價值

1. **尊重理解的自然節奏**
2. **增加長期探索動機**
3. **提升重玩價值**
4. **讓世界感覺「活著」**

當玩家感覺「我懂得越多，世界就對我展現越多」時，FSE 就成功了一半。

---

**本章總結**

Path 不是獎勵。  
Path 是理解的自然延伸。

Progressive Unlock 的本質是：

**讓世界隨著 Actor 的理解而逐漸展開。**

這才是真正的「成長」。

---

**下一章預告**  
第 12 章　其他可重用模式

我們將討論 Entropy 管理、Virtual Object、Event-Driven Causality 等其他重要模式。

