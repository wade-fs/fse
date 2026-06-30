# 第 16 章　Adventure 開發指南

本章為世界建造者提供具體的開發指引，說明如何基於 FSE Core 建立一個全新的冒險世界（Adventure）。

---

### 1. 冒險世界的目錄結構

每一個 Adventure 都是一個獨立的 LPC 專案，透過動態掛載（Virtual Chroot）注入到 FSE 引擎中。一個標準的 Adventure 目錄結構如下：

```
adventures/<adventure_id>/
├── mudlib/
│   ├── cmds/
│   │   └── player/          # 玩家可用的領域指令（如 execute.c, look.c）
│   ├── services/            # 領域專屬服務（如 blockly_service.c, signal_service.c）
│   └── std/                 # 領域專屬基底物件（如 reveal_layer.c, user.c）
└── web/
    └── static/              # 前端靜態資源（HTML, JS, CSS, 圖片等）
```

---

### 2. 宣告與對應領域資料

在 FSE 中，世界是資料驅動的。你需要為你的 Adventure 定義節點（Nodes）、挑戰（Challenges）與因素（Factors）。

#### ① 節點設定 (`content/nodes/<node_id>/node.yaml`)
```yaml
node_id: "infinite_loop_swamp"
name: "無窮迴圈沼澤"
description: "這是一片由於未加中斷條件而陷入時間停滯的沼澤..."
reveal_layers:
  - reveal_block: "controls_break"      # 條件顯現層：解鎖 controls_break 積木
    checks:
      - type: "factor_discovered"
        factor_id: "loop_termination"   # 當且僅當玩家領悟了 loop_termination 因素
```

#### ② 挑戰設定 (`content/nodes/<node_id>/challenges/<challenge_id>.yaml`)
```yaml
challenge_id: "break_loop"
name: "中斷無窮"
expected_ast:                           # 期待的 AST 語意結構
  type: "Loop"
  has_break: 1
failure_consequence:
  memory_cost: 20                       # 失敗代價
  discover_factor: "loop_termination"   # 失敗觸發 Discovery 臨界：讓玩家領悟
  message: "程式陷入死循環，記憶體正在快速枯竭！"
success_message: "🎉 成功！你利用 [中斷] 阻止了死循環！"
```

---

### 3. 設計輸入媒介 (Input Medium) 與 AST 語意映射

不同冒險有不同的輸入媒介：
- **程式冒險**：接收 Blockly/Python 等輸入，前端轉成 AST 並傳遞給 `node_executor.c` 的 `match_ast` 機制。
- **生存冒險**：將指令解耦為感官訊號。例如將 `process_input` 替換成接收感官聚焦指令（`focus smell`），由 `signal_parser` 回傳環境參數（如氣味、震動）。

---

### 4. 接入 EventBus (事件連動)

世界的所有因果均由事件驅動。在你的 LPC 程式碼中，透過 EventBus 傳遞玩家行動的後果：

```lpc
// 廣播玩家行動
EVENT_BUS->publish("PlayerAction", ([
    "player" : this_player(),
    "action" : "execute",
    "ast"    : parsed_ast
]));
```

藉由這套指南，你可以快速搭建、註冊並測試一個全新的 FSE 世界，將核心的探索式理解循環帶給更多人。
