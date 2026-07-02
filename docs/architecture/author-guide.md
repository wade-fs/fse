# FSE 2.0 創作者工作流與指南 (Author Guide)

本指南旨在指導一個故事創作者（Wizard）如何從零開始，利用 FSE 2.0 的自動化工具鏈與資料驅動規範，構建、編譯、測試並發布一個全新的冒險模組。

---

## 一、 創作者完整工作流 (Author Workflow)

```
 1. 建立 Adventure  ── 使用 create-adventure.py 初始化模組目錄結構
       │
       ▼
 2. 定義 Reality    ── 定義世界客觀律 (如 social, natural)
       │
       ▼
 3. 建立 Region     ── 在 novice_map.yaml 宣告地理與故事區域
       │
       ▼
 4. 建立 Site       ── 建立節點 Site/Node，設計感官發現與 interactions
       │
       ▼
 5. 建立 NPC        ── 建立 NPC 存在體，設定對話與可識別度 (Disguise) 規則
       │
       ▼
 6. 建立 Knowledge  ── 設計抽象知識點 (Concept, Skill, Principle 等)
       │
       ▼
 7. 建立 Challenge  ── 編寫 Challenge YAML，綁定 Aligned Action 與證據 Observations
       │
       ▼
 8. 測試驗證        ── 在 tests/ 編寫最小整合測試，確保 make test 綠燈通過
       │
       ▼
 9. 發布上線        ── 註冊到核心 Manifest 啟動遊戲
```

---

## 二、 實踐步驟指南 (Step-by-Step Tutorial)

### 步驟 1：建立 Adventure (Bootstrap)
進入系統根目錄，執行一鍵腳本建立模組：
```bash
python3 create-adventure.py --name my_adventure --title "山海經傳奇"
```
* 這會為你在 `adventures/my_adventure` 下生成整套標準目錄樹，包含 `mudlib/`、`master.c`、`manifest.yaml` 及預設的測試與編譯腳本。
* *註：若 `create-adventure.py` 尚未完成或無法直接執行，創作者亦可手動複製 `adventures/so/` 的結構，將其內部的 `so` 識別符替換為新冒險 ID，並清空 `content/` 目錄下的特定地標。*


### 步驟 2：定義 Reality 與 Knowledge
* 在 `/runtime/realities/`（若是核心共用）或冒險的 `/realities/` 下建立規律檔（如 `natural/gravity.yaml`）。
* 在 `content/factors/` 下建立知識定義檔（如 `gravity_master.yaml`）。

### 步驟 3：定義地圖與節點 (Scaffolding Sites)
* 打開 `novice_map.yaml`，用 YAML 宣告你的房間地標、sensory_signals (看/聽/聞) 以及與之關聯的行動（interactions）。
* 運行編譯腳本生成 LPC 實體檔：
  ```bash
  python3 scaffold_node.py --import_map novice_map.yaml
  ```
* 這會自動在 `content/nodes/<node_id>/` 下批量生成 `node.yaml`、挑戰範本以及發現物 YAML。

### 步驟 4.5：建立 NPC (可選)
* 在 `content/npcs/<npc_id>.yaml` 定義 NPC 屬性與偽裝辨識門檻。
* 在 `node.yaml` 的 `presence` 區塊宣告哪些 NPC 出現在此節點，例如：
  ```yaml
  presence:
    - id: "/npcs/monkey"
      count: 1
  ```
* 若 NPC 有互動對話，可以在 `node.yaml` 的 `interactions` 中加入 `action: "dialogue"` 並指定對應的對話 resolver。
* NPC 底層將繼承 `/runtime/core/actor.c`（透過 `presence.c` 虛擬加載）。

### 步驟 5：建立挑戰挑戰檔 (Challenge Binding)

* 在 `content/nodes/<node_id>/challenges/` 下打開對應的挑戰 YAML。
* 填寫成功的發現因子 `discover_factor`、成功與失敗的訊息、以及 `evolve` 因果副作用（數值改變或業力/狀態改變）。

### 步驟 5：編寫整合測試 (Integration Test)
* 打開 `mudlib/tests/test_my_adventure_loop.c`。
* 模擬玩家進入節點、觸發交互、校驗業力與屬性改變。
* 執行測試以確保一切順暢：
  ```bash
  make test
  ```
* 只要看到 `🎉 [SUCCESS] 測試全部通過！` 輸出，即代表你的冒險模組邏輯完全閉環！

---

## 三、 常見問題排查 (Troubleshooting Guide)

### Q: `observe_breath`（或其他互動行動）完全沒有任何反應
* **排查方式**：請確認 `node.yaml` 該項互動配置中是否宣告了 `resolver` 欄位（例如 `resolver: "start_cliff_first_contact"`），且對應的挑戰 YAML 檔案（例如 `challenges/start_cliff_first_contact.yaml`）確實存在於該節點的 `challenges/` 目錄下。

### Q: 理解三態判定成功（UNDERSTANDING）解鎖了，但玩家沒有獲得 Factor
* **排查方式**：請確認挑戰 YAML 中是否寫了 `discover_factor` 欄位（例如 `discover_factor: "gravity_master"`），並且其全域 Factors 宣告檔（例如 `content/factors/gravity_master.yaml`）已正確建立。

### Q: 業力（Karma）改變了，但世界路徑沒有重定向（心魔鬼打牆未生效）
* **排查方式**：確認 `node.yaml` 內是否定義了 `karma_loop_threshold`。若未定義，系統將預設使用全域預設值 `80`。請確認玩家的業力值是否確實跨過了該門檻。

### Q: 伺服器啟動或載入時，Reality Resolver 提示 "找不到 Law" 的 warning 資訊
* **排查方式**：請確認您在挑戰（Challenge）中所引用的 `knowledges` 列表格式（例如 `natural.gravity.observe_breath`）其結構是否對齊：
  * 對應的客觀規律 Law YAML `runtime/realities/natural/gravity.yaml` 必須存在。
  * 且該 Law 檔的 `evaluate` 區塊中的 `knowledge_id` 必須與挑戰引用的 `kn_id` 完全一致。

