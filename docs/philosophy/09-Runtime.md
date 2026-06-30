# 09 — Runtime

> Runtime 是 Core 的實作，不是 Core 本身。
> 這份文件說明 Runtime 的職責邊界，以及它為什麼放在最後。

---

## 為什麼 Runtime 放最後

因為 Runtime 不是哲學。

Runtime 是哲學的**一種實作方式**。

先有：

```
為什麼 FSE 存在（00）
第一原則（01）
核心概念（02）
通用語言（03）
Discovery（04）
Progress（05）
世界模型（06）
Presentation（07）
Mapping（08）
```

然後才有 Runtime。

Runtime 服務於這些思想，

不是這些思想服務於 Runtime。

---

## Runtime 的唯一職責

**把 Core 的思想執行起來。**

就這樣。

具體來說：

```
Core 說：世界有因果
Runtime 執行：Event → Consequence → State Change

Core 說：Discovery 由行動+困惑觸發
Runtime 執行：Factor 前置條件檢查 + 閾值判定

Core 說：玩家能看見的世界由理解深度決定
Runtime 執行：Reveal Layer 過濾

Core 說：Progress 有三個維度
Runtime 執行：Factor / Stage / World State 追蹤
```

---

## Runtime 不知道的事

Runtime 不知道，也不應該知道：

```
- 這是 Formosa 還是 Dinosaur World
- Settlement 是什麼
- Era 是什麼
- Lesson 是什麼
- 玩家是學生還是探險家
- 這個世界在教什麼知識
```

這些都是 Adventure 的事。

Runtime 只知道：

```
World / Node / Factor / Discovery / Progress / Event / Actor
```

---

## Runtime 可以改變

Runtime 可以：

- 從 LPC 改成 Go
- 從 Go 改成 Rust
- 從伺服器端改成客戶端
- 從 MUD 協定改成 WebSocket
- 從單機改成多人

只要它仍然正確地執行 Core 的思想，

它就還是 FSE。

這是 FSE 架構最重要的設計決策：

**Core 不依賴 Runtime，Runtime 依賴 Core。**

---

## Runtime 的三個層

詳細的實作說明請見 [architecture/fse-architecture.md](../architecture/fse-architecture.md)。

概念層面，Runtime 由三個層構成：

### 層一：核心服務（Adventure-Agnostic）

執行 Core 概念的通用機制：

```
event_bus        → 因果傳遞（Event）
factor_service   → 理解記錄（Factor / Discovery）
progress_manager → 進度追蹤（Progress）
node_executor    → 行動判定（Node + Challenge）
reveal_layer     → 可見性過濾（Presentation）
```

### 層二：Adventure 注入

Adventure 啟動時告訴 Runtime：

```
「去哪裡找 Factor 定義」
「去哪裡找 Stage 定義」
「用什麼語言」
「從哪個 Node 開始」
```

Runtime 接受注入後，才能運作。

### 層三：Adventure 內容

Adventure 的 YAML、LPC、或任何內容格式，

提供具體的 Node、Factor、Stage 定義。

Runtime 讀取這些定義，執行規則。

---

## Runtime 的邊界測試

一個好的 Runtime 設計，

應該能通過這個測試：

**把 Formosa 換成 Dinosaur World，Runtime 不需要改任何程式碼。**

只需要換 Adventure 的注入配置和內容 YAML。

如果換 Adventure 需要改 Runtime 的程式碼，

說明 Runtime 被 Adventure 污染了，

需要重新 Mapping。

---

## Runtime 是 FSE 的最後一層

```
Philosophy（為什麼）
    ↓
Architecture（如何組織）
    ↓
Patterns（如何設計）
    ↓
Runtime（如何執行）
```

Runtime 在最底層，

承接所有上層的思想，

把它們變成真實可執行的世界。

---

## Related

→ **Architecture**: [FSE Architecture](../architecture/fse-architecture.md) — Runtime 的完整實作說明

→ **ADR**: [ADR-0004 Adventure Mapping](../adr/ADR-0004-Adventure-Mapping.md) — Runtime 為什麼保持 Adventure-Agnostic

→ **Reflection**: [Why PC Changed Runtime](../reflection/Why-PC-Changed-Runtime.md) — PW 如何改變了 Runtime 的設計
