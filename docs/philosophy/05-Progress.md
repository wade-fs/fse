# 05 — Progress

> Progress 在 FSE 裡不是數字，不是百分比，不是等級。
> 這份文件回答一個根本問題：在 FSE 的世界裡，「前進」到底是什麼意思？

---

## 問題

Progress 這個詞很危險。

因為大多數人聽到 Progress，想到的是：

```
Level 3 → Level 4
50% → 75%
10/20 個任務完成
```

這些都是**可量化的外部狀態**。

但 FSE 的 Progress 不是這些。

---

## Progress 的三個維度

FSE 的 Progress 由三個維度構成。

它們同時存在，但性質完全不同。

### 1. Knowledge Progress（理解的深度）

玩家理解了什麼。

這是 Factor 的累積。

```
玩家不知道暴龍靠嗅覺
    ↓
玩家死了，困惑了，重試了
    ↓
玩家理解了「風向決定生死」
    ↓
Factor: predator_sensory_system 解鎖
```

Knowledge Progress 是**不可逆的**。

一旦玩家真正理解了某件事，

他不會「降級」。

---

### 2. World Progress（世界的改變）

玩家的行動在世界上留下的痕跡。

這不是玩家的成長，

而是**世界本身的狀態變化**。

```
Formosa：
  玩家探索老車站 → 糖鐵踏印 +1 → 聚落記憶值上升
  記憶值夠高 → 時代翻頁 → 世界真的變了

Programming World：
  玩家修復了無窮迴圈 → 系統熵值下降
  系統熵值夠低 → 新的節點解鎖
```

World Progress 有時候**可以退步**。

Formosa 的失源者就是世界在倒退。

玩家不夠快，世界就會遺忘。

---

### 3. Capability Progress（能力的邊界）

玩家因為理解而獲得的新可能。

不是「解鎖了技能」，

而是「現在能做以前做不到的事」。

```
不知道風向的重要性
  → 只能在空曠地帶行動

Discovery：風向決定氣味傳播
  → 可以進入捕食者領域的邊緣
  → 可以觀察到新的生態行為
  → 世界對玩家展示了新的面貌
```

Capability Progress 是 Knowledge Progress 的**直接後果**。

它不是獨立系統，是理解在世界上的展現。

---

## Progress 不代表什麼

```
Progress ≠ 時間投入
Progress ≠ 關卡數量
Progress ≠ 戰鬥力
Progress ≠ 收集完成度
```

一個玩家可以在 FSE 裡花了很多時間，

但如果沒有 Discovery，他沒有 Progress。

另一個玩家可以在一個 Node 裡死了七次，

然後在第八次突然領悟——

那七次失敗，**是 Progress 的一部分**，不是浪費。

---

## Progress 的核心循環

```
Discovery
    ↓
Factor 解鎖
    ↓
世界展示新面貌（reveal_layer）
    ↓
新的情境、新的行動可能
    ↓
新的 Inquiry
    ↓
新的 Discovery
    ↓
（循環）
```

Progress 不是直線，而是**螺旋上升**。

每一次 Discovery，世界的可見範圍就擴大一點。

可見範圍擴大，帶來新的困惑。

新的困惑，帶來下一個 Discovery。

---

## Progress 的成功條件

FSE 的 Progress 成功了，是指：

```
玩家在世界裡停下腳步，
只因為想知道前方還有什麼。
```

不是因為有任務要完成。

不是因為有點數要收集。

而是因為：**世界本身引起了好奇心。**

這是 Progress 最高的形式。

---

## 三個 Adventure 的 Progress 對比

| 維度 | Formosa | Dinosaur World | Programming World |
|---|---|---|---|
| Knowledge | 歷史理解的深度（踏印） | 生態知識的累積 | 程式概念的掌握 |
| World | 聚落記憶值 / 時代推進 | 生態系統的穩定度 | 系統熵值 / Bug 數量 |
| Capability | 看見更多歷史層 | 進入更危險的地區 | 解決更複雜的問題 |
| 退步可能 | ✓（失源者，遺忘） | ✓（死亡重置） | ✓（Bug 累積） |

---

## 設計原則

一個 Adventure 設計師在設計 Progress 時，

應該問：

```
1. 玩家的理解，如何改變世界對他的展示？
2. 玩家的行動，如何真正改變世界的狀態？
3. 世界可以退步嗎？退步的代價是什麼？
4. 如果玩家停下來，世界還是有趣的嗎？
```

如果第 4 題的答案是「不是」，

那麼 Progress 系統設計得太窄了。

---

## Related

→ **Pattern**: [Progressive Reveal](../patterns/Progressive-Reveal.md) — Capability Progress 如何透過 Reveal 體現

→ **ADR**: [ADR-0003 Discovery-First Progression](../adr/ADR-0003-Discovery-First-Progression.md) — Progress 設計的決策記錄

→ **Adventure**: [Formosa](../adventures/Formosa.md) — Progress 三個維度最豐富的實例（踏印 / 記憶值 / 歷史層可見性）
