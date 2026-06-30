# Stage 07: 侏羅紀降臨 (Jurassic Arrival)

## 📖 故事與情境
你穿過狹窄的紅色岩壁裂縫，眼前豁然開朗。這是一個完全不同於三疊紀的宏偉高濕熱帶河谷。高大遮天的桫欏與銀杏樹在霧氣中若隱若現，地表傳來梁龍行進時如雷鳴般的震動。地表巨獸橫行，河流中更潛伏著致命的魚龍，你必須學會尋找堅韌的植物纖維，藉由燧石工具編織出能讓你擺脫地表威脅的抓鉤。

---

## 🗺️ 地圖結構
本階段圍繞著「侏羅紀河谷」展開：
*   **[侏羅紀河谷](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/jurassic_valley/room.yaml) (jurassic_valley)**：起點。茂密叢林、巨樹根系。
    *   ➔ **[蕨類三角洲](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/fern_delta/room.yaml) (fern_delta)**：巨型蕨類林（`go delta`）。可收集藥用蕨類治癒傷口。
    *   ➔ **[巨獸足跡](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/giant_footprint/room.yaml) (giant_footprint)**：梁龍腳印積水坑（`go footprint`）。可收集引火地衣。
    *   ➔ **[倒下的巨木](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/fallen_giant/room.yaml) (fallen_giant)**：中空巨木（`go log`）。可採集防水樹皮條。
    *   ➔ **[急流渡口](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/river_crossing/room.yaml) (river_crossing)**：寬闊急流（`go river`，需領悟 `water_boiling`）。
        *   ➔ **[黑曜石採石場](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/obsidian_quarry/room.yaml) (obsidian_quarry)**：黑色採石場（`go across`，需用簡易木筏渡河）。

---

## 🔍 解謎與領悟路徑 (Discovery Loop)

### 1. 採集與工具打製 (Fiber & Flints)
*   **素材採集**：在 `jurassic_valley` 專注觀察地面（`focus ground`），在桫欏巨樹的根部發現堅韌的植物纖維 (fibers)。
*   **石刃切割**：利用之前在三疊紀打製的燧石刀刃（`flint_knapping`）割下纖維（`cut fibers`）。

### 2. 抓鉤編織與高空攀登
*   **抓鉤製作**：將植物纖維擰成強韌的繩索，與砍伐的堅硬松枝彎鉤綁在一起，製成抓鉤（`make hook` / `make grappling_hook`）。
*   **擺脫地表**：手持簡易抓鉤，甩向高大桫欏樹的分枝進行攀爬（`climb tree` / `climb`）。
*   **領悟因子**：`canopy_climbing` (樹冠攀爬技術)。地圖上 reveal 出前往 `canopy_refuge` (樹冠避難所) 的上方通道（`go up`），你成功離開了危機四伏的地表！
