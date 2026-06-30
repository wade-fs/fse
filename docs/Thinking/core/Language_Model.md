# FSE Language & Presentation Model (語言與表現模型規範)
# Version: v0.2

> **「理解的寬度，決定了世界的邊界。而世界與玩家的溝通，透過媒介（Medium）來傳遞。」**
> 本文件定義了 Flow State Engine (FSE) 中的「語言與表現模型」，並區分了通用的「 Reveal 層渲染機制」與特定冒險（如 PW）專屬的「語意/語法映射機制」。

---

## 一、世界溝通媒介 (World Communication / Input Medium)
【FSE 通用概念】

在 FSE 通用層，「語言」被抽象化為**「輸入媒介 (Input Medium)」**。不同冒險（Adventures）的溝通媒介各不相同，但底層驅動邏輯一致：

*   **`fsmud` 冒險**：媒介 ➔ **「自然語言/歷史語境」**（如：`travel 嘉義市`，或不同時代下日語/台語/客語的對話選單）。
*   **`DW (恐龍世界)` 冒險**：媒介 ➔ **「環境訊號」**（如：風向、氣味散播、聲音分貝、視覺陰影）。
*   **`PW (編程世界)` 冒險**：媒介 ➔ **「程式語言/抽象語法」**（如：Scratch 積木、Python/C 原始碼代碼）。

---

## 二、條件式顯示與視角過濾 (Reveal Layer)
【FSE 通用核心 ─ 適用於所有 Adventure】

Reveal Layer 負責控制玩家能「看見與感知什麼」，其本質是一個由玩家擁有的 Discovery (領悟) 狀態驅動的語意過濾器：

$$\text{Render}(S, Player\_Discoveries) \to \text{Semantics (Text / UI / Environment Blocks)}$$

### 1. 概念與歷史層 Reveal (fsmud 實例)
*   **無基礎 Discovery** ➔ 看到一般描述：`「一片雜草叢生的荒地。」`
*   **解鎖 Discovery "糖鐵興衰"** ➔ 看到 Reveal 歷史層描述：`「雜草下隱約露出一排鏽蝕的糖鐵軌道殘影。」`（地標新路徑開啟）

### 2. 環境與感官 Reveal (DW 恐龍世界實例)
*   **無基礎 Discovery** ➔ 看到一般森林描述：`「前方是一片幽暗的松林。」`
*   **解鎖 Discovery "獸腳類的夜間視力"** ➔ 看到 Reveal 安全線索：`「在松林的陰影深處，掠食者的雙眼正處於失焦狀態，你可以利用陰影避開其視線。」`

### 3. 介面與工具箱 Reveal (PW 編程世界實例)
*   **初學階段** ➔ Blockly 工具箱（Toolbox）隱藏複雜積木，僅提供基本順序執行積木（防範認知過載）。
*   **解鎖 Discovery "迴圈終止條件"** ➔ 工具箱動態 Reveal **`[直到...]` 迴圈積木** 與 **`[中斷]` 積木**。

---

## 三、多語言語意映射 (Multi-Language Semantic Mapping)
【PW (編程世界) 專屬擴充 ─ 其他 Adventure 不適用】

在編程冒險中，FSE 語言模型允許玩家在登入時選擇不同的「程式語言/抽象語法」來與同一個 World 互動。

### 1. 通用動作抽象化 (The Abstract AST)
不論玩家在瀏覽器端使用何種語法，其提交的邏輯都將在 FSE 中被解譯（Compile）為統一的抽象行為：

| 玩家選擇的語法類型 (Syntax) | 玩家輸入的表達式 (Expression) | FSE 內部語意映射 (AST Semantics) |
| :--- | :--- | :--- |
| **Scratch (積木)** | `[重複 10 次] ➔ [向右走]` | `Loop(times=10, act=Move(RIGHT))` |
| **Python (腳本)** | `for i in range(10): move_right()` | `Loop(times=10, act=Move(RIGHT))` |
| **C (指標/結構)** | `for(int i=0; i<10; i++) { right(); }` | `Loop(times=10, act=Move(RIGHT))` |

### 2. 答案與邏輯判定 (Semantic Validation)
*   PW 冒險 of Quest (任務) 在設定時，不需要為每種語言編寫獨立的判定。
*   YAML 檔案只需定義：
    `expected_ast: Loop(times=10, act=Move(RIGHT))`
*   後端 Go VM 或是 LPC 處理程序只負責比對玩家提交的表達式編譯後是否與 `expected_ast` 一致，從而實現了 100% 程式與資料分離。

---

## 四、Web / Browser 端積木程式整合設計
【PW (編程世界) 專屬擴充 ─ 其他 Adventure 不適用】

對於 Blockly / Scratch 等視覺化編程冒險：

1.  **積木 JSON 傳輸協定**：
    *   瀏覽器端的 Blockly Workspace 在點擊「執行 (Run)」時，將積木拓撲結構序列化為 **JSON AST**。
    *   此 JSON AST 透過 WebSocket 傳送至底層 FSE 引擎（`lpc-vm`）。
2.  **LPC 虛擬機中的邏輯重組**：
    *   `lpc-vm` 接收到 JSON 後，將其還原為虛擬機內部的執行序列，並依序觸發地標上的事件綁定（Event Binding）。
