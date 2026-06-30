# Stage 11: 白堊紀黎明 (Cretaceous Dawn)

## 📖 故事與情境
你降落在一片宏偉的史前白堊紀海岸。這裡海浪拍打著岩礁，而在不遠處的紅樹林與灌木叢中，史前最恐怖的陸地掠食者——霸王龍（Tyrannosaurus rex）正發出震耳欲聾的咆哮，搜尋著任何有動態感官或氣味特徵的生物。你必須依靠之前學會的所有靜止與潛行本領，製作簡陋工具，安全穿過牠的領地，抵達人類早期聚落。

---

## 🗺️ 地圖結構
本階段圍繞著「白堊紀海岸」與「聚落山脊」展開：
*   **[白堊紀海岸](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/cretaceous_shore/room.yaml) (cretaceous_shore)**：起點。霸王龍遊蕩海岸。
    *   ➔ **[紅樹林海岸](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/mangrove_shore/room.yaml) (mangrove_shore)**：茂密紅樹林（`go mangrove`）。可採集海鮮、生魚。
    *   ➔ **[潮汐水池](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/tide_pool/room.yaml) (tide_pool)**：岩礁水坑（`go pool`）。可收集海星，但喝海水會更渴。
    *   ➔ **[聚落山脊](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/settlement_ridge/room.yaml) (settlement_ridge)**：原始聚落（需安全避開霸王龍）。
        *   ➔ **[山脊瞭望台](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/ridge_lookout/room.yaml) (ridge_lookout)**：最高點瞭望台（`go lookout`）。可觀察地貌與恐龍遷徙規律。

---

## 🔍 解謎與領悟路徑 (Discovery Loop)

### 1. 霸王龍超感官避難 (T-Rex Evading)
*   **驚動受傷的後果**：當霸王龍發出狂吼並踩下巨大腳印時，玩家如果選擇跑步或隨意查看，會被霸王龍敏銳的感官察覺並遭受撕咬，生命垂危。
*   **塗抹掩蔽氣味**：玩家必須先在海岸邊尋找濕潤的史前海藻泥或泥沙（`focus ground`），將泥沙塗抹在全身以掩蓋自己溫熱的哺乳動物體味（`apply mud` / `wear mud`）。
*   **極限靜止**：在霸王龍巡視的瞬間保持動態靜止（`freeze`），像一塊冰冷的岩石般融入泥沙背景。
*   **繞行突破**：藉由泥沙屏蔽氣味與靜止避開視覺，執行 `sneak` 緩慢繞行霸王龍，解鎖通往 `settlement_ridge` (聚落山脊) 的道路！

### 2. 原始聚落與石器工藝
*   **採集燧石**：在山脊邊緣採集碎石，獲得高品質燧石（`raw_flint`）。
*   **打製石刃**：利用另一塊硬岩反覆敲擊燧石的解理面，剝落邊角，打製出一柄極其鋒利的燧石石刀（`knap flint` / `make blade`）。
*   **領悟與晉升**：成功打製石器，解鎖因子：`flint_knapping` (燧石打製技術)，你帶領著穿越的古人類在山脊上站穩了腳步。
