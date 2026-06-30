# ADR-0007: Adventure Laboratory

**狀態**：已採納（Accepted）
**日期**：2026
**作者**：Wade

---

## 背景

在 FSE 的設計過程中，我們需要一個方式來**驗證 Runtime 的設計是否正確**。

問題是：

光靠思想實驗，很難知道 Runtime 的 API 是否夠用。

我們需要一個「真實的 Adventure」來驗證：

```
Discovery 的觸發流程，真的如我們設計的那樣運作嗎？
Progress 的三個維度，在實際 Adventure 裡都能表達嗎？
Presentation 的分離，在不同媒介下都可行嗎？
Mapping 的機制，Adventure 開發者真的用得起來嗎？
```

---

## 問題的核心

Formosa 是一個**大型 Adventure**：

- 歷史跨度大（西拉雅到民主紀元）
- 地點複雜（全台灣）
- 內容多（踏印、NPC、失源者、時代推進）

用 Formosa 來驗證 Runtime，會有一個問題：

**Formosa 太複雜，很難分辨「是 Runtime 的問題」還是「是 Formosa 設計的問題」。**

我們需要一個**更小、更精準的 Adventure**，

專門用來驗證 Runtime 的每個 API。

---

## 決策

**Programming World（PW）定位為 Adventure Laboratory**，

不是「Programming Course（程式設計課）」，

而是 **FSE Runtime 的驗證環境**。

PW 的每個設計決策，都對應一個 Runtime 需要驗證的能力：

| PW 的設計 | 驗證的 Runtime 能力 |
|---|---|
| 積木 AST → 行動判定 | `node_executor` + `ast_matcher` |
| Factor 解鎖 → 積木出現 | `factor_service` + `reveal_layer` |
| Stage 推進 | `progress_manager` |
| 失敗觸發 Discovery | Discovery Loop 機制 |
| 多語言（Scratch/Python/C）| `i18n_service` + Medium 分離 |

PW 小而精準，每個 Node 都對應一個可驗證的 Runtime 功能。

---

## Formosa vs PW：兩個完全不同的角色

| | Programming World | Formosa |
|---|---|---|
| 定位 | Adventure Laboratory | Large-Scale Adventure |
| 規模 | 小（幾十個 Node） | 大（數百個 Node） |
| 主要目的 | 驗證 Runtime API | 展示 FSE 的真實能力 |
| 複雜度 | 低（每個 Node 功能單純） | 高（多時代、多維度） |
| 媒介 | 積木/程式碼（特殊） | 純文字（標準） |
| 驗證重點 | Runtime 的所有 API | Adventure 的設計深度 |

這兩個 Adventure 不是競爭關係，而是**互補**的。

PW 驗證 Runtime，Formosa 展示可能性。

---

## 理由

### 1. 小 Adventure 更容易發現 Runtime 問題

當 PW 的某個功能無法實作，我們立刻知道：

「Runtime 的 API 不夠，需要擴充。」

而不是在 Formosa 的複雜設計裡迷失。

### 2. PW 的特殊媒介迫使 Presentation 分離

PW 用積木作為輸入媒介，這是 Formosa 沒有的。

這迫使我們真正實作「媒介分離」，

而不只是理論上說說。

### 3. PW 的程式語言概念天然適合 Discovery-First

「迴圈需要終止條件」這件事，

沒辦法靠課本學會。

必須讓程式當掉，讓玩家自己領悟。

PW 自然地驗證了 Discovery-First Progression 的正確性。

---

## 後果

- PW 的設計優先考慮「能不能驗證 Runtime」，而不是「能不能教會程式設計」
- 每次 Runtime 新增功能，應該在 PW 裡設計一個對應的 Node 來驗證
- Formosa 的開發可以依賴 PW 驗證過的 Runtime API，減少風險
- PW 同時也是 FSE 對外展示的「最小可展示版本」（MVP Adventure）

---

## 替代方案（被否決）

**方案 A：用 Formosa 驗證所有 Runtime 功能**

被否決原因：Formosa 太複雜，驗證困難，問題難以定位。

**方案 B：寫純 Unit Test 驗證 Runtime，不需要 Adventure**

被否決原因：Unit Test 只能驗證邏輯正確，無法驗證「設計是否合理」。只有真實的 Adventure 能告訴我們「這個 API 用起來順不順手」。

**最終決策**：PW 作為 Adventure Laboratory，用真實的 Adventure 設計來驗證 Runtime，同時保持 PW 本身作為獨立 Adventure 的完整性。

---

## Related

→ **Adventure**: [Programming World](../adventures/Programming-World.md) — Laboratory 的主體

→ **Adventure**: [Formosa](../adventures/Formosa.md) — Large-Scale Adventure，與 Laboratory 互補

→ **Reflection**: [Why PC Changed Runtime](../reflection/Why-PC-Changed-Runtime.md) — Laboratory 角色如何產生雙向影響
