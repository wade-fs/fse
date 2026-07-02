# Flow State Engine (FSE) 2.0 全局系統架構說明書

Flow State Engine (FSE) 2.0 採用「引擎-故事」嚴格分離的三層宣告式架構。引擎本身（Runtime）不綁定任何特定冒險的世界觀、路徑與術語，由冒險模組（Adventure）在啟動時動態注入配置與服務。

---

## 一、 核心原則：引擎與故事的清澈邊界

FSE 2.0 平台開發必須嚴格遵守以下邊界守則：
* **禁止污染 runtime/**：`runtime/` 目錄下的任何代碼（包括各類 Services 服務與 Executors 執行器）**絕對不得**出現任何特定冒險（如 `so` 或 `pc` 等）的具體房間、節點、NPC 或物品路徑。
* **冒險專屬服務**：特定冒險專屬的業務邏輯服務（如 `karma_service` 業力進程），必須放置在 `adventures/<adventure_id>/mudlib/services/` 下，透過 `manifest.yaml` 動態加載。
* **特殊節點尋路**：天劫雷雲、心魔幻境等特殊地標，必須在 `manifest.yaml` 的 `special_nodes` 中以宣告式進行路徑配置，執行期由專屬服務動態讀取，禁止硬編碼。

---

## 二、 冒險解剖學 (Adventure Anatomy)

一個完整的 FSE 冒險模組由以下七個層次組成，層次分明，各司其職：

```
Adventure
├── 1. Identity (身份宣告)      : 冒險名稱、ID、版本及 manifest 宣告。
├── 2. World (世界設定)         : 故事背景、地理分佈與世界物理法則描述。
├── 3. Reality Package (認識論)  : 由多個 Reality 分支組成的客觀規律 Law YAML 庫。
├── 4. Content (世界內容)       : 地域（Region）、節點（Site/Node）、存在體（NPC）與物件。
├── 5. Knowledge Tree (知識樹)  : 獨立的抽象規則與知識概念，供 Challenge 引用。
├── 6. Challenge (挑戰)         : 一次認識論的三態評估事件。
└── 7. Progression (進度線)     : 玩家的成長軌道與階段晉級條件。
```

每個層次的具體職責對比如下：

| 層次 | 是什麼 | 不是什麼 |
|------|--------|---------|
| **Identity** | Adventure 的名稱、語言、manifest 宣告 | 具體遊戲邏輯 |
| **World** | 世界觀、時代、物理規則描述文件 | LPC 程式碼 |
| **Reality Package** | 可被 Resolver 引用的 Law YAML | 故事劇情文字 |
| **Region** | 地域的地理與文化分組 | 節點本身 |
| **Site/Node** | 玩家可進入的一個地點及其互動 | 全域通用邏輯 |
| **NPC/Presence** | 有名字、行為、對話的存在體 | 玩家角色本身 |
| **Object/Item** | 可攜帶、使用、交換的實體物件 | 地標節點 |
| **Knowledge Tree** | 可被 Challenge 引用的抽象知識概念定義 | 具體 NPC 的台詞 |
| **Challenge** | 一次認識論（Reality Check）三態評估事件 | 寫死的一對一問答 |
| **Progression** | 玩家的成長路徑與階梯晉級定義 | 單一挑戰節點 |

---

## 三、 標準目錄結構 (Adventure File Hierarchy)

雖然各冒險在宣告上保有高度自由度，但為了與平台工具鏈對接，每個冒險模組必須遵循以下標準目錄結構：

```
adventures/<adventure_id>/
├── manifest.yaml          # Identity 宣告與全路徑配置單一真理來源
├── docs/                  # 設計文件與故事世界觀文檔
├── novice_map.yaml        # 宣告式地圖結構定義
├── scaffold_node.py       # 宣告式節點與挑戰編譯工具
├── generate_map_viz.py    # 節點聯通 Mermaid & SVG 可視化工具
│
└── mudlib/
    ├── master.c           # 冒險啟動入口 (Master Object)
    ├── secure/
    │   └── valid.c        # 權限驗證
    ├── std/               # 冒險專屬繼承類別
    │   ├── node.c         # 繼承自 /runtime/core/node.c
    │   ├── user.c         # 繼承自 /runtime/core/actor.c (玩家載體)
    │   ├── presence.c     # 繼承自 /runtime/core/living.c (NPC/存在體)
    │   └── fse_room.c     # 繼承自標準空間，整合 ActionExecutor
    ├── services/          # 冒險專屬服務 (引導時由 master.c 自動加載)
    │   └── karma_service.c
    └── content/
        ├── nodes/         # 所有場景節點定義
        │   └── <node_id>/
        │       ├── node.yaml
        │       ├── challenges/   # 認識論評估挑戰 YAML
        │       └── discoveries/  # 感官發現物定義
        ├── npcs/          # 存在體 NPC 定義
        ├── objects/       # 實體物品定義
        ├── factors/       # 永久記憶概念與 Factor 定義
        ├── progression/   # 進度階段 YAML
        └── locales/       # 多國語言 i18n 對譯表
```

---

## 四、 平台自動化工具與引導鏈

為加速世界構築，FSE 提供了強大的引導與編譯工具鏈：

### 1. 一鍵 Bootstrap 冒險模組：`create-adventure.py`
在全域根目錄下，創作者可以使用該平台工具快速生成全新且符合標準目錄架構的冒險模組：
```bash
python3 create-adventure.py --name <adventure_id> --title "<冒險中文名>"
```
* **職責**：自動創建完整的目錄樹（包含 `mudlib/web/static`），自動映射 `virtual_rules` 與 `master.c`，生成預設 `Makefile`、新手 `novice_map.yaml` 與單元測試，達成一鍵零設定綠燈啟動。

### 2. 宣告式節點與挑戰編譯：`scaffold_node.py`
在冒險目錄下運行，讀取 `novice_map.yaml` 進行批量或單一地標編譯，並自動注入 sensory_signals 感官信號、Presence 與 paths。

### 3. 可視化地圖同步更新：`generate_map_viz.py`
讀取地圖 YAML，自動生成與地圖同名的 Markdown 流程圖檔 (Mermaid) 與 SVG 向量結構地圖檔。

---

## 五、 宣告式註冊規範 (`manifest.yaml`)

`manifest.yaml` 是冒險專案的**單一真理來源**。一個標準的配置範例如下：

```yaml
adventure_id: "so"
name: "尋仙問道 - Shushan Odyssey"
version: "1.0"

# 注入核心服務的內容路徑 (對齊標準目錄結構)
content_paths:
  nodes: "/content/nodes"
  realities: "/realities"
  progression: "/content/progression"
  locales: "/content/locales"
  factors: 
    - "/content/factors"
    - "/content/nodes/*/discoveries"

