# 04 — Discovery

> Discovery 是 FSE 的核心驅動力。
> 這份文件定義它是什麼，以及它如何在世界中發生。

---

## 定義

Discovery 是**世界觀改變的那一刻**。

不是：

- 獎勵（Achievement）
- 解鎖通知（Unlock Alert）
- 任務完成（Quest Complete）
- 知識點（Knowledge Point）

而是：

**玩家內心說「原來如此」的那一秒。**

---

## Discovery 不能被教

這是最重要的約束。

說出來的 Discovery 不是 Discovery。

那只是資訊。

資訊可以被忘記。

Discovery 不能。

因為 Discovery 是玩家**自己得出**的結論，

是行動、後果、困惑、重試累積之後的**內部重構**。

---

## Discovery 的觸發結構

```
情境（Situation）
    ↓
行動（Action）       ← 玩家對世界做了什麼
    ↓
後果（Consequence）  ← 世界依規則回應，不可完全預期
    ↓
困惑（Confusion）    ← 這個很重要，不能跳過
    ↓
重試（Retry）        ← 帶著上一次的記憶再試
    ↓
Discovery            ← 玩家自己得出結論
```

**困惑是必要的中間狀態。**

如果太快給答案，困惑就不會出現。

困惑不出現，Discovery 就不會發生。

---

## Discovery 的四個特性

```
1. 不能被直接教
   說出來的 Discovery 只是資訊

2. 必須由行動引發
   玩家做了什麼
   世界回應了什麼
   玩家才能領悟什麼

3. 有時間差
   Discovery 不一定在行動當下發生
   有時候是第三次失敗之後才突然懂了

4. 改變後續行動
   真正的 Discovery 會讓玩家下一步不一樣
   如果行為沒有改變，就還沒有真正 Discover
```

---

## Discovery 與 Inquiry 的關係

Inquiry 是過程。

Discovery 是結果。

但 Discovery 不會在單次 Inquiry 中出現，而是：

```
多次 Inquiry 的累積，達到臨界點，Discovery 發生
```

換句話說：

```
Inquiry loop accumulation → threshold crossing → Discovery
```

---

## Discovery 與 Factor 的關係

Discovery 發生的那一刻，

世界在玩家的記錄裡增加一個 Factor。

Factor 是 Discovery 的**持久化形式**。

Factor 改變玩家能看見的世界（reveal_layer），

也改變玩家能做的事（新的 Node、新的對話）。

Discovery 是一次性的瞬間。

Factor 是它留下的痕跡。

---

## 三個 Adventure 的 Discovery 範例

### Formosa

```
情境：護送甘蔗到市集
行動：走在鐵軌旁邊
後果：糖鐵突然來了，NPC 喊「快閃！」
困惑：「為什麼中午會有火車？」
重試：下次提前問 NPC
Discovery：「原來糖鐵有固定班次，這是日治時代的工業命脈」
```

```
情境：進入台北城門（乙未之夜）
行動：試圖阻止辜顯榮出城
後果：無論如何，城門都打開了
困惑：「為什麼我什麼都做不了？」
Discovery：「有些歷史，無法阻止。見證比行動更難。」
```

### Dinosaur World

```
情境：看見暴龍在遠處
行動：「那麼大應該看不到我」→ 繼續走
後果：暴龍衝過來，死了
困惑：「我明明很遠？」
重試：注意到風向在吹向暴龍
Discovery：「暴龍靠嗅覺，不靠視覺。風向決定你的生死。」
```

```
情境：跑步橫越平原
行動：全速跑
後果：兩步就喘，停下來
困惑：「我平常跑步不會這樣？」
Discovery：「三疊紀氧氣只有 15%，這個星球還沒準備好讓哺乳類呼吸。」
```

### Programming World

```
情境：寫一個「數到一百」的程式
行動：while(true) { count++ }
後果：程式當掉，電腦風扇狂轉
困惑：「為什麼？我只是要數到一百？」
重試：加了 if count == 100: break
Discovery：「迴圈需要終止條件，電腦不知道你心裡想的是一百。」
```

---

## Discovery 不是什麼

| 看起來像 Discovery | 實際上是 |
|---|---|
| 「你完成了任務！」 | Quest Completion |
| 「你解鎖了新技能！」 | Unlock Notification |
| 「正確答案是 X」 | Information Transfer |
| 「你獲得了 +10 知識點」 | Gamification Reward |

以上這些，在 FSE 裡都不算 Discovery。

它們可以存在，但不是驅動力。

驅動力只有一個：

**玩家自己說出「原來如此」。**

---

## 設計原則

如果一個 Adventure 設計師想在某個 Node 製造 Discovery，

他需要問的不是：「玩家應該學到什麼？」

而是：

```
1. 玩家在這裡會做什麼行動？
2. 世界會給出什麼不可完全預期的後果？
3. 這個後果會讓玩家感到困惑嗎？
4. 困惑之後，玩家有機會重試嗎？
5. 重試之後，玩家能自己得出結論嗎？
```

如果以上五個問題都有肯定的答案，

Discovery 就會在這裡發生。
