# FSE 2.0 引擎規範與運行期生命週期 (Runtime Specification)

本文件定義了 Flow State Engine (FSE) 2.0 核心 Runtime 的功能邊界、物件虛擬化技術，以及整個冒險在運行期活起來的生命週期。

---

## 一、 核心引擎運行生命週期 (Adventure Lifecycle)

當一個 FSE 遊戲進程啟動時，它會按照以下精密的順序將世界與邏輯激活：

```
     1. [Boot] MUD 驅動引擎啟動，載入 SimulEfun.c
        │
        ▼
     2. [Load Manifest] 讀取 adventures/<id>/mudlib/manifest.yaml 取得配置
        │
        ▼
     3. [Register Reality] 註冊 Content realities 目錄，對齊客觀物理/精神律則
        │   優先度與覆蓋規則：
        │   1. 全域 Law 庫：/runtime/realities/ (優先載入)
        │   2. 冒險專屬 Law：adventures/<id>/realities/ (後載入，同名 id 將覆蓋全域)
        │   * 建議專屬 Law 檔案採用前綴命名空間（如 `cultivation.realm.*`）以避免全域衝突。
        ▼
     4. [Load Knowledge] 載入全域共用或冒險專屬的 Knowledge Tree 知識樹

        │
        ▼
     5. [Load Content] 註冊 /nodes/ 及 /content/ 目錄 (語系對譯與發現物)
        │
        ▼
     6. [Register Services] 動態加載 manifest 宣告之專屬服務 (如 karma_service)
        │
        ▼
     7. [Ready] Master 準備就緒，引擎進入事件心跳循環 (Heart Beat Loop)
        │
        ▼
     8. [Player Login] 玩家登入，建立 actor.c 實體並載入存檔 (restore_state)
        │
        ▼
     9. [Action] 玩家與環境互動，發起 Predict 行動 (例如 trade_fair)
        │
        ▼
    10. [Reality Evaluation] ActionExecutor 派發並調用 reality_resolver
        │   依照 required_observations 與偏誤模式，裁決三態 (U/M/M) 結果，套用副作用
        ▼
    11. [Progression] 進度管理器檢測階段升級條件，解鎖對應的 Factor 記憶
        │
        ▼
    12. [Persistence] 寫入玩家最新狀態 (save_state)
```

---

## 二、 虛擬物件映射規範 (Virtual Object Spec)

FSE 使用 LPC 虛擬物件編譯器攔截地圖路徑加載：
* **攔截點**：`master.c` 的 `compile_object(string file)`。
* **映射**：讀取 `manifest.yaml` 中的 `virtual_rules`。
* **對稱路徑注入**：
  * 當虛擬路徑 `/nodes/town_center/node` 被載入時，`virtual.c` 實體化 `/std/node.c` (或該冒險設定的 class)。
  * **主動注入**：`virtual.c` 調用 `ob->setup_virtual_by_path(file_path, prefix, config_file)`。
  * **動態覆寫**： cloner 物件透過覆寫 `query_entity_id()` 與 `query_short()`，優先向 `virtual_config` 查找名稱，規避 LPC 菱形繼承的變數隔離。

---

## 三、 服務責任與邊界

Runtime 核心服務的職責如下，**嚴禁硬寫入任何冒險的路徑與故事特定規則**：

| 服務名稱 | 責任邊界 | 跨冒險共用方法 |
|---|---|---|
| **`event_bus.c`** | 全域 Pub/Sub 事件總線。不限制事件名稱，只負責信號傳遞。 | `subscribe(event, func)`, `publish(event, data)` |
| **`factor_service.c`** | 管理 Factors 解鎖與衰退。不定義什麼是境界或學識，只從冒險 content 中載入 Factor 元數據。 | `discover_factor(player, fid)`, `has_factor(player, fid)` |
| **`progress_manager.c`** | 提供多線並行進度軌道。不預設起始階段，全部從 manifest 讀取 initial_stage。 | `complete_quest(player, qid, track)`, `query_current_stage()` |
| **`action_executor.c`** | 通用行動派發器。依據 YAML 的 `resolver` 鍵動態尋找並載入專屬 Resolver。 | `dispatch_action(node, player, action, target, act_cfg)` |
