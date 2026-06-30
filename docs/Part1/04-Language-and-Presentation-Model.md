# 05-Language-and-Presentation-Model.md

# 第 4 章　語言與表現模型

世界如何與 Actor 溝通？

本章討論 FSE 中「語言」的抽象概念，以及世界如何透過表現（Presentation）讓 Actor 感知與互動。

---

### 世界溝通媒介（Input Medium）

在 FSE 通用層，「語言」被抽象化為 **Input Medium（輸入媒介）**。

不同 Adventure 可以選擇完全不同的溝通方式，但底層驅動邏輯一致：

- **Programming World**：程式語言 / 抽象語法樹（AST）
- **Prehistoric Civilization**：環境訊號（風向、氣味、聲音、震動）
- **Formosa**：自然語言 + 歷史語境（對話、書寫、踏印）

**關鍵原則**：Runtime 不應該預設某種特定媒介。它只知道 Actor 做了什麼，以及世界如何回應。

---

### Reveal Layer（顯現層）

Reveal Layer 是 FSE 中最重要也最優雅的機制之一。

它的本質是：

> **由 Actor 的 Discovery 狀態驅動的感知過濾器**

**核心公式**：

$$
Render(S, Actor\_Discoveries) \to Presentation
$$

- **S**：當前世界狀態
- **Actor_Discoveries**：Actor 已擁有的理解
- **Presentation**：Actor 實際看到的內容（文字、積木、環境描述、NPC 反應等）

**這意味著**：
- 世界本身沒有改變
- 改變的是 Actor 能「看見」什麼

---

### Presentation（表現形式）

玩家實際接觸到的所有內容（文字、圖像、聲音、介面）都屬於 Presentation。

**重要區分**：
- **Runtime** 只負責 Reveal 邏輯（什麼該顯示）
- **Adventure** 負責具體的 Presentation 形式（怎麼顯示）

例如：
- Programming World 用 Blockly 積木呈現
- Prehistoric Civilization 用環境描述文字呈現
- Formosa 可能同時使用文字 + 歷史層視覺效果

Runtime 不應該知道前端是用文字還是視覺積木。它只知道：「這個 Factor 解鎖後，某個 Path 應該被 Reveal。」

---

### 多語言與多媒介映射

FSE 支援同一 World 在不同媒介下的呈現。

**Programming World 範例**：
- Scratch 積木 → AST
- Python 程式碼 → AST
- C 語言迴圈 → AST

Runtime 只驗證 AST 是否符合預期，**不關心來源語法**。

這讓同一個學習概念可以用不同語言呈現，極大提升了包容性。

---

### 本章總結

語言與表現模型的核心思想是：

**世界不應該被綁死在某一種表現形式上。**

Reveal Layer + 抽象 Input Medium 的設計，讓 FSE 能夠同時支援文字冒險、視覺化程式學習、環境模擬等多種形態，而不會破壞核心的理解循環。

---

**下一章預告**  
第 5 章　Ubiquitous Language 與 Mapping

我們將討論為什麼 Runtime 必須保持中性語言，以及 Adventure 如何進行安全的語意映射。

