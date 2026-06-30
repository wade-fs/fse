# 第 17 章　Runtime 擴展與貢獻

FSE Runtime 是一套中立的學習與探索規則引擎。隨著 Adventure 形式的豐富，Runtime 本身也需要持續且安全的演進。

---

### 1. Runtime Freeze (凍結原則)

為了確保現有世界的穩定性，FSE Runtime 遵循 **Runtime Freeze** 原則：
- **核心代碼保護**：位於 `runtime/` 內的核心服務（如 `event_bus.c`, `node_executor.c` 等）不得隨意修改以適應單一 Adventure 的特有需求。
- **擴展優先**：所有特定領域的擴展，必須優先通過 **Mapping（映射）**、**Virtual Objects（虛擬物件）** 以及專屬的 **Service** 來解決。

---

### 2. 核心擴展機制

當你確實需要修改 Runtime 核心，請確保採用以下低耦合機制：

#### ① 事件驅動 (Event Hook)
Runtime 核心在關鍵生命週期會拋出事件，開發者可以訂閱這些事件，而無需更改核心實作：
- `"PlayerConfused"`: 當玩家挑戰失敗進入困惑狀態時觸發。
- `"FactorDiscovered"`: 當玩家解鎖特定 Discovery 因素時觸發。
- `"StageAdvanced"`: 當全局階段推進時觸發。

#### ② 虛擬物件編譯器 (`virtual_object.c`)
FSE 將世界的狀態與情境高度資料化。若要增加新的地標（Node）或怪物（NPC）類型，請擴展 `virtual_object.c` 的編譯分支，讀取對應的 YAML 設定，而不是在代碼中寫死邏輯。

---

### 3. 貢獻流程

1. **先在 Adventure 實驗**：新概念先在特定冒險中開發（例如先在 PW 驗證 AST 匹配）。
2. **抽象與分離**：當兩個以上的 Adventure 均提出相同需求時，方可考慮抽象至 `runtime/`。
3. **單元測試與驗證**：修改 Runtime 後，必須使用 PW (Programming World) 進行全閉環驗證，確保沒有破壞「情境 ➔ 行動 ➔ 困惑 ➔ 領悟 ➔ 新路徑」的核心循環。
