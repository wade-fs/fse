# 06. Quest System - 任務與引導系統 (Quest System)

《源流福爾摩沙》的任務系統承載著引導玩家理解土地歷史、與各大在地勢力互動的核心體驗。

---

## 🗺️ 任務類型與實作範本

### 1. 新手引導任務 (Tutorial Quest)
*   **代表範本**：「老站長的心願」（[tutorial_quest.md](file:///home/wade/src/github/FormosaSaga/docs/quests/tutorial_quest.md)）。
*   **流程設計**：探索民雄市場獲得「糖鐵踏印」 ➡️ 回報給民雄老車站月台的老站長 ➡️ 獲得獎勵。作為玩家的第一個遊戲循環引導。

### 2. 團隊勢力委託任務 (Faction/Co-op Quests)
*   **劉家地契委託**（[liu_deed_recovery.md](file:///home/wade/src/github/FormosaSaga/docs/quests/liu_deed_recovery.md)）：要求等級 7。在民雄鬼屋與同伴尋找暗格，取回遺失的地契，增加劉家聲望。
*   **糖鐵搶修委託**（[sugar_railway_repair.md](file:///home/wade/src/github/FormosaSaga/docs/quests/sugar_railway_repair.md)）：要求等級 6。在甘蔗林合作搶修脫軌的五分車，增加糖業聲望。
*   **廟委鎮煞委託**（[temple_exorcism.md](file:///home/wade/src/github/FormosaSaga/docs/quests/temple_exorcism.md)）：要求等級 5。在大士爺廟察覺地下陣法，進入秘境收集鎮符石，獲得大士爺護符與廟委聲望。

---

## 🛠️ 開發規範
所有委託任務均需撰寫與其同名的整合測試檔（如 `test_quest_loop.c`、`test_temple_exorcism.c`），並模擬玩家移動、取得踏印、回報、增長聲望的完整事件分發。
