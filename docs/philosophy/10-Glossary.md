# 10 — Glossary

> 這裡是 FSE 所有核心術語的快速查閱表。
> 每個詞都有一句話定義，以及「不是什麼」的對照。
> 詳細說明請見各自的文件。

---

## World（世界）

**一句話**：知識存在的地方。有情境，有因果，有時間。

**World 是**：老師。但它不開口說話，只讓事情發生。

**World 不是**：背景、容器、舞台、課程。

**World 的必要條件**：
```
1. 因果成立——玩家的行動有真實後果
2. 知識內嵌於情境——不寫在說明書上
3. 可以重複進入——每次都能有新的發現
```

**各 Adventure 的 World**：
- Formosa → 台灣歷史的時間與地景
- Programming World → 程式執行的邏輯空間
- Prehistoric Civilization → 中生代的生態系統

→ 詳見 [06-World-Model.md](06-World-Model.md)

---

## Node（節點）

**一句話**：World 的組成單位。一個有情境、有約束、有因果的可互動空間。

**Node 是**：玩家當下所在、所能感知、所能行動的空間。

**Node 不是**：關卡、房間、場景、地圖格子。

**Node 的組成**：
```
exits         → 可以去哪裡（出口）
reveal_layers → 玩家能看見什麼（由理解深度決定）
challenges    → 世界提出的約束
discoveries   → 可能在這裡發生的領悟
```

**Node 不知道**：它屬於哪個 Adventure，它叫什麼名字（Settlement / Lesson / Biome 都是 Adventure 的語言，不是 Node 的語言）。

**各 Adventure 的 Node**：
- Formosa → Site（地點）
- Programming World → Lesson Node
- Prehistoric Civilization → Biome（生態區）

---

## Actor（行動者）

**一句話**：在 World 裡移動、行動、提問的人。

**Actor 是**：用行動提問的人。

**Actor 不是**：學生、使用者、角色扮演的人物。

**Actor 的核心行為**：
```
Move    → 去一個地方
Observe → 感知世界的狀態
Act     → 對世界做某件事
Fail    → 承受後果（必要的）
Try     → 再試一次，帶著上一次的記憶
```

**Actor 的成長**：不是等級數字，而是對這個世界的理解深度。

---

## Factor（因子）

**一句話**：玩家理解某件事情的最小記錄單位。

**Factor 是**：玩家與世界的一次理解交集的持久化形式。

**Factor 不是**：技能、成就、點數、徽章、解鎖項目。

**Factor 的特性**：
```
- 有前置條件（需要先理解某些事才能理解這個）
- 解鎖後改變玩家能看見的世界（reveal_layer）
- 解鎖後改變玩家能做的事
- 可能觸發 Progress 的推進
```

**Factor 與 Discovery 的關係**：Discovery 是那一刻的領悟，Factor 是它留下的持久紀錄。

---

## Discovery（領悟）

**一句話**：世界觀改變的那一刻。

**Discovery 是**：玩家內心說「原來如此」的那一秒。

**Discovery 不是**：獎勵、解鎖通知、任務完成、知識點、成就。

**Discovery 的觸發結構**：
```
情境 → 行動 → 後果 → 困惑 → 重試 → Discovery
```

**最重要的約束**：Discovery 不能被直接教。說出來的 Discovery 只是資訊。

**困惑是必要的**：沒有困惑，Discovery 不會發生。太快給答案，困惑就不出現。

**Discovery 與 Factor 的關係**：Discovery 是瞬間，Factor 是痕跡。

→ 詳見 [04-Discovery.md](04-Discovery.md)

---

## Progress（進度）

**一句話**：玩家在 World 裡的累積狀態。

**Progress 是**：理解的深度在世界上留下的痕跡。

**Progress 不是**：等級、分數、百分比、任務數量、時間投入。

**Progress 的三個維度**：
```
Knowledge Progress  → 玩家理解了什麼（Factor 的累積）
World Progress      → 世界因玩家行動而改變的狀態（可以退步）
Capability Progress → 玩家因理解而獲得的新可能（Knowledge 的展現）
```

**Progress 的成功條件**：
玩家停下來，只因為想知道前方還有什麼。不是因為任務，不是因為點數。

→ 詳見 [05-Progress.md](05-Progress.md)

---

## Presentation（呈現）

**一句話**：玩家能感知世界的哪個部分。

**Presentation 是**：世界如何向玩家揭示自己（Reveal）。

**Presentation 不是**：UI、介面、視覺設計、畫面。

**Reveal 的三個層次**：
```
存在性 Reveal  → 這裡有什麼（玩家能看見哪些東西）
意義性 Reveal  → 這意味著什麼（相同的東西有了新的意義）
可能性 Reveal  → 能做什麼（新的行動選項出現）
```

**核心機制**：同一個地點，在不同理解深度的玩家眼中，是不同的世界。

**各 Adventure 的媒介**：
```
Formosa            → 文字描述、NPC 對話、地景變化
Programming World  → 工具箱積木、程式執行結果
Prehistoric Civ    → 環境訊號（風向、聲音、氣味、光線）
```

**Runtime 只知道**：「現在應該展示 reveal_block X。」
**Adventure 決定**：reveal_block X 長什麼樣子。

→ 詳見 [07-Presentation.md](07-Presentation.md)

---

## Event（事件）

**一句話**：World 內部的因果傳遞媒介。

**Event 是**：行動與後果之間的橋樑，讓 World 的各部分解耦。

**Event 不是**：通知、訊息、Log。

**Event 的流動**：
```
Actor 行動
    ↓
Event 發布
    ↓
World 各部分回應（factor_service / progress_manager / reveal_layer）
    ↓
State 改變
    ↓
新的 Situation 形成
```

**Event 的價值**：因果仍然成立，但各系統不需要直接互相呼叫。

---

## 快速對照表

| 術語 | 一句話 | 不是什麼 |
|---|---|---|
| **World** | 知識存在的地方 | 背景、容器、課程 |
| **Node** | 可互動的情境空間 | 關卡、房間、地圖格 |
| **Actor** | 用行動提問的人 | 學生、使用者 |
| **Factor** | 理解的最小持久單位 | 技能、成就、點數 |
| **Discovery** | 世界觀改變的那一刻 | 獎勵、任務完成、知識點 |
| **Progress** | 理解留下的累積痕跡 | 等級、分數、百分比 |
| **Presentation** | 玩家能感知什麼 | UI、介面、畫面 |
| **Event** | 因果傳遞的媒介 | 通知、訊息、Log |

---

## Adventure 語言 vs FSE 核心語言

Adventure 可以有自己的語言，但需要 Mapping 到 FSE 的核心語言。

| Adventure 語言 | FSE 核心語言 |
|---|---|
| Settlement（Formosa） | World |
| Site（Formosa） | Node |
| Era（Formosa） | Stage |
| Memory（Formosa）| Discovery |
| 踏印（Formosa） | Factor |
| Lesson Node（PW） | Node |
| Bug（PW） | Confusion Event |
| Biome（PC） | Node |
| 地質年代（PC） | Stage |

Runtime 永遠只看右欄。左欄是 Adventure 的事。

→ 詳見 [08-Mapping.md](08-Mapping.md)
