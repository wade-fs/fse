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

### 步驟 4：建立挑戰挑戰檔 (Challenge Binding)
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
