# 08 — Mapping

> Mapping 是 Adventure 如何把自己的世界接上 Runtime 的橋樑。
> Runtime 永遠不知道 Settlement 是什麼。
> Adventure 告訴 Runtime：「這個叫做 Node。」

---

## 問題

FSE 有兩個層：

```
Runtime  → 只懂：World / Node / Factor / Discovery / Progress / Event
Adventure → 有自己的語言：Settlement / Lesson / Biome / Memory / Era
```

這兩個層如何溝通？

答案是 Mapping。

---

## Mapping 的定義

Mapping 是 Adventure 的概念，

對應到 Runtime 的概念的**翻譯關係**。

不是繼承，不是轉換，而是：

**同一件事情，在不同語境下的名字。**

---

## 為什麼需要 Mapping

因為 Runtime 必須保持 Adventure-Agnostic。

Runtime 不能說：「當 Settlement 的記憶值夠高，推進 Era。」

Runtime 只能說：「當 World Progress 達到閾值，推進 Stage。」

Mapping 負責把 Adventure 的語言翻譯成 Runtime 的語言，

在 Adventure 啟動時注入。

---

## Formosa 的 Mapping

| Adventure 概念 | Runtime 概念 | 說明 |
|---|---|---|
| Settlement（聚落） | World | 聚落是 Formosa 的世界容器 |
| Site（地點） | Node | 每個地點是一個可互動節點 |
| Era（時代） | Stage | 時代推進 = Progress 的里程碑 |
| Memory（記憶碎片） | Discovery | 記憶收集的那一刻是 Discovery |
| 踏印（踏查記錄） | Factor | 每個踏印是一個已解鎖的理解 |
| 失源者（Specter） | Confusion Event | 遺忘是 Confusion 的具體化 |
| Reveal Layer | Reveal Layer | 直接對應，無需翻譯 |

---

## Programming World 的 Mapping

| Adventure 概念 | Runtime 概念 | 說明 |
|---|---|---|
| Zone（程式領域） | World | 每個程式概念領域是一個 World |
| Lesson Node | Node | 每個學習節點是一個 Node |
| Stage（學習階段） | Stage | 直接對應 |
| Factor（程式概念） | Factor | 直接對應 |
| Bug | Confusion Event | Bug 是系統困惑的具體化 |
| 積木解鎖 | Reveal Layer | 新積木可用 = 新理解展現 |

---

## Dinosaur World 的 Mapping

| Adventure 概念 | Runtime 概念 | 說明 |
|---|---|---|
| Era（地質年代） | Stage | 地質年代的推進是 Progress |
| Biome（生態區） | Node | 每個生態區是一個可互動節點 |
| Ecological Knowledge | Factor | 每個生態知識點是一個 Factor |
| Death / Reset | World Progress 退步 | 死亡讓某些世界狀態重置 |

---

## Mapping 的三個規則

### 規則一：Runtime 永遠不知道 Adventure 的名字

Runtime 程式碼裡，不會出現：

```
"settlement"
"era"
"lesson"
"biome"
```

這些字只存在於：

- Adventure 的 YAML 內容
- Adventure 的 master.c 注入邏輯
- Adventure 自己的文件

---

### 規則二：Mapping 在啟動時完成

Adventure 啟動時，master.c 把所有 Mapping 注入進 Runtime：

```
factor_svc → 告訴 Runtime 去哪裡找 Factor 定義
progress_svc → 告訴 Runtime 去哪裡找 Stage 定義
i18n_svc → 告訴 Runtime 用什麼語言
```

啟動後，Runtime 不再問「這是什麼 Adventure」。

它只是執行規則。

---

### 規則三：Mapping 不是限制，是翻譯

Mapping 不阻止 Adventure 定義自己的概念。

Formosa 可以有 Specter。

Dinosaur 可以有 Predator Behavior。

Programming 可以有 AST Validation。

只要這些概念在需要接觸 Runtime 時，

能翻譯成 Runtime 懂的語言，

Mapping 就完成了它的工作。

---

## Mapping 失敗的警訊

如果 Adventure 設計師發現自己在說：

```
「Runtime 需要知道這個聚落的記憶值」
「Runtime 需要特別處理 Era 推進」
「這個邏輯只能寫在 Runtime 裡，因為 Formosa 需要」
```

這說明 Mapping 失敗了。

Runtime 被 Adventure 污染了。

解決方法：

回到 Mapping，找到正確的翻譯，

讓 Runtime 只看見通用概念。

---

## Related

→ **ADR**: [ADR-0004 Adventure Mapping](../adr/ADR-0004-Adventure-Mapping.md) — 為什麼 Runtime 不能知道 Settlement

→ **Adventure**: [Formosa](../adventures/Formosa.md) — Settlement → World 的完整 Mapping 範例

→ **Adventure**: [Programming World](../adventures/Programming-World.md) — Lesson Node → Node 的 Mapping 範例

→ **Adventure**: [Prehistoric Civilization](../adventures/Prehistoric-Civilization.md) — Biome → Node 的 Mapping 範例
