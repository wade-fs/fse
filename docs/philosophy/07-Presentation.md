# 07 — Presentation

> Presentation 不是 UI。
> 它是世界如何向玩家揭示自己。

---

## 不是 UI，是 Reveal

大多數遊戲的 Presentation 是：

「如何把資訊顯示給玩家看。」

FSE 的 Presentation 是：

「**玩家能感知世界的哪個部分。**」

這是根本的差別。

---

## Reveal 的核心機制

在 FSE 裡，世界不是全部可見的。

玩家能看見什麼，取決於他已經理解了什麼。

```
玩家的理解（Factor 的累積）
    ↓
Reveal Layer 過濾
    ↓
玩家感知到的世界
```

同一個地點，不同理解深度的玩家，

看見的是**不同的世界**。

---

## 為什麼要限制可見性

因為過早揭示資訊，會破壞 Discovery。

```
玩家第一次走進生態區：
  看見：「前方是一片松林。」

玩家理解了「獸腳類的夜間視力」之後：
  看見：「在松林的陰影深處，掠食者的雙眼正處於失焦狀態，
         你可以利用陰影避開其視線。」
```

如果玩家還沒理解這件事，

他不應該看到這個提示。

提示只對已經有一定理解的人有意義。

對沒有理解的人，提示只是噪音。

---

## Reveal 的三個層次

### 1. 環境層（World 告訴玩家什麼存在）

最基礎的 Reveal。

玩家進入一個地點，世界描述它看起來是什麼。

這個描述本身，隨著玩家的理解而改變：

```
無理解：「一片雜草叢生的荒地。」
有「糖鐵興衰」的 Factor：「雜草下隱約露出一排鏽蝕的糖鐵軌道殘影。」
```

---

### 2. 可能性層（World 告訴玩家能做什麼）

玩家能採取什麼行動，

取決於他理解了什麼。

```
不知道暴龍靠嗅覺：
  可用行動：跑、躲、等待
  不可用：利用風向隱蔽自己（因為不知道風向重要）

理解了「暴龍靠嗅覺，風向決定氣味傳播」：
  新增行動：選擇順風路線移動
  新增行動：等待風向改變再行動
```

---

### 3. 意義層（World 告訴玩家這意味著什麼）

最深的 Reveal。

同樣的後果，有理解的玩家和沒理解的玩家，

讀出的意義完全不同。

```
無理解：「程式當掉了。」（困惑，沮喪）
有「迴圈需要終止條件」的 Factor：「哦，又是無窮迴圈。我需要加 break。」（立刻知道怎麼做）
```

Reveal 的意義層，讓有理解的玩家

能從後果中讀出世界的規則。

---

## Presentation 的媒介

不同的 Adventure，用不同的媒介向玩家揭示世界：

```
Formosa          → 文字描述、NPC 對話、地景變化
Dinosaur World   → 環境訊號（風向、氣味、聲音、視覺）
Programming World → 程式執行結果、錯誤訊息、工具可用性
```

媒介不同，但 Reveal 的邏輯相同：

**理解的深度決定玩家感知的廣度。**

---

## Presentation 不等於難易度

Reveal Layer 不是難易度設定。

它不是：

```
初級玩家 → 看見更多提示
進階玩家 → 提示減少
```

它是：

```
玩家理解了什麼 → 世界展示對應的面貌
```

一個玩家在 Formosa 深耕了一個月，

他對台灣歷史的感知，

遠比剛進來的玩家豐富——

不是因為他是「進階玩家」，

而是因為他**真的理解了更多**。

---

## Reveal 的設計原則

Adventure 設計師在設計 Reveal Layer 時，

應該問：

```
1. 這個資訊，對沒有理解的玩家是噪音嗎？
2. 這個資訊，對有理解的玩家是線索嗎？
3. 這個 Reveal 會讓玩家「原來如此」嗎？
4. 如果玩家沒有對應的 Factor，他會困惑嗎？（困惑是好事）
5. 這個 Reveal 製造了新的問題，引發新的 Inquiry 嗎？
```

好的 Reveal 不是「給答案」，

而是「讓答案對有準備的人自然浮現」。

---

## Presentation 與 Discovery 的關係

Presentation 是 Discovery 的**前一步**和**後一步**。

```
前一步：
  世界揭示了一個線索（Reveal）
      ↓
  玩家行動，遭遇後果
      ↓
  Discovery 發生

後一步：
  Discovery 之後，玩家的 Factor 增加
      ↓
  Reveal Layer 更新
      ↓
  世界展示新的面貌（新的 Reveal）
      ↓
  引發新的 Inquiry
```

Presentation 和 Discovery 是互相驅動的。

沒有 Reveal，Discovery 失去前因。

沒有 Discovery，Reveal 沒有後果。

---

## Related

→ **Pattern**: [Progressive Reveal](../patterns/Progressive-Reveal.md) — Presentation 的核心設計模式

→ **ADR**: [ADR-0005 World Presentation](../adr/ADR-0005-World-Presentation.md) — Semantic Layer 與 Medium Layer 分離的決策記錄

→ **Adventure**: [Programming World](../adventures/Programming-World.md) — 積木工具箱作為 Presentation 媒介的特殊實例