initial_stage: "stage_1_mortal"
default_language: "zh_TW"

# 虛擬路徑物件編譯映射規則 (相對於 mudlib root)
virtual_rules:
  nodes: "/std/node.c"
  rooms: "/std/fse_room.c"

# 專屬服務自動加載列表
extra_services:
  - "/services/karma_service.c"

# 特殊功能性地標路徑宣告
special_nodes:
  tribulation: "/nodes/tribulation_cloud/node"
  heart_demon: "/nodes/heart_demon_rift/node"
```

---

## 六、 運作原理與引用規範

### 1. 互動分發與解鎖
* **統一入口閘門**：節點內的所有玩家輸入均由 `fse_room.c` 的 `resolve_interaction()` 統一攔截。
* **串聯路由機制**：
  ```
  玩家輸入 ➔ fse_room.c resolve_interaction()
               ├── 無 resolver 宣告 ➔ 直接判定（適用一級簡單交互，成敗寫死在節點）
               └── 有 resolver 宣告 ➔ 派發至 ActionExecutor ➔ 載入專屬 Resolver 進行評估
  ```
* 任何涉及認識論（Reality Check）或複雜行為（對話、交易、戰鬥）的挑戰，均會自動轉交給對應的解算器（如 `reality_resolver.c`），並載入該節點 `challenges/` 底下的 YAML 設計。


### 2. 證據鏈評估 (Observations)
* 挑戰可宣告多個並行 Reality 分支（如 `natural` 與 `spiritual`）。
* 在判定特定 Knowledge 時，可設置雙重或多重 Observation 證據鏈依賴：
  ```yaml
  evaluate:
    required_observations:
      - "howling_wind"
      - "physical_tension"
    aligned_action: "observe_breath"
  ```
  玩家必須同時集齊以上兩項 Evidence 且發起正確的 Aligned Predict，才能獲得領悟（UNDERSTANDING）解鎖對應的 Factor。

### 3. 可識別度 (NPC Presence Recognition)
* NPC 必須繼承 `presence.c`。
* 透過 `set_recognition_rules(true_name, disguised_name, factors_array)` 宣告辨識門檻。
* 系統會自動比對觀看者 (`this_player()`) 是否具備特定的 Factors，否則只會看到被偽裝後的名稱（如 `一團模糊的黑影`）。
