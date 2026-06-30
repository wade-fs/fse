# Stage 09: 異特龍考驗 (Allosaurus Gauntlet)

## 📖 故事與情境
你深入了恐龍遷徙的狹窄走廊。一頭體型龐大、行動敏捷的史前掠食者——異特龍（Allosaurus）正在前方必經之路的開闊地帶四處遊蕩。異特龍對動態視覺極度敏感，任何風吹草動都會引起牠的狂暴追擊。你必須學會「極限靜止」與環境色融為一體，並在倒木迷宮中巧妙周旋。

---

## 🗺️ 地圖結構
本階段圍繞著「遷徙走廊」展開：
*   **[遷徙走廊](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/migration_trail/room.yaml) (migration_trail)**：起點。異特龍遊蕩區。
    *   ➔ **[史前水坑](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/watering_hole/room.yaml) (watering_hole)**：恐龍飲水點（`go water`）。可研究異特龍的三趾腳印。
    *   ➔ **[高草叢](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/tall_grass/room.yaml) (tall_grass)**：高大草堆（`go grass`）。可在此臥倒偽裝，採集恐龍蛋。
    *   ➔ **[倒木裂口](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/deadfall_gap/room.yaml) (deadfall_gap)**：巨木迷宮（`go gap`）。可在此利用地形避開異特龍追擊。
    *   ➔ **[翼龍峭壁](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/pterosaur_cliff/room.yaml) (pterosaur_cliff)**：峭壁平台（`go cliff`，需成功穿過走廊）。

---

## 🔍 解謎與領悟路徑 (Discovery Loop)

### 1. 肉食恐龍的動態視覺 (Motion & Senses)
*   **奔跑被捕的後果**：當異特龍注視玩家時（`allosaurus looks at you`），如果玩家選擇移動（`go`）或奔跑，異特龍會立刻鎖定玩家並發起致命撲殺（扣 HP）。

### 2. 極限靜止偽裝 (Motion Stillness)
*   **屏息靜止**：在異特龍轉頭看過來時，立即執行 `freeze`（屏息靜止），將大腦發出的微弱動態訊號降至零，將心跳和身形隱藏在環境背景中。
*   **領悟因子**：`motion_stillness` (動態靜止原理)。
*   **安全潛伏**：在成功掌握靜止原理後，利用走廊兩側的岩縫或高草叢，趁異特龍轉頭的一瞬間緩慢潛行挪動（`sneak`）。
*   **成功穿越**：玩家無聲無息地繞過異特龍，揭示了前往前方 `pterosaur_cliff` (翼龍峭壁) 的出口！
