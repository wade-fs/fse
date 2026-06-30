# Stage 08: 樹冠建築師 (Canopy Builder)

## 📖 故事與情境
你來到了百公尺高空的樹冠層。這裡強勁的高空橫風不斷搖晃著樹梢，下方的深淵深不可測。為了在此建立一個長期的、能防風禦寒的避難所，你必須理解結構力學與風阻規律。你需要在巨樹的分叉間搭建木屋平台，並學會安全穿越搖晃的藤蔓吊橋。

---

## 🗺️ 地圖結構
本階段圍繞著「樹冠避難所」與「藤蔓吊橋」展開：
*   **[樹冠避難所](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/canopy_refuge/room.yaml) (canopy_refuge)**：起點。高空巨樹樹冠。
    *   ➔ **[上層樹冠](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/upper_canopy/room.yaml) (upper_canopy)**：最高樹梢（`go up`，需領悟 `canopy_climbing`）。可在此馴服幼翼龍。
    *   ➔ **[藤蔓吊橋](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/vine_bridge/room.yaml) (vine_bridge)**：高空吊橋（`go bridge`，需領悟 `structural_load`）。
        *   ➔ **[鳥類棲息地](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/bird_roost/room.yaml) (bird_roost)**：始祖鳥巢穴（`go roost`，需加固吊橋）。可在此收集斑點鳥蛋。

---

## 🔍 解謎與領悟路徑 (Discovery Loop)

### 1. 高空防風與力學分析 (Wind & Load)
*   **物理觀察**：在 `canopy_refuge` 專注感知風力（`focus wind`），觀察高空橫風的流向，解鎖因子：`altitude_wind` (高空風向規律)。
*   **纖維收集**：在樹幹周圍尋找粗壯的寄生藤蔓並用石刃割下（`cut vines`），獲得高強度纖維。
*   **扭轉補強**：玩家透過扭緊多股藤蔓來抵消纖維本身的剪力偏差（`twist vines`），解鎖因子：`fiber_strength` (纖維強度特性)。

### 2. 構築平台避難所
*   **力學結構搭建**：利用強度補強的藤蔓繩索與樹幹建立三角形的穩固支架結構，搭建高空防風平台（`build platform` / `build shelter`）。
*   **領悟因子**：`structural_load` (結構力學載重)。避難所構築完成，獲得安全防護！
*   **吊橋加固謎題**：在 `vine_bridge` 中直接走過去（`cross bridge`），若沒有解鎖 `structural_load`，吊橋會直接斷裂墜落致死。解鎖力學後，玩家需先用粗藤蔓加固老舊吊橋（`reinforce bridge`），才能安全通行前往始祖鳥巢穴。
