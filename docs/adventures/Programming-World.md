# Adventure: Programming World（PW）

> PW 是 FSE 的 Adventure Laboratory。
> 它的首要角色不是「教程式設計」，而是「驗證 Runtime 的每一個 API」。

---

## 一句話

Programming World 是一個程式執行的世界。

玩家不是在「學」程式設計，

而是**在程式的世界裡行動，遭遇程式邏輯的後果，然後自己領悟**。

---

## 為什麼 PW 是 Laboratory

Formosa 很複雜——數百個地點、多個時代、多種 NPC。

當 Formosa 遇到問題，很難知道是「Runtime 不夠」還是「設計太複雜」。

PW 解決這個問題：

**每個 PW 的 Node，都對應一個需要驗證的 Runtime 能力。**

```
無窮迴圈沼澤    → 驗證：Discovery Loop + 失敗觸發 Factor
變數消失森林    → 驗證：世界狀態 + Factor 前置條件
競爭條件峽谷    → 驗證：多執行緒概念的 Consequence
```

---

## PW 的 Mapping

| PW 概念 | FSE Runtime 概念 | 說明 |
|---|---|---|
| Zone（程式概念領域） | World | 每個程式概念領域是獨立的 World |
| Node（學習節點） | Node | 直接對應 |
| Stage（學習階段） | Stage | 直接對應 |
| Factor（程式概念） | Factor | 直接對應 |
| Bug / 系統錯誤 | Confusion Event | Bug 是 Confusion 的具體化 |
| 積木解鎖 | Reveal Layer | 新積木可用 = 理解後的新可能 |
| AST 驗證 | Node Executor | `ast_matcher` 判定程式邏輯 |

---

## PW 的特殊能力：多語言輸入

PW 的 Presentation 媒介有多種：

```
初學者 → Blockly 積木（視覺化）
中級者 → Python（腳本）
高級者 → C（底層）
```

但 Runtime 只看到一個結果：

```
玩家的輸入（無論是哪種語言）
    ↓
AST（抽象語法樹）
    ↓
ast_matcher 比對
    ↓
Consequence
```

這驗證了 Presentation 分離的正確性：

**媒介不同，但 Runtime 的判定邏輯相同。**

---

## PW 的 Discovery 範例

### 無窮迴圈沼澤

```
情境：沼澤裡的數字在無限累積
行動：while(true) { count++ }
後果：程式當掉，整個沼澤陷入混亂
困惑：「我只是要讓它數到一百？」
Retry：加了條件——還是不對
再試：改成有限迴圈
Discovery：「迴圈需要終止條件，電腦不知道你心裡想的是一百」
Reveal：工具箱出現 [直到...] 和 [中斷] 積木
```

### 變數消失森林

```
情境：森林裡放的東西一直消失
行動：把值存到一個函式裡的變數，出了函式就不見了
後果：任務失敗，存的東西不見了
困惑：「我明明存進去了？」
Discovery：「變數有生命週期，出了它的作用域就死了」
Reveal：出現跨函式的全域變數概念
```

### 競爭條件峽谷

```
情境：兩條河流同時注入同一個水庫，水量計算一直出錯
行動：讓兩個程式同時讀寫同一個變數
後果：數字亂掉了
困惑：「我明明寫對了？」
Discovery：「並行不等於同步。競爭條件讓結果無法預測。」
Reveal：出現 mutex（互斥鎖）的概念
```

---

## PW 的 Progress

### Knowledge Progress

程式概念的理解深度：

```
順序執行 → 條件判斷 → 迴圈 → 函式 → 作用域 → 並行 → 競爭條件
```

每個概念都是前一個的延伸，

但玩家不是被「教」這些概念，

而是透過程式在世界裡「出問題」而領悟。

### World Progress

系統的健康程度：

```
系統熵值（Bug 的累積程度）
  → 熵值低：世界運作正常，新的 Node 開放
  → 熵值高：阻礙出現，某些路徑被 Bug 封堵
  → 玩家修復 Bug → 熵值下降 → 世界恢復
```

### Capability Progress

能解決更複雜的問題：

```
只能寫線性程式 → 能寫條件分支 → 能寫有限迴圈 → 能寫函式 → 能處理並行
```

---

## PW 驗證了什麼

### 已驗證

✓ **ast_matcher executor** 可以正確判定程式邏輯

✓ **多語言輸入（Blockly/Python/C）** 都能轉成統一 AST

✓ **Factor 前置條件** 正確控制積木的可見性

✓ **失敗觸發 Discovery**：在 `challenge.yaml` 的 `failure_consequence` 裡設定 `discover_factor`，失敗時反而觸發概念解鎖——這個機制非常重要，驗證通過

✓ **Stage 推進** 由 Factor + Quest 組合條件控制

✓ **Presentation 分離**：積木、Python、C 都透過相同的 reveal_layer 機制控制可見性

### 待驗證

? **多玩家在同一個 Zone 的狀態同步**

? **玩家的程式「副作用」是否能真正改變世界狀態**（不只是判定對錯）

? **退步機制**：玩家能否讓系統熵值無限上升，直到完全無法行動？

---

## PW 教會了 Runtime 什麼

1. **`failure_consequence` 的重要性**：失敗不只是「扣血」，失敗可以觸發 Discovery。這個設計在 PW 裡驗證後，成為 Runtime 的標準機制。

2. **Executor 必須是可擴充的**：PW 需要 `ast_matcher`，Formosa 需要 `discovery_trigger`，未來的 Adventure 可能需要更多。Runtime 必須讓 Executor 成為插件，而不是內建邏輯。

3. **Reveal Layer 必須支援「工具箱更新」**：PW 的積木出現/消失，讓我們意識到 Reveal Layer 不只是文字描述的切換，而是任何形式的「可見性改變」。

---

## 目前實作狀態

```
LPC / Go 基底：已完整
  master.c               ✓
  user.c（玩家載體）      ✓
  blockly_service.c      ✓
  ast_matcher executor   ✓

YAML 內容：部分完成
  stage_1_sequence       ✓（無窮迴圈沼澤）
  stage_2_loop           進行中
  stage_3_conditional    待設計

Web 前端（Blockly UI）：已完整
  index.html             ✓
  pw-blockly-adapter.js  ✓
```

---

## Related

→ **Philosophy**: [08-Mapping](../philosophy/08-Mapping.md) — PW 的 Mapping 設計原則

→ **Pattern**: [Discovery Loop](../patterns/Discovery-Loop.md) — 無窮迴圈沼澤是 Discovery Loop 最乾淨的範例

→ **ADR**: [ADR-0007 Adventure Laboratory](../adr/ADR-0007-Adventure-Laboratory.md) — PW 作為 Adventure Laboratory 的定位

→ **Reflection**: [Why PC Changed Runtime](../reflection/Why-PC-Changed-Runtime.md) — PW 如何反過來改變了 Runtime 的設計
