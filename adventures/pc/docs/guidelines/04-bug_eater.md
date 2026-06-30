# Stage 04: 吃蟲人 (Bug Eater)

## 📖 故事與情境
你面臨著嚴重的飢渴威脅。史前河谷的低窪積水泛著詭異的藍綠色，那是有毒的藍藻。你必須掌握「以熱淨水」的離散物理物理法則，利用高溫卵石煮沸淨水；同時，為了生存，你必須吃下那些令人抗拒的史前巨大節肢動物，將它們轉化為安全的蛋白質。

---

## 🗺️ 地圖結構
本階段圍繞著「史前河床」與「孢子叢林」展開：
*   **[史前河床](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/triassic_riverbed/room.yaml) (triassic_riverbed)**：起點。有積水石坑、巨型蜻蜓。
    *   ➔ **[蘆葦沼澤](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/reed_marsh/room.yaml) (reed_marsh)**：下陷泥地（`go marsh`）。可採集革質恐龍蛋。
*   **[背光洞窟陰影](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/triassic_shade/room.yaml)**：
    *   ➔ **[孢子叢林](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/spore_grove/room.yaml) (spore_grove)**：高大蕈林（`go grove`）。可捕捉白色幼蟲。
    *   ➔ **[琥珀水池](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/amber_pool/room.yaml) (amber_pool)**：樹脂水池（`go amber`，需有 `thermodynamics`）。可採集黏性樹脂，並觀察昆蟲化石結構。

---

## 🔍 解謎與領悟路徑 (Discovery Loop)

### 1. 卵石淨水謎題 (Water Purification)
*   **飲水中毒的後果**：直接捧起河床藍藻水飲用（`lick water`/`drink water`），會中毒嘔吐並扣減生命值，但獲得了領悟線索。
*   **熱能轉導**：玩家必須先利用火堆將河床的卵石烤紅（`heat rocks` / `heat stones`），接著用木枝夾起紅熱的卵石投入積水石坑中（`drop rocks water` / `boil water`）。
*   **沸騰淨化**：水流嘶嘶作響翻滾沸騰，消滅有害藍藻與寄生蟲。玩家喝下乾淨的熟水，解鎖因子：`water_boiling` (水體煮沸技術)。

### 2. 生物烹飪與營養轉化
*   **生食的代價**：直接吞食生的巨蜈蚣肉或白色幼蟲（`eat grub`），會引發嚴重腹痛嘔吐並扣減 HP，觸發困惑 `grub_reaction` / `raw_egg`。
*   **高溫炙烤**：利用生起的營火將蟲肉或恐龍蛋穿在木枝上炙烤（`roast meat` / `roast egg`），油脂焦香，殺死寄生蟲，解鎖因子：`arthropod_nutrition` (節肢動物營養學)。再次食用熟蟲肉，飢餓感一掃而空，順利完成第一頓飯！
