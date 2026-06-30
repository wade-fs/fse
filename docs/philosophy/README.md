# Philosophy

> FSE 在任何技術之上都成立的核心思想。

---

這裡保存的不是程式。

不是 API，不是架構圖，不是資料庫欄位。

而是：**FSE 為什麼成立，以及它的不變量是什麼。**

---

## 什麼是不變量

無論 FSE 改成 Rust、改成 Unity、改成 Web，

這些文件仍然成立。

它們描述的不是「FSE 如何實作」，

而是「FSE 為什麼成立」。

---

## 閱讀順序

```
00-Why-FSE-Exists.md      → FSE 為什麼存在
01-First-Principles.md    → 不可逆的第一原則（公理）
02-Core-Concepts.md       → 最小概念集
03-Ubiquitous-Language.md → 跨 Adventure 的通用語言
04-Discovery.md           → Discovery 是什麼
05-Progress.md            → Progress 的三個維度
06-World-Model.md         → 世界如何運作
07-Presentation.md        → 玩家如何感知世界（Reveal）
08-Mapping.md             → Adventure 如何對應到 Runtime
09-Runtime.md             → Runtime 是 Core 的實作
10-Glossary.md            → 所有核心術語的快速查閱表 ← 從這裡開始翻
```

Runtime 放最後，因為它是哲學的實作，不是哲學本身。

Glossary 放在最後，因為它是查閱用的，不是閱讀用的。

---

## 這裡沒有的東西

以下內容**不在** philosophy/：

| 主題 | 位置 |
|---|---|
| 系統架構（服務、YAML、事件總線） | [architecture/](../architecture/) |
| 設計模式（Discovery Loop、Progressive Reveal） | [patterns/](../patterns/) |
| 各冒險的 Mapping 與驗證 | [adventures/](../adventures/) |
| 架構決策記錄 | [adr/](../adr/) |
| 從實作中學到的教訓 | [reflection/](../reflection/) |
