# ADR-0004: Adventure Mapping

**狀態**：已採納（Accepted）
**日期**：2026
**作者**：Wade

---

## 背景

在設計 FSE 的 Runtime 時，我們遭遇了一個核心問題：

如果 Runtime 知道太多 Adventure 的細節，它就無法被重用。

```
❌ 如果 Runtime 理解「Settlement」：
   它就只能用於 Formosa

❌ 如果 Runtime 理解「Lesson」：
   它就只能用於 Programming World

❌ 如果 Runtime 理解「Biome」：
   它就只能用於 Dinosaur World
```

但如果 Runtime 完全不知道任何 Adventure 的語言，

它要怎麼驅動這些 Adventure？

這就是 Adventure Mapping 要解決的問題。

---

## 問題的核心

大多數 MUD 或遊戲 Framework，把世界特有的概念**寫死**在 Runtime 裡：

```
Room（房間）
NPC（非玩家角色）
Quest（任務）
Item（道具）
```

這些概念在大多數場景都適用，但：

- Formosa 的「Settlement」不是 Room
- Programming World 的「Zone」不是 Room  
- Dinosaur World 的「Biome」不是 Room

強行用 Room 套進去，就是設計的妥協。

---

## 決策

採用 **Adventure Mapping**：

Runtime 只認識最小的通用概念集：

```
Runtime 只知道：
  World / Node / Factor / Discovery / Progress / Event / Actor
```

每個 Adventure，在啟動時把自己的語言**翻譯**成 Runtime 的語言：

| Adventure 概念 | Runtime 概念 |
|---|---|
| Settlement（Formosa） | World |
| Site（Formosa） | Node |
| Memory（Formosa） | Discovery |
| 踏印（Formosa） | Factor |
| Lesson Node（PW） | Node |
| Stage（PW） | Stage |
| Biome（Dinosaur） | Node |
| Era（Dinosaur） | Stage |

翻譯的動作，在 Adventure 的 `master.c` 啟動時完成：

```c
// master.c（Formosa）
factor_svc->register_discovery_path("/content/memories");
progress_svc->register_progression_path("/content/eras");
progress_svc->set_default_initial_stage("main", "era_1_siraya");
```

---

## 理由

### 1. Runtime 保持 Adventure-Agnostic

Runtime 的程式碼裡，不出現任何 Adventure 的語言。

這讓 Runtime 可以驅動任何 Adventure，只要 Adventure 提供正確的 Mapping。

### 2. Adventure 保有自己的語言

Adventure 不需要改變自己的概念名稱。

Formosa 可以繼續說「Settlement」，

Programming World 可以繼續說「Lesson」，

只要在接觸 Runtime 時，翻譯成 Runtime 懂的語言。

### 3. Mapping 是唯一的介面

Adventure 和 Runtime 之間的溝通，只透過 Mapping 進行。

這讓邊界清晰：

- Runtime 的改變，不影響 Adventure 的語言
- Adventure 的改變，不影響 Runtime 的邏輯

---

## 後果

- `master.c` 成為每個 Adventure 最重要的「翻譯器」
- Runtime 服務（`factor_service`、`progress_manager` 等）不含任何路徑字串，全由 `master.c` 注入
- 新增 Adventure 只需要：(1) 設計自己的概念體系，(2) 在 `master.c` 完成 Mapping，(3) 提供 YAML 內容
- 不需要修改任何 Runtime 程式碼

---

## 替代方案（被否決）

**方案 A：在 Runtime 裡直接支援常見的 Adventure 類型**

被否決原因：Runtime 會逐漸累積 Adventure 特有的邏輯，失去通用性。

**方案 B：每個 Adventure 有自己的 Runtime Fork**

被否決原因：Runtime 的改進無法同步到所有 Adventure，維護成本爆炸。

**方案 C：Adventure Mapping（最終採用）**

Runtime 保持通用，Mapping 作為唯一的介面。每個 Adventure 完全負責自己的翻譯。

---

## Related

→ **Philosophy**: [08-Mapping](../philosophy/08-Mapping.md) — Mapping 的哲學層定義

→ **Adventure**: [Formosa](../adventures/Formosa.md) — Settlement → World 的 Mapping 範例

→ **Adventure**: [Programming World](../adventures/Programming-World.md) — Executor 插件機制作為 Mapping 的延伸
