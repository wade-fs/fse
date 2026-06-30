# 11. Historical Incident System - 編年史與地方誌系統 (Historical Incident System)

歷史大事紀與地方誌系統，提供文人玩家將全服玩家的開拓足跡與救贖歷程編寫成永久傳世風土誌的機制。

---

## 📜 歷史大事紀 (Historic Records)
*   **自動搜集**：系統會自動透過事件訂閱（`MemoryCompleted` 與 `SpecterResolved`），將「某玩家在某日解鎖了某記憶」或「某玩家成功共鳴解除了失源者危機」記錄在該聚落的大事紀中。
*   **大事紀檢視**：文人可使用 `record write` 檢視，作後續地方誌創作的素材。

---

## ✍️ 地方誌發表與研讀 (Chronicles)
*   **發表**：文人可輸入 `record publish <標題>:<內容>` 將創作的地方誌公開收錄至當地文獻中，為聚落提升 3% 記憶度。
*   **研讀與獎勵**：玩家可透過 `record list` 與 `record read <編號>` 研讀他人編寫的地方誌，讀者在首次閱讀每篇地方誌時會獲得 2 點文士點數與 5 點經驗，並為聚落帶來額外 1% 的記憶度提升。
