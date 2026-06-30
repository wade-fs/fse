# Stage 03: 第一夜 (First Night)

## 📖 故事與情境
夜幕降臨三疊紀荒原。史前荒野的日夜溫差高達 50°C，寒風如刀，你的體溫正在迅速流失。除了低溫威脅，黑暗中巨型節肢動物（如巨蜈蚣）的節奏性爬行聲在岩縫間迴響。你必須找到背光處，點燃一堆可以持續燃燒的篝火，並在生存壓力下學會辨識危險。

---

## 🗺️ 地圖結構
本階段圍繞著「背光洞窟陰影」與「史前河床」展開：
*   **[背光洞窟陰影](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/triassic_shade/room.yaml) (triassic_shade)**：起點。溫度宜人，但有巨蜈蚣棲息。
    *   ➔ **[余燼凹地](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/ember_hollow/room.yaml) (ember_hollow)**：古老火圈遺址（`go hollow`，需領悟 `thermodynamics`）。可用更低體力消耗生火。
    *   ➔ **[史前河床](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/triassic_riverbed/room.yaml) (triassic_riverbed)**：碎石河谷（`go riverbed`）。
        *   ➔ **[晶石滲流](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/crystal_seep/room.yaml) (crystal_seep)**：礦物岩壁（`go seep`）。可採集用於作畫的赭石顏料，並喝到安全純淨的水。

---

## 🔍 解謎與領悟路徑 (Discovery Loop)

### 1. 安全生火與避難
*   **物理互動**：使用前一階段獲得的 `embers`（火種）或是再次利用乾燥蕨類根部摩擦，在避難所升起篝火（`light campfire` / `rub branches roots`）。
*   **領悟與晉升**：成功生火後，執行 `rest`（休息）完成 `survive_day_zero` 任務，在溫暖中熬過冰冷的第一夜。

### 2. 生態防衛（失敗是信息）
*   **盲目狩獵的後果**：在未掌握工具打製時，直接赤手空拳捕捉巨蜈蚣（`hunt megapede`），會被其劇毒齶牙咬傷（扣 HP）並陷入困惑，解鎖因子：`arthropod_warning` (節肢動物警戒色)。
*   **正確狩獵**：在解鎖 `flint_knapping` (燧石打製) 後，利用石片與木棒保持距離將其擊殺，獲得 `bug_meat` (生蟲肉)。
