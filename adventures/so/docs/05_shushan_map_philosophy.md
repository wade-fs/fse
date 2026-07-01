# Shushan Map Philosophy (地圖哲學藍圖)
# 蜀山遊記：地圖與空間哲學

> "The Mind is the Map, and the Landscape is the Mindscape."
> —— 心即是圖，景即是心。

在《蜀山遊記》中，地圖不再是客觀、靜態的二維網格或座標，而是修行者內心境界與因果業力的投影。

---

## 一、 心境投影路徑 (Mindscape Exits)
地圖物理上的「路徑（Paths）」不是固定開啟的，而是由修行者的認知狀態決定：

* **境界未到，此路不存**：
  * 例如從「靜慮崖」通往「悟道林」的路徑。在物理上它一直都在，但在玩家沒有解鎖 `stillness_resonance` 之前，他的 `paths` 列表裡不會顯示這個出口。強行向該方向移動會觸發迷路（Evolve: lost_in_fog）。
  * 只有當心境與環境共鳴時，這條路徑才會透過 `Reveal Layer` 展現出來。

* **心魔鬼打牆 (Karma Loop)**：
  * 業力（Karma）過高或深陷執念的玩家，在試圖離開某個區域（如「煉魔谷」）時，不論輸入什麼方向，Paths 都會被動態重定向，使其不斷繞回原地。
  * 唯有通過特定的 Action（如打坐觀照、淨化業力）改變當前的認知狀態，真實的出口才會顯現。

---

## 二、 多維空間層疊 (Temporal & Spiritual Layering)
同一個物理地標（Site_ID），因修行者的理解深度不同，會展現出完全不同的維度空間：

* **凡俗維度 (Mundane Layer)**：
  * 看到的是世俗的物理景象（如：峨眉山頂的石碑、遊客、普通的廟宇）。
* **靈性維度 (Spiritual Layer)**：
  * 當玩家在 Memory 中存有 `spirit_perception` 時，同一個節點會 Reveal 出隱藏的「聚靈陣、古仙人留下的劍意刻痕」。
* **心魔試煉維度 (Demon Trial Layer)**：
  * 當玩家處於 `misconception: ego_delusion` 或業力極高時，同一個節點會演化成「黑風咆哮、幻影重重」的心魔試煉場，原有的路徑會全部消失，只剩下與心魔對峙的唯一挑戰。

---

## 三、 移動即因果檢驗 (Movement as Karma Evaluation)
在地圖上的空間位移，本身就是對修行者因果與業力的動態檢驗：

* **因果屏障 (Karmic Barriers)**：
  * 某些邊境節點（例如「生死橋」、「渡業河」）在 YAML 中關聯了 `spiritual.karma` 規則。
  * 業力值小於特定門檻的玩家可安全通過（Observe: 平靜無波）。
  * 業力深重者試圖強行通過時，會觸發 `Evolve: lightning_strike` (遭受天雷反噬，扣減靈力，並被震退回前一個節點)。

* **預測防禦 (Predictive Crossing)**：
  * 面對因果屏障，玩家的移動行動必須搭配正確的精神 Prediction。例如：在過橋前執行 `chant_purification` (誦持心經) 暫時平息心神。
  * Resolver 判定該 Predict 與橋上的法則對齊，便會 Reveal 出一條安全的避雷路徑，允許通過。
