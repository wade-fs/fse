# 《史前文明》新手教學與新手村闖關機制說明

本文件說明《史前文明》(PC) 模組的新手村設計哲學、通關路徑以及資料驅動的生還者防護與復活機制。

---

## 一、新手村安全區的四大相連空間

新手村（`novice` 階段）由以下四個實體相連的空間組成，玩家在此區域內可安全探索：

```mermaid
graph TD
    triassic_plains["三疊紀荒原 (triassic_plains)<br>【生火起點】"]
    fern_forest["巨型蕨類森林 (fern_forest)<br>【打製石器】"]
    dark_cave["黑暗溶洞 (dark_cave)<br>【解讀壁畫】"]
    predator_canyon["捕食者峽谷 (predator_canyon)<br>【逆風逃脫】"]
    desert_canyon["乾燥峽谷 (desert_canyon)<br>【Day Zero 起點】"]

    triassic_plains <-->|go forest| fern_forest
    fern_forest <-->|go cave (需火光)| dark_cave
    fern_forest <-->|go canyon (需石刃)| predator_canyon
    predator_canyon -->|go desert (通關開啟)| desert_canyon
```

---

## 二、新手任務通關與 Stage 晉升路徑

玩家必須活用環境中的感官資訊，逐步累積 Factor (領悟) 才能脫離新手村：

1. **鑽木取火**：在 `triassic_plains` 專注地面 (`focus ground`) 發現乾枯根部，與樹枝摩擦 (`rub branches roots`) 點燃營火，領悟 **`thermodynamics` (摩擦生熱與引火)**。
2. **打製石器**：前往 `fern_forest`，專注地面發現黑曜石並進行敲擊 (`strike obsidian obsidian`)，獲得黑曜石石片，領悟 **`flint_knapping` (打製石器)**。
3. **照亮溶洞**：手持火種前往 `dark_cave` 點燃柴火 (`burn wood`)，解鎖 **`cave_painting`**。接著觀察牆上壁畫 (`look painting`)，理解風向規律，領悟 **`wind_direction` (風向與氣味傳播)**。
4. **逆風潛行**：使用石刃割開荊棘屏障，進入 `predator_canyon`。利用感官 (`focus wind`) 察覺風向後，使用 **`hide downwind`** 塗抹泥土並藉由逆風掩蓋氣味，成功繞過 Herrerasaurus 逃脫。

完成逆風逃脫將解鎖 **`stealth_camouflage` (隱蔽偽裝與潛行)** 並完成 **`first_escape`** 任務。進度管理器會隨即將玩家的 Stage 晉升至 `day_zero`，並在 `predator_canyon` 顯現通往 **`desert_canyon` (乾燥峽谷)** 的出口！

---

## 三、新手保護機制 (Safe Haven)

為鼓勵玩家在不受打擊的情況下學習物理交互（例如測試 `kick rocks` 的反衝力，或是 `lick water` 的毒性試驗）：
* 只要玩家置身於新手村的 4 個房間內，**任何環境致死或扣減生命值的懲罰都不會導致角色死亡**。
* 當 HP 扣減至 0 或以下時，系統會自動攔截並將 **HP 鎖定為 1**，並跳出警告提示玩家：「*新手保護機制生效：你在安全區內力竭，生命值降為 1，並未死亡。*」

---

## 四、資料驅動的階段性墓地復活機制 (Stage Respawn)

一旦玩家跨出新手村進入 `desert_canyon`，環境致死機制（如熱調節、低氧奔跑昏厥、二氧化碳中毒）便會全面開啟。

為了讓老玩家死亡後不需重新跑新手村，系統實作了**完全資料驅動的階段性墓地機制**：
* 玩家死亡 (`on_death`) 或登入載入世界 (`_enter_world`) 時，不再寫死特定的 Factor 判定。
* 系統會直接讀取玩家當前 Stage 檔案中所定義的 **`spawn_node`** 作為復活與出生起點：
  * **`novice` 階段**：復活於 `triassic_plains` (荒原)。
  * **`day_zero` 階段**：復活於 `desert_canyon` (乾燥峽谷)。
  * **`first_night` 階段**：復活於 `triassic_shade` (背光洞窟陰影)。
