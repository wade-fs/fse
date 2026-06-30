# 07-Discovery-First-Progression.md

# 第 6 章　Discovery-First Progression

玩家不是因為完成 Quest 而成長。  
玩家是因為 **Discovery** 而成長。

這是 FSE 進度系統最核心的設計哲學。

---

### 傳統 Progression vs FSE Progression

**傳統遊戲常見模式**：

Quest → 完成 → Reward → Level Up

**FSE 的模式**：

Explore → Act → Consequence → Confusion → Discovery → Understanding → New Path

---

### 為什麼要 Discovery-First？

1. **理解不是累積的，而是突變的**  
   真正的理解往往發生在某一瞬間，而不是線性增加。

2. **避免空洞的任務導向**  
   如果進度只看任務數量，玩家會為了完成而完成，失去探索的本質。

3. **讓失敗變得有意義**  
   失敗不再是懲罰，而是通往 Discovery 的必要階段。

4. **進度真正反映理解深度**  
   玩家真正「懂了」多少，比他完成了多少任務更重要。

---

### FSE 的 Progress 設計

在 FSE 中，**Progress 是 Discovery 的累積**，而不是任務計數。

**主要組成**：
- 已解鎖的 **Factors / Discoveries**
- 當前 **Stage**（理解階段）
- 開啟的 **Paths**（新可能）
- 累積的 **Understanding Depth**（可透過 Entropy 機制動態變化）

**Progress Manager** 的核心職責不是發放獎勵，而是：
- 檢測 Discovery 是否足以觸發 Stage 推進
- 管理 Entropy（理解會隨時間衰退）
- 透過 Reveal Layer 讓新的 Path 顯現

---

### Discovery 驅動的優點

- **自然流暢**：玩家感覺自己是在「理解世界」，而不是「刷任務」。
- **可重複體驗**：同一個 Node 可以因為不同 Discovery 而產生不同體驗。
- **長期動機**：因為每次 Discovery 都會帶來新的好奇（New Curiosity），形成正向循環。
- **適合多種 Adventure**：無論是程式學習、歷史探索還是生態觀察，都能套用同一套機制。

---

**本章總結**

Progression 不應該是「我完成了多少」，而應該是「我理解了多少」。

把 Discovery 放在進度系統的核心，不僅更符合人類理解的自然過程，也讓 FSE 的世界真正成為「會教會人的世界」。

---

**下一章預告**  
第 7 章　Curiosity Before Knowledge

我們將討論 FSE 最重要也最根本的原則之一：先建立值得探索的世界，再談知識的傳遞。

