# Stage 12: 最後的文明 (Last Stand)

## 📖 故事與情境
小行星即將撞擊地球，地表將陷入零下 40°C 的永久撞擊冬夜與強烈電磁脈衝 (EMP) 橫掃的地獄。為了不讓人類剛剛萌芽的智慧火種隨著恐龍一同消逝在歷史的灰燼中，你必須帶領族人深入古老的祖先洞穴。你需要在地熱裂隙旁燒製可以長期保存的陶器，建立神聖祭壇，並用黑曜石在不會被侵蝕的黑岩壁上刻蝕下文明的符號。

---

## 🗺️ 地圖結構
本階段圍繞著「祖先洞穴」展開的地下密室網絡：
*   **[祖先洞穴](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/ancestor_cave/room.yaml) (ancestor_cave)**：起點。地熱裂隙、祭壇遺址。
    *   ➔ **[洞穴畫廊](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/cave_gallery/room.yaml) (cave_gallery)**：原始壁畫走廊（`go gallery`，需領悟 `cave_painting`）。
    *   ➔ **[地下泉水](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/underground_spring/room.yaml) (underground_spring)**：地下水源（`go spring`）。可採集純淨水並觀察洞穴盲眼生物。
    *   ➔ **[知識保存庫入口](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/vault_entrance/room.yaml) (vault_entrance)**：文明地下避難所（`go vault`，需領悟 `information_preservation`）。

---

## 🔍 解謎與領悟路徑 (Discovery Loop)

### 1. 冶窯燒陶與水源保存 (Clay & Pottery)
*   **黏土採集**：在 `settlement_ridge` 附近的河谷岸邊採集高塑性黏土（`collect clay`）。
*   **塑形陶坏**：將黏土揉捏，塑造成可以裝水的罐狀陶坏（`mold clay` / `make pot`）。
*   **高溫煆燒**：將黏土陶坏置入祖先洞穴的地熱熱泉或高溫熔岩裂縫中進行焙燒（`burn pot` / `bake pot`）。
*   **領悟與獲物**：高溫將黏土中的矽酸鹽燒結相變，解鎖因子：`pottery_craft` (陶器製作工藝)，獲得可以長期儲水和防潮的 `clay_pot` (陶罐)。

### 2. 符號傳承與知識保存 (Information Preservation)
*   **祭壇建立**：在洞穴中央用石塊搭建神聖祭壇（`build altar`），將陶罐置於其上，凝聚族人信仰，解鎖因子：`ritual_altar` (祭壇儀式)。
*   **刻蝕壁畫**：用刮取的 Ochre（赭石顏料）在石壁上繪製壁畫（`paint wall` / `draw`），記錄星辰與求生歷史，解鎖因子：`cave_painting` (壁畫記錄)。
*   **抗老化介質選擇**：為了抵擋末日火雨、極寒與強電磁脈衝，玩家面臨選擇：該用什麼記錄知識？用骨頭會腐爛，用金屬會氧化，唯有黑曜石與碳素在岩壁上的物理刻蝕能經歷億萬年。
*   **永存刻蝕**：用鋒利的黑曜石石片在石英硬岩上雕刻符號（`carve symbols` / `write history`）。
*   **領悟因子**：`information_preservation` (信息永存技術)。
*   **避難所封鎖**：解鎖信息永存後， reveal 出前往 `vault_entrance` 的出口。解開機關，將火種封存，人類文明成功逃過了末日浩劫，等待下一個紀元的甦醒！
