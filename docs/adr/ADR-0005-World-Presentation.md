# ADR-0005: World Presentation

**狀態**：已採納（Accepted）
**日期**：2026
**作者**：Wade

---

## 背景

在設計玩家如何「看見世界」這件事時，我們遭遇了一個問題：

玩家看見的世界，應該由誰決定？

最初的想法是：Runtime 決定。

但這立刻出現了問題：

```
Formosa 的玩家 → 看見文字描述
Programming World 的玩家 → 看見積木工具箱
Dinosaur World 的玩家 → 感知環境訊號（風向、聲音、氣味）
```

如果 Runtime 要知道「這是文字還是積木還是環境訊號」，

它就必須理解每個 Adventure 的 Presentation 格式——

這違反了 Adventure-Agnostic 的原則。

---

## 問題的核心

Presentation 有兩個層次：

**層次一：Runtime 知道的（Semantic Layer）**

```
玩家有哪些 Factor？
→ 因此哪些 reveal_block 應該展示？
→ 哪些路徑應該開放？
```

**層次二：Runtime 不知道的（Medium Layer）**

```
這個 reveal_block 在 Formosa 是「文字段落」
同一個 reveal_block 在 PW 是「積木出現在工具箱」
同一個 reveal_block 在 Dinosaur 是「環境訊號出現」
```

Runtime 應該只處理層次一，

層次二交給 Adventure 自己決定。

---

## 決策

採用 **Reveal Layer + Medium 分離**：

Runtime 只負責：

```
Render(World_State, Player_Factors) → reveal_block_ids
```

翻譯成「玩家能看見哪些 reveal_block 的 ID」。

**至於這些 ID 在具體 Adventure 中呈現為什麼形式，Runtime 不管。**

Adventure 自己決定 ID 對應的呈現方式：

```
Formosa：
  reveal_block: "historical_layer_1915"
  → 呈現為：額外的文字描述段落

Programming World：
  reveal_block: "controls_break"
  → 呈現為：工具箱新增 [break] 積木

Dinosaur World：
  reveal_block: "predator_nearby_signal"
  → 呈現為：環境描述中加入嗅覺線索
```

---

## 理由

### 1. Presentation 的「什麼」與「怎麼」分離

Runtime 知道：「現在應該展示 reveal_block X。」

Adventure 知道：「reveal_block X 在我的世界裡是什麼樣子。」

這個分離讓 Runtime 保持通用，讓 Adventure 保持靈活。

### 2. 媒介多樣性是 FSE 的優勢

不同的 Adventure 可以用完全不同的媒介傳達世界。

Formosa 用文字，PW 用積木，Dinosaur 用環境訊號——

這些在 Runtime 層都是同樣的機制（reveal_block），

只是呈現的形式不同。

### 3. 命名：Presentation 而非 UI

「UI」暗示「介面」，

「Presentation」暗示「世界如何向玩家揭示自己」。

這是設計哲學的差別：

我們不是在設計介面，我們是在設計**玩家感知世界的方式**。

---

## 後果

- `reveal_layer` 服務只輸出 `reveal_block_id` 的集合，不含任何 UI 邏輯
- Adventure 的 YAML 裡，`reveal_layers` 只需定義觸發條件（哪些 Factor），呈現方式在 Adventure 的前端或 LPC 層處理
- 未來支援新的 Presentation 媒介（VR、音訊、觸覺），不需要修改 Runtime
- 設計師在思考 Presentation 時，應該先問「玩家應該感知什麼」，而不是「玩家應該看到什麼介面」

---

## 替代方案（被否決）

**方案 A：Runtime 內建所有 Presentation 類型（文字、積木、環境）**

被否決原因：Runtime 會越來越肥大，每新增一種媒介就需要修改 Runtime。

**方案 B：完全讓 Adventure 自己處理所有 Presentation 邏輯（不經過 Runtime）**

被否決原因：Reveal 的觸發條件（Factor 檢查）需要 Runtime 的支援，完全繞開 Runtime 會造成重複邏輯。

**最終決策**：Runtime 處理 Semantic Layer（哪些 reveal_block 該展示），Adventure 處理 Medium Layer（怎麼展示）。
