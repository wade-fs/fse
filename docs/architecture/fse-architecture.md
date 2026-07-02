# Flow State Engine (FSE) 2.0 全局系統架構說明書

Flow State Engine (FSE) 2.0 採用「引擎-故事」嚴格分離的三層宣告式架構。引擎本身（Runtime）不綁定任何特定冒險的世界觀、路徑與術語，由冒險模組（Adventure）在啟動時動態注入配置與服務。

---

## 🗺️ FSE 2.0 三層系統全景

```
 ┌────────────────────────────────────────────────────────┐
 │ 1. Runtime 核心層 (Core Engine)                        │
 │    ├── 虛擬物件編譯與注入 (virtual.c)                 │
 │    ├── 核心服務 (event_bus, factor_service, etc.)      │
 │    └── 並行認識論評估器 (reality_resolver.c)            │
 └──────────────────────────┬─────────────────────────────┘
                            ▼
 ┌────────────────────────────────────────────────────────┐
 │ 2. Adventure 宣告注入層 (master.c + manifest.yaml)      │
 │    ├── 單一真理來源配置映射                            │
 │    └── 動態加載專屬服務 (extra_services) & 特殊地標     │
 └──────────────────────────┬─────────────────────────────┘
                            ▼
 ┌────────────────────────────────────────────────────────┐
 │ 3. Adventure 內容與互動層 (Content Layer)              │
 │    ├── 宣告式地圖與節點 (node.yaml)                    │
 │    └── 多感官證據鏈挑戰 (Challenge YAML)               │
 └────────────────────────────────────────────────────────┘
```

---

## 一、 第一層：Runtime 核心層 (Adventure-Agnostic)

**路徑**：`/runtime/`  
引擎公共的核心，**絕對禁止**編入任何特定冒險的具體路徑（如 `so` 或 `pc` 等）。

### 1. 物件基底繼承鏈 (Inheritance Hierarchy)
```
object.c (底層虛擬機基底)
└── entity.c (實體基礎：提供 ID 與型態)
    ├── virtual_object.c (資料驅動：YAML 讀取與路徑對齊)
    │   └── node.c (節點地標基底，資料驅動加載)
    └── actor.c (自主行為個體基底)
```

### 2. 核心公共服務 (Core Services)
* **`event_bus.c`**：Pub/Sub 全域事件總線，用於解耦各服務間的連鎖觸發（如了結誓願時發布事件）。
* **`factor_service.c`**：概念/變革因子解鎖管理器，負責持久化玩家的永久記憶/里程碑。
* **`progress_manager.c`**：多軌道並行進度管理，負責依據 Quest 和 Factor 判定 Stage 晉級。
* **`action_executor.c`**：通用行動派發服務。當玩家輸入時，將行動發送給專屬執行器（如 `reality_resolver.c` 或戰鬥、百工執行器），避免互動層寫死成敗。

### 3. 雙重虛擬物件尋路防禦 (`virtual_object.c`)
為了防止多重/菱形繼承造成的變數隔離，虛擬物件加載遵循以下機制：
* 虛擬編譯管理器 `/runtime/core/virtual.c` 在 `clone_object` 成功後，主動呼叫 `ob->setup_virtual_by_path(file_path, prefix, config_file)` 注入實際加載路徑。
* 節點重寫 `query_entity_id()` 與 `query_short()`，優先從 `virtual_config` 中動態讀取，實現 100% 的資料驅動。

---

## 二、 第二層：Adventure 宣告注入層 (Single Source of Truth)

**路徑**：`adventures/<id>/mudlib/`  
每個冒險透過唯一的 `manifest.yaml` 提供模組的所有配置，並由 `master.c` 作為純粹的「引導加載器（Loader）」。

### 1. 宣告式配置對齊 (`manifest.yaml`)
```yaml
adventure_id: "so"
name: "尋仙問道"
version: "1.0"

# 資源與語系目錄註冊
content_paths:
  nodes: "/content/nodes"
  realities: "/realities"
  progression: "/content/progression"
  locales: "/content/locales"
  factors: 
    - "/content/factors"

# 虛擬路徑對齊規則
virtual_rules:
  nodes: "/std/node.c"
  rooms: "/std/fse_room.c"

# 動態加載冒險專屬服務
extra_services:
  - "/services/karma_service.c"

# 專屬特殊功能地標 (例如天劫、心魔)
special_nodes:
  tribulation: "/nodes/tribulation_cloud/node"
  heart_demon: "/nodes/heart_demon_rift/node"
```

### 2. 專屬服務動態加載
* 冒險專屬的業務 daemon（如全域業力調度 `karma_service.c`）**必須**放置在冒險的 `services/` 目錄下。
* `master.c` 讀取 `manifest.yaml` 後，動態執行 `load_object()` 自動引導，完全將專屬業務邏輯從 `runtime/` 剝離。

---

## 三、 第三層：Adventure 內容與前端通訊層

**路徑**：`adventures/<id>/mudlib/content/`  
由地圖、節點、NPC、挑戰與語系對譯組成。

### 1. 並行 Reality 三態評估 (Parallel Reality Check)
一個 Challenge 可同時包含多個 Reality 面向的評估分支（如 `social` 與 `spiritual`），每個分支單獨評估，並打包各自的 Evolve 副作用。
* **UNDERSTANDING (領悟)**：玩家 Observations 與 Law 完全對齊，解鎖 Factor。
* **MISUNDERSTANDING (偏誤)**：滿足特定的誤解模式，玩家獲得局部偏誤狀態。
* **MISCONCEPTION (成見)**：行動背離，引發負面效果或天劫心魔懲罰。

### 2. WebSocket 實時通訊協議
透過 Go 驅動層與 LPC 層的 `__JSON_MSG__` 前綴感應，FSE 2.0 在 LPC 層直接向網頁前端推送實時數據：
* **`node_state`**：玩家切換節點或進入房間時推播，供前端更新場景、地圖路徑及 sensory 感官按鈕。
* **`status_update`**：Observations（感知）、`spiritual_energy`（靈力）、`karma`（業力）或打坐狀態變更時實時推播。
* **`reality_evaluation`**：認識論評估結束後發送，推送各分支判定結果，用以驅動前端 Canvas 雷達圖的動態波紋與收縮。
