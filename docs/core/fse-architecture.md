# FSE 架構層級說明

Flow State Engine（FSE）採用三層架構，各層職責嚴格分離，Runtime 核心不含任何冒險特定邏輯，由 Adventure 注入層在啟動時完成配置。

---

## 層級一：Runtime 核心層（Adventure-Agnostic）

路徑：`runtime/`

所有冒險共用的通用引擎，不含任何特定冒險的路徑、術語或業務邏輯。

### 物件基底繼承鏈

```
object.c
└── entity.c
    ├── node.c          節點（關卡地標）基底
    └── actor.c         自主行為個體（NPC、Agent）基底
```

### 核心服務

| 服務 | 路徑 | 職責 |
|---|---|---|
| `event_bus` | `runtime/services/event_bus.c` | Pub/Sub 事件總線，解耦各服務間的連鎖通知 |
| `factor_service` | `runtime/services/factor_service.c` | 概念/因素解鎖管理，支援前置條件檢查 |
| `progress_manager` | `runtime/services/progress_manager.c` | 多軌道並行進度管理，stage 晉級判定 |
| `node_executor` | `runtime/services/node_executor.c` | 關卡挑戰判定引擎，支援多種 executor 擴充 |
| `discovery_service` | `runtime/services/discovery_service.c` | 領悟/發現薄層，委派至 factor_service |
| `i18n_service` | `runtime/services/i18n_service.c` | 國際化服務，支援多語系 YAML 合併載入 |
| `reveal_layer` | `std/reveal_layer.c` | 條件式內容揭示，由 factor 解鎖觸發 |

### Node Executor 擴充機制

`node_executor` 的 executor 類型為開放式，內建 `ast_matcher`，其餘類型由冒險自行提供：

```
runtime/executors/<executor_name>.c   ← 擴充 executor 放置位置
```

每個擴充 executor 需實作：

```c
int execute(object node_obj, object player, mapping ast, mapping chal_data, string cid)
```

### Progress Manager 關鍵設計

- `default_stages` mapping 由 Adventure 注入層設定，Runtime 不含預設值
- `query_spawn_node(player, track)` 讀取當前 stage YAML 的 `spawn_node` 欄位，回傳初始節點路徑
- stage 晉級條件支援 `quests`（完成任務清單）與 `factors`（解鎖概念清單）的組合判定

---

## 層級二：Adventure 注入層

路徑：`adventures/<name>/mudlib/master.c`

每個冒險的 `master.c` 在啟動時向 Runtime 核心注入所有冒險特定配置。Runtime 在注入完成前不具備任何可操作的業務邏輯。

### 注入項目

```c
// 1. 語系
i18n_svc->register_locale_path("/content/locales");
i18n_svc->set_language("zh_TW");

// 2. 概念/因素定義目錄（可注冊多個）
factor_svc->register_discovery_path("/content/factors");
factor_svc->register_discovery_path("/content/nodes/<node_id>/discoveries");

// 3. 進度階段目錄與預設起始階段
progress_svc->register_progression_path("/content/progression");
progress_svc->set_default_initial_stage("main", "stage_1_sequence");
```

### master.c 其他職責

| 函式 | 說明 |
|---|---|
| `connect(token)` | 建立玩家物件（`clone_object("/std/user.c")`） |
| `compile_object(file)` | 虛擬物件機制，將 `/nodes/<id>/node` 路徑映射至通用 `node.c` |
| `run_test_mode()` | 測試模式入口（`MUD_TEST_MODE` 環境變數驅動） |

### 各冒險注入對照

| 注入項目 | PW（編程世界） | Formosa（源流福爾摩沙） | 傳統 MUD |
|---|---|---|---|
| locale | `zh_TW` | `zh_TW` | 依需求 |
| progression | `stage_N_*.yaml` 鏈 | volume/chapter 結構 | 無或自訂 |
| default stage | `stage_1_sequence` | 第一卷起點 | 無 |
| executor 類型 | `ast_matcher` | `discovery_trigger` | `combat_resolver` 等 |
| 玩家物件 | `std/user.c`（PW） | `std/user.c`（Formosa） | `std/user.c`（MUD） |

---

## 層級三：Adventure 內容層

路徑：`adventures/<name>/mudlib/`

冒險特有的內容與玩家載體，與其他冒險完全隔離。

### 目錄結構（以 PW 為例）

