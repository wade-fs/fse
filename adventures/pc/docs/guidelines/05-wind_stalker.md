# Stage 05: 風中潛行者 (Wind Stalker)

## 📖 故事與情境
你被逼入了一條死寂的紅色峽谷。在稀疏的史前灌木與碎石堆中，一頭巨大的三疊紀頂級獵食者——赫雷拉龍（Herrerasaurus）正在四處巡邏。牠的雙眼能輕易捕捉高速移動的物體，且鼻腔龐大，能嗅出上風處獵物的氣味。你必須學會利用風向與偽裝，隱匿自己的存在。

---

## 🗺️ 地圖結構
本階段圍繞著「捕食者峽谷」展開：
*   **[捕食者峽谷](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/predator_canyon/room.yaml) (predator_canyon)**：起點。赫雷拉龍巡邏區。
    *   ➔ **[沙塵暴邊緣](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/dust_storm_edge/room.yaml) (dust_storm_edge)**：漫天風沙區（`go dust`）。可藉由沙塵掩護避開恐龍，並收集燧石。
    *   ➔ **[隱密峽縫](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/hidden_ravine/room.yaml) (hidden_ravine)**：極窄岩縫（`go ravine`，需領悟 `stealth_camouflage`）。古人類手印標記地。
        *   ➔ **[化石峭壁](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/fossil_cliff/room.yaml) (fossil_cliff)**：沉積岩峭壁（`go cliff`）。可研究菊石化石，或攀爬至高處。

---

## 🔍 解謎與領悟路徑 (Discovery Loop)

### 1. 氣流與感知（風向的約束）
*   **盲目奔跑的後果**：若玩家身上帶有獸腥味，且處於上風處直接走進峽谷，會立刻被赫雷拉龍發現並撲殺（扣大量 HP）。
*   **風向觀測**：執行 `focus wind` 感受風向，並閱讀環境描述：「東南風颳過，將你身上的氣味帶往了前方。」

### 2. 靜止偽裝與潛行 (Stealth & Stillness)
*   **觀察地面震動**：在 `triassic_riverbed` 貼地聆聽地鳴（`focus sound`），理解震動波的傳導，領悟因子：`vibration_translation` (地面震動翻譯)。
*   **靜止與隱蔽**：在 `predator_canyon` 尋找灌木叢，採取靜止不動的方式（`hide` / `crouch`），將身體貼在紅砂岩的避風陰影中。
*   **領悟因子**：`stealth_camouflage` (靜止隱蔽偽裝)。
*   **解鎖通道**：解鎖潛行後，玩家的視野中會 reveal 出一條通往 `hidden_ravine` (隱密峽縫) 的窄道，成功避開赫雷拉龍！
