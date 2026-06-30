# 13. Content Pipeline - 地景與記憶碎片擴展指南 (Content Pipeline)

本指南旨在為後續開發者（不論是人類程式設計師還是 AI 代理人）提供標準的擴展工作流，以確保資料格式的一致性與測試的自動化。

---

## 🛠️ 地景與地標擴展流程 (New Site Pipeline)
1.  **聚落聲明**：在 `/data/yaml/settlements/{settlement_id}.yaml` 中的 `sites` 列表內加入新 Site ID。
2.  **地標配置**：在 `/data/yaml/sites/{settlement_id}/{site_id}.yaml` 中建立地標檔案，定義其 `connections`（連通地標）及 `reveal_layers`（條件浮現條件與 NPC）。
3.  **地理交通連通**：在 `/data/yaml/routes/settlements/` 下加入或更新對應的 `route_{id}.yaml` 檔，並配置 `eras`（可通行時代限制）與 `travel_time`（移動耗時）。

---

## 🧩 歷史記憶碎片擴展流程 (New Memory Pipeline)
1.  **定義 YAML**：在 `/mudlib/data/yaml/memories/` 下建立 `{settlement}_{name}_{number}.yaml` 設定檔。
2.  **配置觸發器與條件**：
    *   設定 `trigger_site`（指向目標 Site ID）。
    *   在 `conditions` 中設定觸發前提條件（如擁有某踏印或職涯等）。
    *   填寫該時代的歷史背景描述文字。
3.  **編寫自動化測試**：在 `mudlib/tests/` 下建立對應的 `test_{id}.c`，模擬玩家前往該 Site 並驗證解鎖記憶碎片，最後透過 `make test-fsmud` 運行驗證。
