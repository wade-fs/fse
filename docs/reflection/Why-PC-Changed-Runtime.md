# Reflection: Why PC Changed Runtime

> PC（Programming World）和 Runtime 之間，有一個有趣的關係：
> PW 的設計需求，反過來改變了 Runtime 的設計。
> 這不是預期的，但這正是 Adventure Laboratory 存在的意義。

---

## 什麼是「改變 Runtime」

改變 Runtime，不是指「修改程式碼」。

（當然也有修改程式碼。）

而是指：**改變了 Runtime 應該是什麼的理解。**

PW 問了幾個問題，

這些問題的答案，重新定義了 Runtime 的邊界。

---

## PW 改變 Runtime 的第一件事：Executor 必須是插件

最初的設計，`node_executor` 只有一種判定方式。

PW 的積木 AST 需要一種完全不同的判定邏輯：

```
Formosa 的 Node：
  「玩家到了這個地點，且解鎖了某個 Factor」→ 成功

PW 的 Node：
  「玩家寫的程式，轉成 AST 後，是否符合預期結構」→ 成功
```

這兩種判定，在邏輯上完全不同。

如果把 `ast_matcher` 寫死在 Runtime 裡，

Runtime 就無法保持 Adventure-Agnostic。

**PW 的需求，逼出了 `executor` 插件機制：**

```
runtime/executors/<executor_name>.c
```

每個 Adventure 可以自己定義 Executor，

Runtime 只負責「呼叫 Executor，接收結果」。

這個設計，讓 Runtime 保持通用，同時讓 PW 有自己的判定邏輯。

---

## PW 改變 Runtime 的第二件事：失敗可以觸發 Discovery

這個設計讓很多人第一次聽到時都覺得很奇怪：

**失敗觸發 Discovery？失敗不是懲罰嗎？**

PW 的設計逼我們面對這個問題：

```
玩家寫了一個沒有終止條件的迴圈
程式當掉了（失敗）
這個失敗的後果：
  → 扣血？（懲罰，沒有意義）
  → 給提示：「記得加終止條件！」（直接教學，破壞 Discovery）
  → 觸發 Factor「loop_termination」的 Discovery（失敗本身是學習的觸發器）
```

第三種設計，是我們最終採用的。

在 `challenge.yaml` 裡：

```yaml
failure_consequence:
  discover_factor: "loop_termination"  # 失敗時反而觸發概念解鎖
  error_warning: "迴圈沒有終止條件，系統記憶體耗盡！"
```

**失敗，帶著正確的 Consequence 描述，本身就是 Discovery 的觸發器。**

這個設計，成為 Runtime 的標準機制。

所有 Adventure 都可以使用 `failure_consequence.discover_factor`。

---

## PW 改變 Runtime 的第三件事：Reveal Layer 不只是文字

最初我們對 Reveal Layer 的想像，是「條件式文字顯示」：

```
玩家有 Factor X → 顯示額外的文字描述
玩家沒有 Factor X → 顯示基礎文字描述
```

PW 打破了這個假設：

```
玩家有 Factor「loop_termination」
→ 工具箱出現 [break] 積木（不是文字，是 UI 元素的可見性）
```

積木的出現或消失，是 Reveal Layer 的一種形式，

但它不是文字。

這讓我們重新定義 Reveal Layer：

**Reveal Layer 是「任何形式的可見性改變」，不只是文字。**

Runtime 只負責：「告訴 Adventure，現在應該展示 reveal_block X。」

Adventure 自己決定 reveal_block X 是什麼形式。

這個分離，讓 Runtime 可以支援任意媒介的 Reveal，

而不需要知道媒介是什麼。

---

## PW 改變 Runtime 的第四件事：多語言輸入是 Presentation 的責任

PW 支援 Blockly / Python / C 三種輸入方式。

最初的想法：在 Runtime 裡處理多語言轉換。

這立刻出現了問題：

Runtime 怎麼知道「玩家輸入的是 Scratch 積木還是 Python 程式碼」？

如果 Runtime 要知道這件事，它就必須理解 PW 的語言選擇——

這違反了 Adventure-Agnostic 的原則。

**解決方案：多語言轉換是 PW 的 Presentation 責任，不是 Runtime 的責任。**

```
玩家輸入（任何語言）
    ↓
PW 的 blockly_service.c 轉換成統一的 AST
    ↓
AST 傳給 Runtime 的 ast_matcher
    ↓
Runtime 只看 AST，不知道是什麼語言
```

這個設計，讓 Runtime 保持通用，

同時讓 PW 的多語言支援完全在 Adventure 層實現。

---

## 總結：PW 和 Runtime 的對話

PW 和 Runtime 的關係，不是單向的「Runtime 驅動 PW」，

而是雙向的對話：

```
PW 提出需求 → Runtime 思考如何通用化 → Runtime 改進 → PW 驗證
```

這個循環，正是 Adventure Laboratory 存在的意義。

每一次 PW 說「我需要這個功能」，

都是一次機會讓我們問：

**「這個功能是 PW 特有的，還是所有 Adventure 都需要的？」**

如果是後者，它就進入 Runtime。

如果是前者，它就留在 PW 的 Adventure 層。

這個問題，每次都讓 Runtime 更通用，讓邊界更清晰。

---

## 給未來的 Adventure 開發者

如果你在做新的 Adventure，遭遇了 Runtime 不支援的功能，

請問自己：

```
這個功能，是我的 Adventure 特有的嗎？
  → 是：在 Adventure 層實現，寫 executor 插件或前端邏輯
  → 否：考慮是否應該提交到 Runtime 作為通用功能

如果提交到 Runtime，這個功能的通用形式是什麼？
  → 不是「讓 Runtime 知道積木」，而是「讓 Runtime 支援任意形式的 reveal_block」
  → 不是「讓 Runtime 知道 Python 語法」，而是「讓 Runtime 接受統一的 AST」

提煉到這個程度，Runtime 才能真正通用。
```
