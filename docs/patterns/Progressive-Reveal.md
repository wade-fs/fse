# Pattern: Progressive Reveal

> **一句話**：玩家能看見的世界，由他理解了什麼決定。

---

## 模式概述

Progressive Reveal 描述的是：

**同一個地點，在不同理解深度的玩家眼中，是不同的世界。**

不是因為地點改變了，而是因為玩家改變了。

```
玩家 A（初次進入）：
  「一片雜草叢生的荒地。」

玩家 B（解鎖了「糖鐵興衰」）：
  「雜草下隱約露出一排鏽蝕的糖鐵軌道殘影。
   老站長站在遠處，朝你點了點頭。」

玩家 C（解鎖了「嘉南源流」+「乙未之夜」）：
  「四個時代的殘影同時在這片土地上疊合。
   你感覺到腳下的地層在振動——
   這裡曾經是一切的起點。」
```

---

## 為什麼需要 Progressive Reveal

因為**資訊的時機比資訊本身更重要**。

如果玩家在不理解某件事的情況下看到相關資訊，

這個資訊對他來說是**噪音**。

如果玩家在理解了某件事之後才看到相關資訊，

這個資訊對他來說是**線索**，甚至是**Aha moment**。

Progressive Reveal 確保：

**每一個資訊，都在玩家準備好接收它的時刻出現。**

---

## 三個 Reveal 層次

### 層次一：存在性 Reveal（這裡有什麼）

最基礎的 Reveal。

玩家能「看見」某個 Node 的哪些部分。

```
沒有解鎖任何 Factor：
  看見：基礎描述、可見的出口

解鎖了相關 Factor：
  看見：隱藏的 NPC、新的路徑、額外的細節
```

---

### 層次二：意義性 Reveal（這意味著什麼）

相同的事物，有了不同的意義。

```
不知道風向重要性：
  看見：「一陣風吹過。」

理解了「暴龍靠嗅覺」：
  看見：「一陣風從西南方吹來——
         正好吹向暴龍所在的方向。
         這不是個好時機。」
```

同樣的風，在理解之前是背景，在理解之後是警告。

---

### 層次三：可能性 Reveal（能做什麼）

理解改變了玩家的行動選項。

```
不知道迴圈終止條件：
  工具箱：[順序執行] [重複]（沒有 break）

解鎖了「迴圈需要終止條件」：
  工具箱：[順序執行] [重複] [直到...] [中斷]（新增）
```

---

## Reveal 的觸發條件

Reveal 由 Factor 驅動。

每個 Node 的每個 Reveal 層，都有對應的 Factor 前置條件：

```yaml
reveal_layers:
  - reveal_block: "hidden_npc_old_stationmaster"
    checks:
      - type: factor_discovered
        factor_id: "jianan_plain_exploration"

  - reveal_block: "historical_layer_1915"
    checks:
      - type: factor_discovered
        factor_id: "xilai_temple_incident"
      - type: factor_discovered
        factor_id: "japanese_colonial_industry"
```

條件可以組合：同時滿足多個 Factor，才觸發更深的 Reveal。

---

## 設計要點

**好的 Progressive Reveal 的特徵：**

```
1. 每個 Reveal 層，對「沒準備的玩家」是看不見的
2. 每個 Reveal 層，對「有準備的玩家」是有意義的
3. 新的 Reveal 引發新的問題，驅動下一個 Discovery Loop
4. 玩家不知道還有多少層沒看見——保持神秘感
```

**常見的設計失誤：**

```
✗ 所有資訊一開始就可見（沒有 Progressive）
✗ Reveal 的條件與內容不相關（Reveal 看起來很隨機）
✗ 最後一層 Reveal 之後，沒有新的問題（循環斷了）
✗ 玩家知道「我還有 X 個 Reveal 沒解鎖」（破壞神秘感）
```

---

## 與其他模式的關係

- **Discovery Loop**：每次 Discovery 都驅動新的 Reveal 更新
- **Hidden Knowledge**：Progressive Reveal 是 Hidden Knowledge 的呈現機制
- **Environmental Teaching**：環境本身也能做 Progressive Reveal（風、聲音、光線）

---

## Related

→ **Philosophy**: [07-Presentation](../philosophy/07-Presentation.md) — Reveal 的哲學層定義

→ **Adventure**: [Formosa](../adventures/Formosa.md) — 時間層作為 Reveal 維度，最豐富的 Progressive Reveal 實例
