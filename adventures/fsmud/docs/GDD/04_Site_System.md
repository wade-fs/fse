# 04. Site System - YAML 地標與浮現層系統 (Site System)

地標 (Site) 的所有內容（除了動態進出的玩家）皆是由 YAML 檔案完全資料驅動的。

---

## 🎨 條件浮現層 (Reveal Layers)
LPC 地標工廠 `/std/site.c` 會自動解析 YAML 中的 `reveal_layers` 配置。這些 Reveal Layers 根據玩家的狀態進行條件過濾：
*   **過濾條件類型 (Condition Checks)**：
    *   `has_footprint`：是否擁有特定踏印（如劉家地契任務需要鬼屋踏印）。
    *   `career_rank`：職涯種類與等級限制（如匠人等級 1 在老街顯現工坊）。
    *   `era_completed`：世界歷史時代的通關狀態。
    *   `has_talent`：玩家天賦限制（如主角專屬的 `island_memory` 島嶼記憶）。
*   **動態渲染產出**：
    *   **文字 (Text)**：向符合條件的玩家追加顯示歷史環境描述。
    *   **NPC**：動態將對應的 NPC 投影至該地標供互動。
    *   **入口 (Entry)**：浮現可前往特定歷史層（History Layer）的傳送入口。
