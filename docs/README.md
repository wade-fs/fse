# FSE 文件

**作者**：Wade
**日期**：2026

> 理解不是被教的，而是透過在世界中探索、困惑、發現而自然形成的。

---

## 結構

```
docs/
  philosophy/    → FSE 為什麼成立（不變量）
  architecture/  → FSE 如何組織（可以改）
  patterns/      → Adventure 設計模式
  adventures/    → 各冒險的 Mapping 與驗證
  adr/           → 架構決策記錄
  reflection/    → 從實作中學到的教訓
```

---

## Philosophy（哲學層）

FSE 在任何技術之上都成立的核心思想。

| 文件 | 主題 |
|---|---|
| [00-Why-FSE-Exists](philosophy/00-Why-FSE-Exists.md) | FSE 為什麼存在 |
| [01-First-Principles](philosophy/01-First-Principles.md) | 不可逆的第一原則 |
| [02-Core-Concepts](philosophy/02-Core-Concepts.md) | 最小概念集 |
| [03-Ubiquitous-Language](philosophy/03-Ubiquitous-Language.md) | 跨 Adventure 的通用語言 |
| [04-Discovery](philosophy/04-Discovery.md) | Discovery 是什麼 |
| [05-Progress](philosophy/05-Progress.md) | Progress 的三個維度 |
| [06-World-Model](philosophy/06-World-Model.md) | 世界如何運作 |
| [07-Presentation](philosophy/07-Presentation.md) | 玩家如何感知世界（Reveal） |
| [08-Mapping](philosophy/08-Mapping.md) | Adventure 如何對應到 Runtime |
| [09-Runtime](philosophy/09-Runtime.md) | Runtime 是 Core 的實作 |
| [10-Glossary](philosophy/10-Glossary.md) | 所有核心術語的快速查閱表 |

---

## Architecture（架構層）

Runtime 如何組織，服務如何分工。

| 文件 | 主題 |
|---|---|
| [fse-architecture](architecture/fse-architecture.md) | 整體架構說明 |
| [data-driven-architecture](architecture/data-driven-architecture.md) | 資料驅動設計 |

---

## Patterns（模式層）

Adventure 設計師可以重用的設計模式。

| 文件 | 主題 |
|---|---|
| [Discovery-Loop](patterns/Discovery-Loop.md) | 領悟循環 |
| [Progressive-Reveal](patterns/Progressive-Reveal.md) | 漸進揭示 |
| [Hidden-Knowledge](patterns/Hidden-Knowledge.md) | 隱藏知識 |
| [Environmental-Teaching](patterns/Environmental-Teaching.md) | 環境教學 |

---

## Adventures（冒險層）

各個 Adventure 如何 Mapping，以及它教會了 Runtime 什麼。

| 文件 | 主題 |
|---|---|
| [Programming-World](adventures/Programming-World.md) | PW 的 Mapping 與驗證 |
| [Formosa](adventures/Formosa.md) | Formosa 的 Mapping 與驗證 |
| [Prehistoric-Civilization](adventures/Prehistoric-Civilization.md) | PC 的 Mapping 與驗證 |

---

## ADR（架構決策記錄）

重要設計決策的記錄，包含背景、選項、理由。

| 文件 | 決策 |
|---|---|
| [ADR-0001](adr/ADR-0001-Bootstrap-FSE.md) | Bootstrap FSE |
| [ADR-0002](adr/ADR-0002-Ubiquitous-Language.md) | Ubiquitous Language |
| [ADR-0003](adr/ADR-0003-Discovery-First-Progression.md) | Discovery-First Progression |
| [ADR-0004](adr/ADR-0004-Adventure-Mapping.md) | Adventure Mapping |
| [ADR-0005](adr/ADR-0005-World-Presentation.md) | World Presentation |
| [ADR-0006](adr/ADR-0006-Curiosity-Before-Knowledge.md) | Curiosity Before Knowledge |
| [ADR-0007](adr/ADR-0007-Adventure-Laboratory.md) | Adventure Laboratory |
| [ADR-0008](adr/ADR-0008-No-Combat-System.md) | No Combat System |

---

## Reflection（反思層）

不是設計，而是學習。

| 文件 | 主題 |
|---|---|
| [Why-PC-Changed-Runtime](reflection/Why-PC-Changed-Runtime.md) | PC 如何改變了 Runtime |
| [Lessons-from-Formosa](reflection/Lessons-from-Formosa.md) | Formosa 教會了我們什麼 |
| [What-Dinosaur-Taught-Us](reflection/What-Dinosaur-Taught-Us.md) | Dinosaur 教會了我們什麼 |
