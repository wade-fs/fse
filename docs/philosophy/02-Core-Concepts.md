# 02 — Core Concepts

> 這裡只有 FSE 的最小概念集。
> 不是 API，不是類別，不是資料庫欄位。
> 而是：無論 FSE 改成什麼技術，這些概念永遠成立。

---

## 最小概念集

```
World
Actor
Node
Factor
Discovery
Progress
Event
```

這七個概念，在所有 Adventure 裡都存在。

形式不同，結構相同。

---

## World（世界）

知識存在的地方。

有情境，有因果，有時間。

**World 不是背景。World 是老師。**

但它不開口說話。它只是讓事情發生。

World 的必要條件：

```
1. 因果成立
   玩家的行動有真實後果，不是假的選擇

2. 知識內嵌於情境
   知識不寫在說明書上
   知識藏在世界的運作規則裡

3. 可以重複進入
   第一次不懂
   第二次有點懂
   第三次真的懂了
```

---

## Actor（行動者）

在 World 裡移動、行動、提問的人。

**Actor 不是學生。Actor 是用行動提問的人。**

Actor 的核心行為：

```
Move    → 去一個地方
Observe → 看見世界的狀態
Act     → 對世界做某件事
Fail    → 承受後果
Try     → 再試一次，帶著上一次的記憶
```

注意：Fail 是必要的。

沒有失敗，就沒有真實的因果。

沒有真實的因果，就沒有 Discovery。

---

## Node（節點）

World 的組成單位。

不是「關卡」，不是「房間」，不是「地點」。

而是：一個有情境、有約束、有因果的**可互動空間**。

Node 的特性：

```
- 有 exits（可以去哪裡）
- 有 reveal_layers（玩家能看見什麼，由理解深度決定）
- 有 challenges（世界提出的約束）
- 有 discoveries（可能在這裡發生的領悟）
```

Node 不知道它屬於哪個 Adventure。

Node 只知道：玩家在這裡，世界要回應。

---

## Factor（因子）

玩家理解某件事情的**最小記錄單位**。

不是「技能」，不是「成就」，不是「點數」。

而是：**玩家與世界的一次理解交集**。

Factor 的特性：

```
- 有前置條件（需要先理解某些事才能理解這個）
- 解鎖後改變玩家能看見的世界（reveal_layer）
- 解鎖後改變玩家能做的事（新的 Node、新的對話）
- 可能觸發 Progress 的推進
```

---

## Discovery（領悟）

世界觀改變的那一刻。

不是獎勵，不是解鎖通知。

**是玩家內心說「原來如此」的那一秒。**

Discovery 的觸發結構：

```
情境（Situation）
    ↓
行動（Action）
    ↓
結果（Consequence）
    ↓
困惑（Confusion） ← 這個很重要，不能跳過
    ↓
重試（Retry）
    ↓
Discovery
```

困惑是必要的中間狀態。

太快給答案，Discovery 就不發生了。

詳見：[04-Discovery.md](04-Discovery.md)

---

## Progress（進度）

玩家在 World 裡的**累積狀態**。

不是等級，不是分數，不是百分比。

而是：**理解的深度在世界上留下的痕跡**。

Progress 有三個維度：

```
Knowledge Progress  → 玩家理解了什麼（Factor 的累積）
World Progress      → 世界因玩家的行動而改變的狀態
Capability Progress → 玩家因理解而獲得的新可能
```

詳見：[05-Progress.md](05-Progress.md)

---

## Event（事件）

World 內部的因果傳遞媒介。

當玩家行動時，世界透過 Event 傳遞後果：

```
Actor 行動
    ↓
Event 發布
    ↓
World 各部分回應
    ↓
State 改變
    ↓
新的 Situation 形成
```

Event 使 World 的各個部分解耦，

但因果仍然成立。

---

## 不在這裡的概念

以下概念是 Adventure 特有的，不屬於 Core：

```
Settlement  → Formosa 特有
Lesson      → PW 特有
Biome       → Dinosaur 特有
Quest       → 各 Adventure 自行定義
NPC         → 各 Adventure 自行定義
```

Core 只保留跨所有 Adventure 都成立的概念。

其餘的，交給各 Adventure 自己定義。

---

## Related

→ **Philosophy**: [04-Discovery](04-Discovery.md) — Discovery 的完整定義

→ **Philosophy**: [05-Progress](05-Progress.md) — Progress 的三個維度

→ **Philosophy**: [08-Mapping](08-Mapping.md) — Core 概念如何映射到各 Adventure

→ **Philosophy**: [10-Glossary](10-Glossary.md) — 快速查閱所有術語
