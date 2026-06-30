# Stage 01: 初來乍到 (Novice)

## 📖 故事與情境
你從冰冷的沉眠中醒來，四周是一片荒涼而悶熱的史前橘紅色荒原。熱空氣扭曲了地平線，硫磺的氣味不斷灌入鼻腔。你一無所有，赤手空拳，大雨將至。在失溫和窒息前，你必須掌握熱力學原理，升起人類文明的第一堆篝火。

---

## 🗺️ 地圖結構
本階段圍繞著「三疊紀荒原」展開：
*   **[三疊紀荒原](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/triassic_plains/room.yaml) (triassic_plains)**：起點。地表極熱，有隱約的蕨類植物根部。
    *   ➔ **[岩石突丘](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/rocky_outcrop/room.yaml) (rocky_outcrop)**：位於突丘高處（`go hill`）。可收集硫磺草，並觀測高空對流。
    *   ➔ **[骸骨場](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/bone_yard/room.yaml) (bone_yard)**：石化骨堆（`go bones`）。可取得骨骸碎片並磨尖製成骨刀，在此可首次感受捕食者氣息。
    *   ➔ **[硫磺溫泉](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/sulfur_spring/room.yaml) (sulfur_spring)**：地熱泉眼（`go spring`，需領悟 `heat_regulation`）。可刮取古老生命嗜熱古菌薄膜。

---

## 🔍 解謎與領悟路徑 (Discovery Loop)

### 1. 認知落差與困惑 (Epistemic Gap)
*   玩家試圖用普通松枝直接磨擦地表（`rub branches rocks`），卻一無所獲（扣除體力），引發困惑：在潮濕陰冷的前夕，該拿什麼作為引火的乾燥火絨？

### 2. 感官觀測 (Observation)
*   在 `triassic_plains` 執行 `focus ground`（專注觀察地面），發現岩石縫中乾枯的耐旱蕨類根部 (roots)。

### 3. 物理互動與領悟 (Aha! Moment)
*   **摩擦生火**：執行 `rub branches roots` 或 `rub roots branches`。
*   **領悟因子**：`thermodynamics` (熱力學規律)。
*   **獲得物品**：`embers` (乾燥的火種)。
*   **地圖顯現 (Reveal)**：生火的火光與暖意照亮了黑暗，Reveal 通往 `fern_forest` (巨型蕨類森林) 的出口！
