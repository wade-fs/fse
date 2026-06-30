# Stage 06: 大氣與對流考驗 (Convection Navigation)

## 📖 故事與情境
你被困在了極度缺氧的地下火山管線中，四周充斥著劇毒的二氧化硫氣體，生命危在旦夕。而在跨越管線後，迎接你的卻是一片漫天黃沙、能見度幾近為零的死亡鹽鹼風暴。你必須學會利用空氣熱對流尋找新鮮空氣夾層，並藉由晶石的天然偏光性質看穿風暴。

---

## 🗺️ 地圖結構
本階段圍繞著三疊紀晚期最惡劣的火山地表展開：
*   **[風切大峽谷](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/wind_canyon/room.yaml) (wind_canyon)**：起點。大風風洞。
    *   ➔ **[火山熔岩管](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/lava_tube/room.yaml) (lava_tube)**：地熱地下通道（`go forward`，需領悟 `convection_currents`）。
        *   ➔ **[岩漿台地](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/magma_shelf/room.yaml) (magma_shelf)**：熔岩流台地（`go shelf`，需領悟 `convection_currents`）。
    *   ➔ **[鹽鹼荒灘](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/alkali_flats/room.yaml) (alkali_flats)**：致命沙塵暴區（`go flats`，需領悟 `convection_currents`）。
        *   ➔ **[火山灰原](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/ash_field/room.yaml) (ash_field)**：灰白色灰層（`go ash`）。可在此挖掘植物化石。

---

## 🔍 解謎與領悟路徑 (Discovery Loop)

### 1. 火山管熱對流引導 (Convection current)
*   **窒息後果**：在缺氧的熔岩管（`lava_tube`）中快跑（`run`）或在毒氣中等待（`wait`），會迅速空氧窒息致死並被送回大峽谷起點。
*   **對流觀測**：玩家必須確保身上攜帶了燃燒的 `embers` (火種)。在熔岩管中執行 `focus wind`，會看見火種產生的青煙正朝著岩壁縫隙劇烈地「朝上偏折」——這意味著裂縫背後連接著富氧的外界大氣，產生了熱對流。
*   **吸取氧氣**：執行 `harness convection`（利用熱對流），大口吸入夾層流入的富氧空氣，解鎖因子：`convection_currents` (熱對流引導)。地圖顯現通往外界 `alkali_flats` (鹽鹼荒灘) 的出口！

### 2. 沙塵暴偏振導航 (Polarization Navigation)
*   **迷失後果**：在漫天風沙的 `alkali_flats` 中直接移動（`go north` / `go south`），會因為毫無參照物而在風沙中打轉，最終精疲力竭並被砂石割傷（扣 HP）。
*   **收集偏光介質**：在 `wind_canyon` 的避風岩縫中專注觀察地面（`focus ground`），發現並拾取 `mica` (偏振雲母石片)。
*   **偏光濾砂**：在沙塵暴中將雲母石片舉到眼前觀測天空（`look clouds through mica` / `look sky through mica`）。晶體結構過濾了黃沙雜光，天空中浮現出了深淺分明的偏振光偏光條紋！
*   **磁極定位**：根據條紋校對出南北磁極方向，解鎖因子：`polarization_navigation` (偏振導航)。漫天黃沙退去，隱約現出了通往 `jurassic_valley` (侏羅紀谷地) 的出口！
