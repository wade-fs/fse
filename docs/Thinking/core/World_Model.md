# FSE World Model (世界模型規範)
# Version: v0.1

> **「世界是隱喻的容器，也是因果規則的執行者。」**
> 本文件定義了 Flow State Engine (FSE) 中「世界模型 (World Model)」的通用資料結構與狀態轉移方程式，作為所有沈浸式冒險（如 fsmud, pw）的底層設計規範。

---

## 📊 一、世界狀態定義 (World State Tuple)

在 FSE 中，世界的任何一個「情境 (Situation)」都是由以下三個維度構成的唯讀元組 $S$：

$$S = (Site\_ID, \text{Global\_Progress\_Stage}, \text{Active\_World\_Events})$$

### 1. 空間維度 ($Site\_ID$)
*   代表玩家當前所處的物理或概念地標空間。
*   *實例*：
    *   `fsmud`: `minxiong_ghost_house` (民雄鬼屋)
    *   `pw (編程)`: `infinite_loop_swamp` (無窮迴圈沼澤)

### 2. 時間維度 ($\text{Global\_Progress\_Stage}$)
*   代表世界的宏觀進度階段。
*   *實例*：
    *   `fsmud`: `v0_2_sea_merchants` (海商紀)
    *   `pw (編程)`: `stage_2_conditional` (條件分支期)

### 3. 環境維度 ($\text{Active\_World\_Events}$)
*   代表當前正作用於世界的突發環境狀態或危機。
*   *實例*：
    *   `fsmud`: `typhoon_warning` (颱風警報/路線封鎖)
    *   `pw (編程)`: `memory_leak_warning` (記憶體洩漏/系統資源枯竭)
    *   `DW`:   
        * `predator_nearby`（捕食者靠近）  
        * `oxygen_critical`（氧氣不足）  
        * `night_falling`（黃昏，某些恐龍出沒）

---

## ⚙️ 二、行動與後果方程式 (Action & Consequence)

玩家的任何 `Act (行動)` 都是對當前世界狀態 $S$ 的一次函數調用，並返回一個 `Consequence (後果)`：

$$Act(Player\_Context, S) \to Consequence(Player\_Physical\_Change, Site\_State\_Change)$$

### 1. 玩家狀態後果 ($Player\_Physical\_Change$)
*   行動對玩家肉體或能力狀態造成的直接影響。
*   *實例*：
    *   *fsmud*：在「遺忘浪潮」侵蝕狀態下 `enter_haunted_house` ➔ 理智值扣除 30 ➔ 陷入精神錯亂。
    *   *DW (恐龍世界)*：在「白天且逆風」狀態下 `enter_forest` ➔ 氣味擴散驚動暴龍 ➔ 被捕食死亡。
    *   *pw (編程世界)*：在「單線程」狀態下 `request_api` ➔ 執行緒阻塞 ➔ 卡死。

### 2. 空間狀態後果 ($Site\_State\_Change$)
*   行動對當前地標環境造成的改變。
*   *實例*：
    *   *fsmud*：在大士爺廟 `place_ward_stone` (放置鎮符石) ➔ 地標淨化度 +10。
    *   *DW (恐龍世界)*：在恐龍巢穴 `steal_egg` (偷取恐龍蛋) ➔ 地標恐龍警戒值 +50。
    *   *pw (編程世界)*：在堆積記憶體區 `allocate_memory` ➔ 內存剩餘量 -20M。

---

## 🧩 三、發現（領悟）觸發判定 (Discovery Trigger)

Discovery（舊稱記憶碎片）的本質是**「前置知識」與「當前困惑」的交集判定**，不能被直接塞給玩家：

$$\text{Trigger}(Player\_History, Consequence) \to Discovery\_ID$$

### 1. 觸發流程模型
```
情境 (S) ➔ 玩家行動 (Act) ➔ 異常後果 (Consequence) ➔ 產生困惑 (Confusion) ➔ 探索重試 (Retry) ➔ Discovery 觸發
```

### 2. 判定邏輯
*   **前置理解 ($Player\_History$)**：玩家之前已經解鎖並理解的 Discovery 列表。
*   **異常後果 ($Consequence$)**：玩家在不了解規則時，遭遇的挫折（例如程序崩潰、溺水）。
*   **解鎖結果 ($Discovery\_ID$)**：當玩家再次於特定 Site 進行觀察或研究時，若滿足前置條件且經歷過相應挫折，則正式領悟該項知識（如「三疊紀的低氧機制」或「變數生命週期」）。

---

## 🌪️ 四、系統熵值與危機 (Entropy & Confusion)

為了維持心流學習的張力，FSE 引入了「系統熵值（混亂度）」機制：

1.  **定期衰減**：隨時間推移，地標或系統的健康度會因「熵增」而自然衰退。
    *   *fsmud*: 歷史被遺忘的進度衰退。
    *   *pw (編程)*: 系統積累的 Bug 數量上升。
2.  **混亂實體化 (Specter)**：當熵值高於臨界點，地標會生成「阻礙物（意念殘影/Bug 實體）」，限制玩家通行，直到玩家使用相應的 Discovery 將其修復（Resonance/Debug）。
