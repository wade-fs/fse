# Shushan Entity Philosophy (實體與法寶哲學)
# 蜀山遊記：NPC 與法寶的心境投影

> "All entities are mirrors of the self. What you perceive is what you harbor."
> —— 萬物皆為自心之鏡。你所見者，皆你所藏。

在《蜀山遊記》中，修行者在同一個地點遇到的 NPC（人物）與 Item（物品/法寶），會隨著修行者的心境與因果狀態產生動態的 Reveal 與演化。

---

## 一、 NPC 作為心境之鏡 (NPC as a Spiritual Mirror)
NPC 不是固定不變的資訊發布器，而是修行者內心狀態的折射：

* **身分隨心境而顯隱 (Identity Shift)**：
  * 例如節點中的「砍柴人」：
    * 凡俗/初始狀態：他只是一個提供普通對話的「老翁」。
    * 業力深重/執念狀態：他 Reveal 為「心魔投影」，對話選項變為詰問與質疑，干擾玩家打坐。
    * 心境共鳴/清淨狀態：他 Reveal 為「指路仙翁」，開啟傳授心法或指點迷津的互動路徑。
  * 技術上，NPC 實體擁有動態的 `query_node_state` 適配器，其呈現的名稱、描述與對話行為完全取決於玩家當前的 `factors` 與 `observations`。

* **因果連鎖反應 (Karmic Reactions)**：
  * NPC 對玩家的態度不是基於「好感度數值」，而是基於玩家的業力（Karma）。
  * 業力清淨者，NPC 主動 Reveal 出友好與機緣；業力過高者，NPC 會閉門謝客，甚至 Reveal 出敵意。

---

## 二、 法寶與萬物之顯隱 (Item Reveal as Epiphany)
世間法寶皆有靈性，唯有認知對齊者能感知其真容並與其互動：

* **機緣顯現 (Spiritual Discovery)**：
  * 在「古劍塚」節點中，未悟劍道者只能看見「滿地生鏽的凡鐵殘渣」，執行任何撿拾動作都只能獲得廢鐵。
  * 當玩家在 Memory 中解鎖 `sword_affinity` (劍道共鳴) 後，廢鐵堆中的某一處會 Reveal 出「古劍遊龍的微弱光芒」，此時方可執行拔劍互動。

* **強求反噬 (Consequence of Force)**：
  * 若玩家在未獲得對應理解（Evidence / Observations 不足）的情況下，試圖強行拔取或使用高階法寶。
  * Reality Resolver 判定其 Predict 與法寶的靈性 Laws 產生衝突（Misconception）。
  * **Evolve 結果**：法寶靈光黯淡，玩家遭受劍氣反噬（扣減靈力或生命），且法寶在玩家的感知中徹底消失（隱入凡俗狀態）。

---

## 三、 物資環境之演化 (Environmental Resource Evolution)
節點中可採集或互動的物資，是該區域精神生態 Evolve 的下游結果：

* **靈性淨化 (Pure Field)**：
  * 玩家在某節點（如「靜慮崖」）以 `Understanding` 狀態完成修煉，會觸發節點磁場的淨化。
  * 節點 Evolve：環境產生 `herb_bloomed` 訊號，玩家在下一輪 Observe 中可以看見並採集到「朱果（spiritual_fruit）」。
* **業力污染 (Corrupted Field)**：
  * 若玩家在該節點修行失敗並產生 `Misconception`，暴走的氣血與業力會污染環境。
  * 節點 Evolve：靈草枯萎，環境產生 `miasma_risen` 訊號，原本的清淨地變為毒瘴瀰漫的險境，生成障礙。
