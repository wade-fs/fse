# Adventure: Formosa（源流福爾摩沙）

> Formosa 是 FSE 的第一個大型 Adventure。
> 它在 FSE 框架命名之前就已存在——它只是還沒有意識到自己是一個 FSE 實例。

---

## 一句話

Formosa 是一個在台灣歷史的時間與地景中行走的世界，

玩家透過探索、踏查、對抗遺忘，

理解「記憶如何成為一片土地的靈魂」。

---

## Formosa 的 Mapping

| Formosa 概念 | FSE Runtime 概念 | 說明 |
|---|---|---|
| Settlement（聚落） | World | 聚落是 Formosa 的世界容器 |
| Site（地點） | Node | 每個地點是一個可互動節點 |
| Era（時代） | Stage | 時代推進 = Progress 的里程碑 |
| Memory（記憶碎片） | Discovery | 記憶收集的那一刻是 Discovery |
| 踏印（踏查記錄） | Factor | 每個踏印是一個已解鎖的理解 |
| 失源者（Specter） | Confusion Event | 遺忘是 Confusion 的具體化 |
| Reveal Layer | Reveal Layer | 直接對應 |
| 修復師 | Actor | 玩家的職涯是 Actor 的成長方式 |

---

## Formosa 的 Discovery 如何運作

Formosa 的 Discovery 以「Memory（記憶碎片）」的形式存在。

但 Memory 這個名字有點誤導——

它不是「收集記憶」，而是「觸發領悟的那一刻」。

```
Discovery 觸發條件（以老車站為例）：

前置理解（Factor）：
  - 嘉南平原探索完成
  - 日治時代農業概念解鎖

觸發情境（Site + Action）：
  - 玩家走進舊車站廣場
  - 玩家嘗試搭上糖鐵

後果（Consequence）：
  - 糖鐵突然來了，NPC 喊「快閃！」

困惑：
  - 「為什麼中午會有這班車？」

Discovery：
  - 「原來糖鐵有固定班次，這是日治時代的工業命脈」
```

---

## Formosa 的 Progress

### Knowledge Progress

玩家理解的歷史深度。

```
西拉雅語地名 → 荷蘭時期貿易 → 清治移民 → 日治工業 → 二戰前後 → 民主紀元
```

每個時代都有其獨特的 Factor，

理解了才能看見那個時代的世界。

### World Progress

世界的宏觀狀態。

```
各聚落的記憶值（0 ~ 100）
  → 記憶值代表該地點的歷史保存程度
  → 記憶值足夠高 → 時代翻頁 → 世界真的改變了

時代推進：
  西拉雅時代 → 荷蘭時期 → 明鄭時期 → 清治時期 → 日治時期 → 戰後 → 民主紀元
```

### Capability Progress

理解改變玩家能做的事：

```
不知道任何歷史
  → 只能看見地表的基礎描述

解鎖「西拉雅地名」
  → 能感知地名背後的語言層

解鎖「日治工業」
  → 能看見糖鐵軌道、鹽田遺址

解鎖「嘉南源流」+ 天賦「island_memory」
  → 能看見所有時代的疊合
```

---

## Formosa 的 Presentation

媒介：**文字描述 + NPC 對話 + 地景變化**

Reveal 的特殊性：

在 Formosa 裡，同一個地點在不同時代有不同的樣子：

```
民雄（今）：現代城市，普通的小鎮
民雄（日治）：製糖工廠的核心，蔗田延伸到地平線
民雄（清治）：「打貓」，平埔族人的聚落痕跡

玩家從「現代民雄」出發，
逐漸理解每一層歷史，
才能進入更古老的時代層。
```

這是 Formosa 最特別的 Reveal 設計：

時間本身是 Reveal 的維度。

---

## Formosa 驗證了什麼

### 已驗證

✓ **大型 World 的可行性**：數百個 Site，多個時代，單一 Runtime 可以承載

✓ **多維度 Progress**：Knowledge（踏印） / World（記憶值） / Capability（可見性） 同時存在

✓ **Discovery-First 比 Quest-First 更持久**：玩家因為好奇心繼續探索，不是因為任務

✓ **Adventure Mapping 可行**：Settlement → World 這樣的翻譯在實作上順暢

✓ **文字媒介的 Presentation 分離**：Runtime 不需要知道「這是文字」，只需要知道「reveal_block_id」

### 待驗證（未完成）

? **多玩家的世界狀態同步**：Formosa 的記憶值是全局的還是個人的？

? **時代推進的不可逆性**：玩家能否讓時代「退回」？

? **失源者的密度控制**：記憶值衰退的速率要如何設計才不會讓玩家感到絕望？

---

## Formosa 教會了 Runtime 什麼

1. **Stage 不只是線性的**：Formosa 的時代是線性的，但踏印可以在任何時代任何地點發生——Runtime 需要支援非線性的 Factor 解鎖。

2. **Confusion 需要有形化**：失源者的設計告訴我們，「遺忘」不能只是數字衰退，它必須在世界上有可感知的形式。

3. **World Progress 可以退步**：Formosa 的記憶值衰退機制，讓我們意識到 Progress 不只是前進，也可以後退——Runtime 需要支援這種雙向性。

---

## 目前實作狀態

```
LPC 基底：已完整
  settlement_d.c  ✓
  site_d.c        ✓
  memory_d.c      ✓（Discovery 的實作，名字叫錯了）
  reveal_layer.c  ✓
  timeline_d.c    ✓

YAML 內容：部分完成
  嘉義地區：進行中
  全台其他地區：待設計

FSE Runtime 整合：進行中
```

---

## Related

→ **Philosophy**: [08-Mapping](../philosophy/08-Mapping.md) — Formosa 的 Mapping 設計原則

→ **Pattern**: [Progressive Reveal](../patterns/Progressive-Reveal.md) — 時間層是 Formosa 最獨特的 Reveal 維度

→ **ADR**: [ADR-0007 Adventure Laboratory](../adr/ADR-0007-Adventure-Laboratory.md) — Formosa 作為 Large-Scale Adventure 的定位

→ **Reflection**: [Lessons from Formosa](../reflection/Lessons-from-Formosa.md) — Formosa 教會了 FSE 什麼
