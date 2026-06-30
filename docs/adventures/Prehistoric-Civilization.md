# Adventure: Prehistoric Civilization（PC / Dinosaur World）

> PC 是 FSE 的第三個構想中的 Adventure。
> 它的存在，主要是為了測試 FSE 在「完全不同的知識領域」下是否仍然成立。

---

## 一句話

Prehistoric Civilization 是一個三疊紀到白堊紀的世界。

玩家是一個穿越到中生代的觀察者，

沒有武器，沒有超能力，

只有觀察、推斷、和對生態系統的逐漸理解。

生存的唯一方式是：**理解這個世界的規則。**

---

## 為什麼 PC 很重要

Formosa 驗證了「歷史知識」可以用 FSE 的方式傳遞。

Programming World 驗證了「程式邏輯」可以用 FSE 的方式傳遞。

但有人會問：**FSE 對自然科學知識也有效嗎？**

PC 回答這個問題。

古生物學、地質學、生態學——

這些知識能不能透過「在恐龍世界裡行動、死亡、重試」而習得？

PC 的設計目的，就是證明：**可以。**

---

## PC 的 Mapping

| PC 概念 | FSE Runtime 概念 | 說明 |
|---|---|---|
| Era（地質年代） | Stage | 地質年代的推進是 Progress 的里程碑 |
| Biome（生態區） | Node | 每個生態區是可互動的節點 |
| Ecological Knowledge | Factor | 每個生態知識點是一個已解鎖的理解 |
| Predator Encounter | Confusion Event | 死亡/遭遇是 Confusion 的具體化 |
| Environmental Signal | Reveal Layer | 玩家能「感知」到的環境資訊 |
| Observation（觀察） | Actor 行動 | 觀察是 PC 裡最核心的行動 |

---

## PC 的特殊設計：環境訊號作為媒介

PC 的 Presentation 媒介是：**環境感知**。

```
不是文字說明，不是積木，而是：
  風向（會影響氣味傳播方向）
  聲音（捕食者的叫聲距離與方向）
  地面痕跡（腳印、糞便、植物損傷）
  光線（晝夜週期，某些恐龍的行動模式）
  溫度（體溫調節機制，影響恐龍的活動力）
```

玩家需要「讀取」環境的訊號，

才能在這個世界裡存活。

這驗證了 Environmental Teaching 模式的可行性。

---

## PC 的 Discovery 範例

### 風向與嗅覺

```
情境：看見暴龍在遠處（看起來很遠）
行動：繼續向前走
後果：暴龍衝過來，玩家死亡
困惑：「我明明走得很遠？」
Retry：第二次，注意到風從玩家吹向暴龍
Discovery：「暴龍靠嗅覺，不靠視覺。風向決定你的生死。」
Reveal：逆風路線出現，高危區域的邊緣可以進入
```

### 低氧環境

```
情境：三疊紀的平原，看起來很空曠
行動：全速跑
後果：兩步就喘，停下來
困惑：「我平常跑步不會這樣？」
Discovery：「三疊紀氧氣只有 15%，這個星球還沒準備好讓哺乳類呼吸」
Reveal：行動策略改變（走慢一點，保存體力）
```

### 暴龍的黃昏

```
情境：發現暴龍在黃昏後行動遲緩
行動：趁機靠近觀察
後果：沒有死，看見了暴龍巢穴
Discovery：「暴龍黃昏後視力退化，這段時間最安全」
Discovery 2：「暴龍其實會護雛，它的巢穴周圍有其他成員」
Reveal：夜間行動路線解鎖，暴龍行為的第二層觀察
```

---

## PC 的 Progress

### Knowledge Progress

生態知識的累積：

```
基礎感知（風向、聲音）
    ↓
捕食者行為（嗅覺、視覺、聽覺）
    ↓
生態關係（獵物與捕食者的平衡）
    ↓
環境規律（晝夜、季節、地質）
    ↓
進化機制（為什麼這個時代是這樣的）
```

### World Progress

生態系統的狀態：

```
玩家的行動影響生態平衡：
  頻繁進入某個 Biome → 該區域捕食者警戒值上升
  在巢穴附近活動 → 護雛行為啟動，危險等級提高
  觀察而不干預 → 生態系統維持穩定，更多可觀察的行為出現
```

### Capability Progress

能進入更危險的地區：

```
不知道任何生態知識
  → 只能在空曠的低風險區域活動

理解了風向和嗅覺
  → 可以在捕食者領域的邊緣活動

理解了晝夜行為規律
  → 可以在黃昏後靠近高危區域

理解了群居動物的警報機制
  → 可以在群居捕食者的巢穴附近觀察
```

---

## PC 驗證了什麼

### 待驗證（尚未實作）

? **環境訊號作為 Reveal Layer**：風向、聲音能不能成為 Runtime 標準的 Reveal 觸發器？

? **觀察作為行動**：純「觀察」（不做任何改變）能不能觸發 Discovery？

? **死亡的設計**：死亡後如何重置，但保留 Knowledge（玩家已理解的知識不消失）？

? **生態系統的動態性**：捕食者的行為是固定的還是動態的？如何實作而不讓規律變成「記憶遊戲」？

---

## PC 改變了 Runtime 的什麼

### 促使 Runtime 支援「觀察」作為獨立的行動類型

在 Formosa 和 PW 裡，行動都是「做某件事」（踏查、寫程式）。

PC 要求玩家能「只是觀察」——

觀察也會積累 Factor，改變世界對玩家的展示。

這讓 Runtime 重新思考：**Actor 的行動不只是「改變世界」，也可以是「感知世界」。**

### 促使 Runtime 支援「World Progress 的動態性」

PC 的生態系統不是靜態的。

捕食者在動，環境在變化，即使玩家不行動，世界也在改變。

這讓 Runtime 需要考慮：**Entropy 機制不只是「衰退」，而是「自然演化」。**

---

## 目前狀態

```
概念設計：完成
Mapping 定義：完成（本文件）
Runtime 驗證：尚未開始
YAML 內容：尚未設計
實作：待 Formosa 和 PW 穩定後開始
```

---

## Related

→ **Philosophy**: [08-Mapping](../philosophy/08-Mapping.md) — PC 的 Mapping 設計原則

→ **Pattern**: [Environmental Teaching](../patterns/Environmental-Teaching.md) — 環境訊號作為媒介的設計模式

→ **Pattern**: [Hidden Knowledge](../patterns/Hidden-Knowledge.md) — 生態知識完全藏在環境行為裡

→ **Reflection**: [What Dinosaur Taught Us](../reflection/What-Dinosaur-Taught-Us.md) — 設計 PC 改變了我們對 FSE 的哪些理解