```
adventures/pw/mudlib/
├── master.c                        Adventure 注入層入口
├── std/
│   ├── user.c                      玩家載體（progression、factors、physical_state）
│   ├── node.c                      節點薄層（繼承 /runtime/core/node.c）
│   ├── entity.c                    實體薄層
│   └── reveal_layer.c              揭示層薄層
├── content/
│   ├── nodes/
│   │   └── <node_id>/
│   │       ├── node.yaml           節點配置（exits、reveal_layers、challenges 清單）
│   │       ├── challenges/
│   │       │   └── <id>.yaml       挑戰定義（executor、expected_ast、consequence）
│   │       └── discoveries/
│   │           └── <id>.yaml       發現/領悟定義
│   ├── factors/
│   │   └── <id>.yaml               全域因素定義（name、prerequisites、progress）
│   ├── progression/
│   │   └── stage_N_<name>.yaml     進度階段定義（spawn_node、requires、next）
│   └── locales/
│       └── zh_TW.yaml              語系翻譯鍵值
├── cmds/
│   ├── player/
│   │   └── execute.c               玩家指令（PW：積木 AST 接收與分派）
│   └── admin/
│       └── cmd_tests.c             測試指令
├── services/
│   └── blockly_service.c           PW 特有服務（Blockly 積木前端橋接）
├── include/
│   ├── config.h
│   ├── formosa.h
│   ├── race.h
│   └── ansi.h
└── web/
    └── static/
        ├── index.html              Web 前端（Blockly UI）
        └── js/
            └── pw-blockly-adapter.js
```

### stage YAML 結構

```yaml
version: "1.0"
stage_id: "stage_1_sequence"
name: "序列篇"
description: "學習程序的線性執行，掌握循序邏輯的基礎。"
spawn_node: "/nodes/infinite_loop_swamp/node"   # ← progress_manager.query_spawn_node() 讀取
requires:
  quests:
    - "infinite_loop_termination"
  factors:                                       # 可選，解鎖特定概念亦可觸發晉級
    - "some_factor_id"
next: "stage_2_loop"
```

### node.yaml 結構

```yaml
version: "1.0"
node_id: "infinite_loop_swamp"
name: "無窮迴圈沼澤"
type: "lesson"
mentor: "sys_monitor"
description: "..."
exits:
  go_valley: "counter_valley"
reveal_layers:
  - reveal_block: "controls_break"
    checks:
      - type: "factor_discovered"
        factor_id: "loop_termination"
challenges:
  - id: "infinite_loop_termination"
    executor: "ast_matcher"
```

### challenge YAML 結構（ast_matcher）

```yaml
challenge_id: "infinite_loop_termination"
executor: "ast_matcher"
required_factor: "loop_termination"       # 可選：需先解鎖此概念才能使用特定積木
expected_ast:
  matcher: "rule_based"                   # basic | rule_based | custom_matcher
  rules:
    - path: "$.type"
      operator: "eq"
      value: "Loop"
    - path: "$.has_break"
      operator: "eq"
      value: 1
success_msg: "🎉 迴圈成功終止！"
success_progress: 10
failure_consequence:
  memory_cost: 5
  failure_flag: "loop_not_terminated"
  discover_factor: "loop_termination"     # 失敗時反而觸發概念解鎖
  error_warning: "迴圈沒有終止條件，系統記憶體耗盡！"
```

---

## 層級四：Go 驅動層

路徑：`adventures/cmd/fse/main.go`

負責啟動 LPC VM、HTTP/WebSocket 伺服器與 P2P 信令，所有冒險共用同一個 Go 驅動，以啟動參數區分：

```bash
# 啟動 PW 冒險
./fse --mudlib adventures/pw/mudlib --master master.c --port 8080

# 啟動 Formosa 冒險（預期）
./fse --mudlib adventures/formosa/mudlib --master master.c --port 8081
```

### Go 驅動主要元件

| 元件 | 說明 |
|---|---|
| `lpc-vm/driver` | LPC 執行引擎，載入 mudlib 與 master.c |
| `internal/signaling` | WebSocket Hub，管理玩家連線 |
| `internal/p2p` | P2P 節點，支援多伺服器互連 |
| `EmbeddedFS` | mudlib 可嵌入二進位，支援單檔部署 |

### DriverConfig 關鍵欄位

```go
config := driver.DriverConfig{
    MudLibPath:      "adventures/pw/mudlib",
    MasterFile:      "master.c",
    HeartBeatTick:   0,               // 各物件自行開啟
    CleanUpInterval: 5 * time.Minute,
    EmbeddedFS:      mudscript.Assets, // 嵌入式靜態資源
}
```

---

## 架構原則摘要

| 原則 | 實踐方式 |
|---|---|
| Adventure-Agnostic Runtime | Runtime 服務不含任何路徑字串，全由 master.c 注入 |
| 資料驅動內容 | 節點、挑戰、進度階段皆為 YAML，不需修改 LPC 程式碼即可新增內容 |
| 虛擬物件機制 | `compile_object()` 讓 `/nodes/<id>/node` 路徑無需實體 `.c` 檔案 |
| Executor 擴充點 | `runtime/executors/<name>.c` 支援各冒險自訂判定邏輯 |
| 事件驅動進度 | Factor 解鎖 → EventBus → progress_manager 連鎖更新，無直接耦合 |
| 多軌道進度 | 同一玩家可同時在不同 track 推進，支援主線與支線並行 |
