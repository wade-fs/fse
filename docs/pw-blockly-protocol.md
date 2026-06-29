# PW Adventure — Blockly 通訊協定規格書

**版本**：1.0  
**適用範圍**：`adventures/pw`  
**最後更新**：2026-06  

---

## 目錄

1. [架構總覽](#1-架構總覽)
2. [設計原則](#2-設計原則)
3. [Message 格式基礎](#3-message-格式基礎)
4. [後端 → 前端 訊息](#4-後端--前端-訊息)
   - [TOOLBOX_UPDATE](#41-toolbox_update)
   - [EXECUTION_RESULT](#42-execution_result)
   - [WORLD_STATE](#43-world_state)
   - [I18N_UPDATE](#44-i18n_update-保留)
5. [前端 → 後端 訊息](#5-前端--後端-訊息)
   - [EXECUTE](#51-execute)
   - [REQUEST_TOOLBOX](#52-request_toolbox)
6. [連線生命週期](#6-連線生命週期)
7. [內容範例](#7-內容範例)
   - [無窮迴圈沼澤 完整對話](#71-無窮迴圈沼澤-完整對話)
   - [節點 YAML 與積木對應](#72-節點-yaml-與積木對應)
8. [實作檔案索引](#8-實作檔案索引)
9. [未來擴展規劃](#9-未來擴展規劃)

---

## 1. 架構總覽

PW Adventure 採用「**後端主導的動態 Blockly 容器**」架構：

```
┌─────────────────────────────────────────────────────────┐
│                     前端 (Browser)                      │
│                                                         │
│   ┌─────────────────┐      ┌───────────────────────┐   │
│   │  Blockly 工作區  │      │   PWBlocklyAdapter    │   │
│   │  (dumb container)│◄────►│  (ws 通訊 + 分派)     │   │
│   └─────────────────┘      └───────────┬───────────┘   │
│                                         │ WebSocket      │
└─────────────────────────────────────────┼───────────────┘
                                          │
┌─────────────────────────────────────────┼───────────────┐
│              後端 (FSE / LPC MUD)        │               │
│                                         │               │
│  ┌──────────────┐   ┌───────────────────▼────────────┐  │
│  │ blockly_     │◄──│  execute.c (cmds/player)       │  │
│  │ service.c    │   │  處理 EXECUTE / REQUEST_TOOLBOX │  │
│  │ (adventures/ │   └────────────────────────────────┘  │
│  │  pw/mudlib/  │                    │                  │
│  │  services/)  │          ┌─────────▼──────────┐       │
│  └──────┬───────┘          │  node_executor.c   │       │
│         │                  │  (runtime/services) │       │
│  ┌──────▼───────┐          └─────────┬──────────┘       │
│  │ reveal_layer │                    │                   │
│  │ factor_svc   │          ┌─────────▼──────────┐       │
│  │ progress_mgr │          │  node.yaml / YAML  │       │
│  └──────────────┘          │  challenge 設定     │       │
│                            └────────────────────┘       │
└─────────────────────────────────────────────────────────┘
```

**關鍵設計**：前端 Blockly 只是一個「可程式化容器」——它不決定顯示哪些積木，這完全由後端根據玩家的 `factor`、`stage`、`reveal_layer` 狀態決定。

---

## 2. 設計原則

| 原則 | 說明 |
|------|------|
| **後端主導** | 積木工具箱（Toolbox）由後端決定，前端被動接受更新 |
| **標準 JSON 協定** | 所有訊息使用 `{ "type": "...", "payload": {...} }` 格式 |
| **雙向通訊** | 後端可主動 Push，前端也可發送 Request |
| **冒險隔離** | `blockly_service.c` 放在 `adventures/pw/` 下，不影響 runtime 核心 |
| **向下相容** | `PWBlocklyAdapter` 保留對舊版 `mud_text` 格式的相容處理 |
| **資料驅動** | 積木解鎖邏輯在 `node.yaml` 的 `reveal_layers` 中宣告，不寫死在程式碼裡 |

---

## 3. Message 格式基礎

所有訊息均為 JSON 物件，包含以下頂層欄位：

```json
{
  "type": "MESSAGE_TYPE",
  "payload": { ... },
  "request_id": "req-001"
}
```

| 欄位 | 必填 | 說明 |
|------|------|------|
| `type` | ✅ | 訊息類型識別字（見下方各節） |
| `payload` | 依類型 | 訊息主體，各類型格式不同 |
| `request_id` | ❌ | 前端發送的 Request 中攜帶，用於 request-response 配對（後端目前不強制要求） |

---

## 4. 後端 → 前端 訊息

後端在以下時機主動推送訊息給玩家：
- 玩家連線後（回應 `REQUEST_TOOLBOX`）
- 積木執行成功或失敗後
- 因素解鎖、進度更新時

### 4.1 `TOOLBOX_UPDATE`

**時機**：玩家連線初始化、執行積木後（如有新解鎖）  
**效果**：前端立即以新 toolbox 呼叫 `workspace.updateToolbox()`

```json
{
  "type": "TOOLBOX_UPDATE",
  "payload": {
    "toolbox": {
      "categories": [
        {
          "name": "控制流",
          "colour": "#5b80a5",
          "blocks": [
            { "type": "controls_loop" },
            { "type": "controls_break" }
          ]
        },
        {
          "name": "變數與運算",
          "colour": "#a55b80",
          "blocks": [
            { "type": "variables_count" },
            { "type": "compare_less_100" },
            { "type": "action_increment" },
            { "type": "action_assignment" }
          ]
        }
      ],
      "unlocked_factors": ["loop_termination"]
    }
  }
}
```

> **Note**  
> `unlocked_factors` 僅用於前端顯示提示，不用於控制積木可見性（那是後端的職責）。

### 4.2 `EXECUTION_RESULT`

**時機**：後端完成 AST 匹配判定後  
**效果**：前端顯示結果文字、觸發動畫、更新記憶體條

```json
{
  "type": "EXECUTION_RESULT",
  "payload": {
    "success": 1,
    "message": "🎉 很棒！你成功用 [中斷] 積木修復了無窮迴圈！",
    "memory_cost": 0,
    "new_factors": ["loop_termination"],
    "toolbox_delta": {
      "toolbox": {
        "categories": [ "..." ]
      }
    }
  }
}
```

| 欄位 | 類型 | 說明 |
|------|------|------|
| `success` | `int (0/1)` | 執行是否成功 |
| `message` | `string` | 顯示給玩家的文字訊息 |
| `memory_cost` | `int` | 此次執行消耗的記憶體點數（0 = 無消耗） |
| `new_factors` | `string[]` | 本次新解鎖的因素 ID 列表（可為空） |
| `toolbox_delta` | `object?` | 若有立即更新，附上新的完整 toolbox |

> **Important**  
> 目前 `toolbox_delta` 包含完整 toolbox 而非差量，前端直接套用即可。

### 4.3 `WORLD_STATE`

**時機**：每次執行後同步推送  
**效果**：前端更新記憶體條、場景資訊等 UI 元素

```json
{
  "type": "WORLD_STATE",
  "payload": {
    "current_node": "infinite_loop_swamp",
    "memory": 80,
    "unlocked_factors": ["loop_termination"]
  }
}
```

| 欄位 | 類型 | 說明 |
|------|------|------|
| `current_node` | `string` | 玩家目前所在節點 ID |
| `memory` | `int (0~100)` | 系統記憶體剩餘百分比 |
| `unlocked_factors` | `string[]` | 玩家目前已解鎖的所有因素 |

### 4.4 `I18N_UPDATE`（保留）

**時機**：語言切換或文字熱更新  
**狀態**：協定已定義，前端 adapter 已預留 handler，後端尚未實作推送邏輯

```json
{
  "type": "I18N_UPDATE",
  "payload": {
    "key": "core.executor.analyzing",
    "text": "💡 虛擬機正在分析積木結構..."
  }
}
```

---

## 5. 前端 → 後端 訊息

前端訊息由 `PWBlocklyAdapter` 封裝後，透過 WebSocket 橋接層轉換為 LPC 指令，路由至 `execute.c`。

### 5.1 `EXECUTE`

**時機**：玩家按下「執行程式」按鈕  
**動作**：前端將 Blockly 工作區的積木轉換為 AST，送往後端執行判定

```json
{
  "type": "EXECUTE",
  "payload": {
    "ast": {
      "type": "Loop",
      "condition": "count < 100",
      "act": "count++",
      "has_break": 1,
      "body": [
        { "type": "Break" }
      ]
    }
  },
  "request_id": "req-001"
}
```

**AST 節點類型表**：

| `type` | 說明 | 關鍵欄位 |
|--------|------|----------|
| `Loop` | 迴圈積木 | `condition`, `act`, `body[]`, `has_break` |
| `Break` | 中斷積木 | — |
| `Assignment` | 賦值積木 | `var_name`, `value` |

### 5.2 `REQUEST_TOOLBOX`

**時機**：WebSocket 連線建立成功後（由 `PWBlocklyAdapter` 自動發送）  
**動作**：請求後端依玩家當前狀態回傳可用工具箱

```json
{
  "type": "REQUEST_TOOLBOX",
  "request_id": "req-002"
}
```

後端回應：`TOOLBOX_UPDATE` + `WORLD_STATE`（同時推送）

---

## 6. 連線生命週期

```
前端                                          後端
  │                                            │
  │──── WebSocket 連線建立 ───────────────────►│
  │◄─── (onopen 觸發) ──────────────────────── │
  │                                            │
  │──── REQUEST_TOOLBOX ──────────────────────►│ resolve_toolbox(player)
  │◄─── TOOLBOX_UPDATE ─────────────────────── │ → blockly_service
  │◄─── WORLD_STATE ────────────────────────── │
  │                                            │
  │  (玩家拖曳積木，按執行)                      │
  │                                            │
  │──── EXECUTE { ast: {...} } ───────────────►│ receive_execution()
  │                                            │ → node_executor.c
  │                                            │ → AST 匹配判定
  │◄─── EXECUTION_RESULT (success/fail) ─────── │
  │◄─── TOOLBOX_UPDATE (如有新解鎖) ─────────── │
  │◄─── WORLD_STATE (memory 更新) ───────────── │
```

---

## 7. 內容範例

### 7.1 無窮迴圈沼澤 完整對話

這是一個從連線到解題成功的完整訊息序列範例。

#### ① 連線初始化（玩家尚未解鎖 `loop_termination` 因素）

前端發送：
```json
{ "type": "REQUEST_TOOLBOX", "request_id": "req-001" }
```

後端回應（TOOLBOX_UPDATE）：
```json
{
  "type": "TOOLBOX_UPDATE",
  "payload": {
    "toolbox": {
      "categories": [
        {
          "name": "控制流",
          "colour": "#5b80a5",
          "blocks": [
            { "type": "controls_loop" }
          ]
        },
        {
          "name": "變數與運算",
          "colour": "#a55b80",
          "blocks": [
            { "type": "variables_count" },
            { "type": "compare_less_100" },
            { "type": "action_increment" },
            { "type": "action_assignment" }
          ]
        }
      ],
      "unlocked_factors": []
    }
  }
}
```

後端回應（WORLD_STATE）：
```json
{
  "type": "WORLD_STATE",
  "payload": {
    "current_node": "infinite_loop_swamp",
    "memory": 100,
    "unlocked_factors": []
  }
}
```

> `controls_break` 尚未出現，因為玩家還沒解鎖 `loop_termination` 因素。

---

#### ② 玩家第一次執行（只用了 Loop，沒有 Break）→ 失敗，觸發因素發現

前端發送：
```json
{
  "type": "EXECUTE",
  "payload": {
    "ast": {
      "type": "Loop",
      "condition": null,
      "act": null,
      "has_break": 0,
      "body": []
    }
  },
  "request_id": "req-002"
}
```

後端回應（TOOLBOX_UPDATE，`controls_break` 首次出現）：
```json
{
  "type": "TOOLBOX_UPDATE",
  "payload": {
    "toolbox": {
      "categories": [
        {
          "name": "控制流",
          "colour": "#5b80a5",
          "blocks": [
            { "type": "controls_loop" },
            { "type": "controls_break" }
          ]
        },
        { "...": "..." }
      ],
      "unlocked_factors": ["loop_termination"]
    }
  }
}
```

後端回應（WORLD_STATE，memory 扣減）：
```json
{
  "type": "WORLD_STATE",
  "payload": {
    "current_node": "infinite_loop_swamp",
    "memory": 80,
    "unlocked_factors": ["loop_termination"]
  }
}
```

> 失敗後，`discover_factor: "loop_termination"` 被觸發（來自 challenge YAML 的 `failure_consequence`），前端 toolbox 立即出現 `controls_break`。

---

#### ③ 玩家第二次執行（加入 Break 積木）→ 成功

前端發送：
```json
{
  "type": "EXECUTE",
  "payload": {
    "ast": {
      "type": "Loop",
      "condition": "count < 100",
      "act": "count++",
      "has_break": 1,
      "body": [
        { "type": "Break" }
      ]
    }
  },
  "request_id": "req-003"
}
```

後端回應（EXECUTION_RESULT，success）：
```json
{
  "type": "EXECUTION_RESULT",
  "payload": {
    "success": 1,
    "message": "🎉 很棒！你成功用 [中斷] 積木修復了無窮迴圈，挽救了即將崩潰的記憶體！執行成功！",
    "memory_cost": 0,
    "new_factors": []
  }
}
```

後端回應（WORLD_STATE）：
```json
{
  "type": "WORLD_STATE",
  "payload": {
    "current_node": "infinite_loop_swamp",
    "memory": 80,
    "unlocked_factors": ["loop_termination"]
  }
}
```

---

### 7.2 節點 YAML 與積木對應

`node.yaml` 中的 `reveal_layers` 是控制哪些積木「可見」的核心設定：

```yaml
# /content/nodes/infinite_loop_swamp/node.yaml
node_id: "infinite_loop_swamp"
name: "無窮迴圈沼澤"

reveal_layers:
  - reveal_block: "controls_break"      # 要解鎖的積木類型
    checks:
      - type: "factor_discovered"
        factor_id: "loop_termination"   # 條件：玩家必須已發現此因素
```

對應到後端 `blockly_service.c` 的積木登記表（`block_registry`）：

```lpc
// adventures/pw/mudlib/services/blockly_service.c
private mapping block_registry = ([
    // block_type  →  ({ "分類名稱", "顏色", "輸出類型" })
    "controls_loop":   ({ "控制流", "#5b80a5", "statement" }),
    "controls_break":  ({ "控制流", "#5b80a5", "statement" }),  // ← reveal_layer 解鎖後出現
    "variables_count": ({ "變數與運算", "#a55b80", "value" }),
    // ...
]);
```

**積木解鎖完整流程**：

```
node.yaml  reveal_layers
    │
    ▼  check: factor_discovered "loop_termination"
reveal_layer.c :: check_layer(player)
    │
    ▼  passed → reveal_block: "controls_break"
reveal_layer.c :: resolve_toolbox(player)
    │  return: { "controls_break": 1 }
    ▼
blockly_service.c :: query_player_toolbox(player)
    │  merge: base_blocks + 解鎖積木 → build_categories()
    │  return: { categories: [{ name: "控制流", blocks: [..., controls_break] }] }
    ▼
execute.c :: format_toolbox_update()
    │  json_encode(TOOLBOX_UPDATE)
    ▼
前端 PWBlocklyAdapter :: _handleToolboxUpdate()
    │  _buildToolboxXml(categories)
    ▼
Blockly.workspace.updateToolbox(xmlStr)
```

---

## 8. 實作檔案索引

| 檔案 | 位置 | 職責 |
|------|------|------|
| `blockly_service.c` | `adventures/pw/mudlib/services/` | PW 專屬 toolbox 格式化服務 |
| `execute.c` | `adventures/pw/mudlib/cmds/player/` | 接收前端訊息，路由至 node_executor |
| `pw-blockly-adapter.js` | `adventures/pw/mudlib/web/static/js/` | 前端 WebSocket 通訊適配器 |
| `index.html` | `adventures/pw/mudlib/web/static/` | 前端 Blockly 頁面 |
| `reveal_layer.c` | `adventures/pw/mudlib/std/` | 積木解鎖條件判斷（繼承自 node） |
| `node_executor.c` | `runtime/services/` | AST 匹配與挑戰判定（runtime 通用） |
| `factor_service.c` | `runtime/services/` | 因素解鎖管理（runtime 通用） |
| `node.yaml` | `content/nodes/<node_id>/` | 節點設定，含 reveal_layers 宣告 |
| `<challenge_id>.yaml` | `content/nodes/<node_id>/challenges/` | 挑戰定義，含 expected_ast 與結果訊息 |

---

## 9. 未來擴展規劃

### 9.1 新 Message Type

| Type | 方向 | 說明 | 優先度 |
|------|------|------|--------|
| `PLAYER_ACTION` | 前→後 | 玩家移動、互動等非積木動作 | 中 |
| `STAGE_UPDATE` | 後→前 | 關卡/章節進度更新（顯示進度條） | 中 |
| `DIALOGUE` | 後→前 | 導師/NPC 對話訊息（含說話者、頭像 key） | 高 |
| `HINT_REQUEST` | 前→後 | 玩家主動請求提示 | 低 |
| `HINT` | 後→前 | 回應提示請求 | 低 |
| `ERROR_DETAIL` | 後→前 | 詳細錯誤說明（可折疊顯示） | 低 |

### 9.2 `TOOLBOX_UPDATE` 擴充欄位

目前 `toolbox` 只有 `categories` 和 `unlocked_factors`，未來可加入：

```json
{
  "toolbox": {
    "categories": [ "..." ],
    "unlocked_factors": [ "..." ],
    "hidden_blocks": ["advanced_recursion"],
    "featured_block": "controls_break",
    "tooltip_overrides": {
      "controls_break": "💡 試著把這個積木放進迴圈裡！"
    }
  }
}
```

### 9.3 `node.yaml` 的 `recommended_toolbox` 擴充

節點可以直接宣告「此關卡推薦的積木快捷列」，讓玩家有更好的引導：

```yaml
# node.yaml 未來擴充
recommended_toolbox:
  categories:
    - name: "本關卡工具"
      blocks: ["controls_loop", "controls_break"]
  hint: "你需要一個能終止迴圈的積木…"
```

### 9.4 AST 類型擴充

隨著 PW Adventure 新增節點，AST 節點類型將擴充：

| AST type | 對應概念 | 預計節點 |
|----------|----------|----------|
| `Loop` | 迴圈 | `infinite_loop_swamp` ✅ |
| `Break` | 中斷 | `infinite_loop_swamp` ✅ |
| `Assignment` | 賦值 | `counter_valley` ✅ |
| `Conditional` | 條件判斷 | `if_else_peak`（未來） |
| `FunctionDef` | 函式定義 | `function_forest`（未來） |
| `FunctionCall` | 函式呼叫 | `function_forest`（未來） |
| `RecursiveCall` | 遞迴呼叫 | `recursion_abyss`（未來） |

### 9.5 多冒險支援

當 FSE 新增其他 Adventure（非 PW）時，架構支援如下擴展方式：

- 每個 Adventure 自行在 `adventures/<name>/mudlib/services/` 建立自己的 `blockly_service.c`
- 前端 adapter 可依 URL path 切換不同的後端服務（如 `/ws/pw`、`/ws/algo`）
- `runtime/services/node_executor.c` 保持 Adventure-Agnostic，通用邏輯不變

```
adventures/
├── pw/
│   └── mudlib/services/blockly_service.c    ← PW 專屬
├── algo/                                     ← 未來：演算法冒險
│   └── mudlib/services/blockly_service.c    ← Algo 專屬
└── math/                                     ← 未來：數學冒險
    └── mudlib/services/blockly_service.c    ← Math 專屬
```

### 9.6 前端 `PWBlocklyAdapter` 擴充點

`PWBlocklyAdapter` 已設計為可繼承的 class，未來新冒險可覆寫特定方法：

```javascript
class AlgoBlocklyAdapter extends PWBlocklyAdapter {
    // 覆寫 _dispatch 新增 Algo 專屬 message type
    _dispatch(msg) {
        if (msg.type === "ALGO_TRACE") {
            this._handleAlgoTrace(msg.payload);
            return;
        }
        super._dispatch(msg);
    }

    _handleAlgoTrace(payload) {
        // 顯示演算法步驟追蹤視覺化
    }
}
```

---

*本文件描述截至 2026-06 的實作狀態。協定版本為 1.0，未來若有破壞性變更，請同步更新 `type` 欄位版本號或在頂層加入 `"protocol_version"` 欄位。*
