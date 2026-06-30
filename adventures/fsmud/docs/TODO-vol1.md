# 《源流福爾摩沙》開發進度與 TODO

## 🎯 當前進展與 Milestone 狀態

| 里程碑階段 | 功能目標 | 狀態 | 備註 |
|:---|:---|:---:|:---|
| **P0** | 民雄垂直切片 (Minxiong Vertical Slice) | **✓ 已完成** | 踏印獲得、記憶度增長、地標探索 look 關聯 |
| **P1** | YAML 驅動動態地標 (Site Aggregate) | **✓ 已完成** | 普通 LPC 地標解放為 YAML，`settlement_d` 動態複製生成 12 個 Sites |
| **P2** | 歷史記憶碎片系統 (Memory Fragment) | **✓ 已完成** | `memory_d` + `/data/yaml/memories/` YAML，玩家觸發共鳴並持久化存檔 |
| **P3** | 時代推展機制 (Era Progression) | **✓ 已完成** | `MemoryCompleted` 事件驅動 `world_progress`，達門檻自動觸發 `next_era()` |
| **P4** | 職涯與勢力系統 (Profession / Faction) | **✓ 已完成** | `career_d` + YAML 四職涯（農商匠文），`faction_d` + 三勢力（劉家/糖業/廟委），事件驅動自動累積修練點 |
| **P5** | 失源者、危機與共鳴 (Specter / Oblivion / Resonance) | **✓ 已完成** | `oblivion_d`（週期衰減+危機廣播）、`resonance_d`（多人共鳴）、`/std/specter.c`（可互動物件）、`cmd_commune` |
| **P7** | 新手任務與引導流程 (Tutorial Quest) | **✓ 已完成** | 以民雄「老站長」實作探索地標 -> 獲得踏印 -> 任務回報循環 |

---

## 🛠️ 已完成事項摘要

### P0 — 民雄垂直切片
- 實作踏印 (`footprint_d`) 與踏印圖譜 (`footprint_atlas`) 持久化。
- 修復啟動時 `footprint_atlas` nil 指針崩潰 (SIGSEGV)。

### P1 — YAML 驅動地標
- 移除所有普通 LPC 地標檔案，改由 `/data/yaml/sites/minxiong/` 驅動（12 個 YAML 地標）。
- `settlement_d.c:get_site_object` 作工廠：優先找 `.c` 實體，其次動態 `clone_object("/std/site.c")` + `setup_from_yaml()`。

### P2 — 歷史記憶碎片
- 建立 `/data/yaml/memories/`，放置記憶片段定義（含觸發地標、前置條件、進度權重）。
- `user.c` 新增 `unlocked_memories` 屬性，確保跨登入持久化。
- `memory_d.c` 在玩家進入地標時自動比對 `trigger_site`、前置條件並觸發解鎖。

### P3 — 時代推展
- **事件鏈完整打通**：
  - `memory_d.c` 解鎖後呼叫 `EVENT_D->publish("MemoryCompleted", {..., "progress": N})`。
  - `timeline_d.c` 訂閱 `MemoryCompleted`，在 `on_memory_completed()` 裡累加 `world_progress`。
  - 達到 YAML `min_progress` 門檻後自動呼交 `next_era()`，廣播並發送 `EraShifted` 事件。
- **資料驅動門檻**：`min_progress` 由各時代 YAML 設定，非硬編碼。
- **民雄記憶地圖**：新增 6 則記憶片段覆蓋 `ghost_house`、`sugar_factory_ruins`、`dashiye_temple`，構成完整解鎖路徑。
- **`memory` 玩家指令**：新增 `cmd_memory.c`，可列出記憶清單、查看時代進度、閱讀片段全文。

### P6 — 跨區域地圖與地理網絡擴展
- 修正 `route_minxiong_chiayi.yaml` 中屬性名稱 `notes` 改為 `nodes` 錯誤。
- 實作嘉義市主聚落地標 (`chiayi_city.yaml`) 與附屬 Sites（`chiayi_city_temple.yaml`、`chiayi_train_station.yaml`），補全與美化設定與描述。
- 實作新港鄉主聚落地標 (`singang.yaml`) 與其附屬 Site `singang_market.yaml`（替代舊 `singang_site.yaml`），修正新港聚落 `singang.yaml` 的 sites 清單。
- 完善 `std/site.c` 處理無 `entity_id` 玩家造成的 nil map indexing 執行期異常。
- 調整 `testlib` 測試環境，補全必要的 `std` 繼承檔案與 `daemon/yaml` 軟連結，確保核心測試（276/276）在 MudScript 模式下完全通過。

### P7 — 新手任務與引導流程
- 以民雄的「老站長」為核心，實作新手引導任務「老站長的心願」。
- 修復 `footprint_atlas` 存檔問題，移除 `std/user.c` 內多餘宣告以避開序列化引擎死角。
- 更新了 `testlib/std/user.c` 軟連結，確保自動測試環境吃到所有最新修改。
- `minxiong_old_station.yaml` 中移除自動賦予糖鐵踏印的邏輯，移至 `minxiong_market.yaml`，確保「探索市集獲取踏印 -> 回報車站」的邏輯順暢。
- 補全 `test_quest_loop.c` 整合測試並確認全部通過。

---

## 📋 下一步規劃

### P8 — 職涯動作與勢力事件玩法深化
- [x] 實作不同職涯（農商匠文）的專屬動作或命令（已完成 `cmd_farm.c`, `cmd_trade.c`, `cmd_craft.c`, `cmd_record.c`，並緊密結合歷代台灣產業脈絡）。
- [x] 設計與實作「廟委鎮煞委託」任務（含前置任務鎖定、組隊秘境線索、收集鎮符石回報、聲望與護符獎勵）與完整流程整合測試。
- [x] 設計與實作「糖鐵搶修委託」任務與整合測試（已完成，詳見 [sugar_railway_repair.md](quests/sugar_railway_repair.md)）。
- [x] 設計各大勢力（劉家、糖業、廟委）的聲望影響與專屬派系任務。（劉家地契委託、糖鐵搶修委託、廟委鎮煞委託均已實作並通過整合測試）
- [x] 設計週期性的「遺忘浪潮」危機事件與失源者（Specter）互動/共鳴儀式（已完成設計規劃，詳見 [oblivion_and_resonance.md](quests/oblivion_and_resonance.md)）。
- [x] 依設計實作週期性的「遺忘浪潮」危機事件與失源者共鳴儀式整合測試與功能（整合測試 [test_oblivion_resonance.c](mudlib/tests/test_oblivion_resonance.c) 已實作並通過）。
- [ ] 規劃下一個任務：優化跨區域地理網絡與多重歷史時代切換機制。
- [x] **P8.1 — 動態跨區域地理與時代切換路徑機制** (推薦)
  - [x] 實作地理路線 YAML 支援時代過濾條件（如 `era_active` 欄位限制）。
  - [x] 在 `route_d` 監聽 `EraShifted` 事件，動態重繪地理路徑並廣播變更。
  - [x] 設計不同時代在同路線下的動態描述與 look/travel 差異。
  - [x] 撰寫 `test_era_routes.c` 整合測試驗證。
- [x] **P8.2 — 歷史文獻記錄與傳言 (Rumor & Chronicles) 系統**
  - [x] 實作 `chronicle_d.c` 收集玩家大型事件與解除失源者的紀錄。
  - [x] 實作 `cmd_record.c` 供文人玩家編寫「地方誌」與文獻。
  - [x] 設計地方誌發佈與閱讀機制，閱讀他人地方誌可獲得記憶值與職涯修練點。

### P9 — 小說卷一主線地理與多重歷史時代切換實作
為打通小說卷一的主線路徑（民雄 → 嘉義 → 新港 → 台南 → 鹿港），我們需要開始補全真實的地理地標、歷史層與記憶碎片：
- [x] **P9.1 — 台南聚落與多歷史層模板實作（安平）** (優先)
  - [x] 建立 `tainan` 主聚落 YAML 檔。
  - [x] 實作 `anping` 地標與跨三個時代的歷史層：
    - v0.2 海商紀：鄭成功渡海事件場景與記憶碎片（NL000d 鄭成功）
    - v1.0 清領：安平古堡與貿易港口地標
    - v2.1 日治/西來庵時期：安平歷史遺跡與傳說
  - [x] 實作西拉雅新市地標（v0.1 時代，西拉雅族記憶碎片）。
  - [x] 建立對應的地理路線（如 `route_chiayi_tainan.yaml`）。
- [x] **P9.2 — 鹿港工藝聚落與職涯深化**
  - [x] 補全 `lukang` 聚落之實體地標與記憶碎片。
  - [x] 設計工匠專屬的工藝創作（木雕、製香）玩法與地標結合。
- [x] **P9.3 — 彰化與八卦山戰役歷史重現**
  - [x] 補全彰化與八卦山地標，實作 v2.0 時代之乙未戰爭歷史層與關鍵 NPC 互動。

---

## 🗺️ 全新開發階段：歷史內容平台 (Roadmap)

### 🛠️ 第一階段：內容自動化工作流 (Content Pipeline)
*   **P11 — 內容脚手架生成工具 (Content Scaffolding Tools)** (優先)
    *   [x] 設計 `make new-site <site_id>` 工具腳本，一鍵自動生成對應的 YAML 模板（`sites/`、`memories/`、`npcs/` 等）。
    *   [x] 建立 `Content Checklist` 規範檔，定義 Site 的基本標配元素（Look, Travel, NPC, Rumor, Memory, Hidden Object 等）。

### 📜 第二階段：歷史事件重現 (Historical Incident)
*   **P12 — 歷史事件聚合器 (Incident System)**
    *   [x] 設計 `incident` 概念層，作為 Site、NPC、Quest、Memory 的聚合器（如「民雄糖業事件」）。
    *   [x] 玩家不再只是單純解任務，而是圍繞一個核心 Incident 進行多線調查。

### 🏡 第三階段：經典村落 100% 密度 (Minxiong Complete)
*   **P13 — 經典民雄極致深化**
    *   [x] 將民雄的 Sites 密度由 12 個提升至 50 個以上（增加警察宿舍、舊街、米店、水圳、照相館、藥房等）。
    *   [x] 豐富各個新 Site 的描述與小景，建立完美的教學級經典區域範本。

### 🚶 第四階段：活生生的日常模擬 (NPC Daily Life)
*   **P14 — NPC 日常生活軌跡模擬**
    *   [x] 實作 `schedule_d.c` 虛擬時間 daemon（15秒=15遊戲分鐘，廣播 `GameTimeTick` 事件）。
    *   [x] 在 `npc.c` 新增日程表屬性（`schedule_routines`, `schedule_overrides`）與 `on_time_tick()` 自動移動邏輯。
    *   [x] `npc_d.c` 解析 YAML `schedule` 區塊並套用至 NPC 物件。
    *   [x] 新增 `is_npc()` 與 `query_action_msg()`，讓 `site.c` look 時能顯示「NPC 正在做什麼」。
    *   [x] 支援 era_active / global_event 兩種條件的 override 日程（跨時代 NPC 軌跡）。
    *   [x] 實作 `cmd/player/time.c` 讓玩家查詢遊戲時間與場景 NPC 動態。
    *   [x] 新增 3 個有日程的民雄示範 NPC：老站長（市場→車站）、市場阿嬤、行腳商人（跨聚落：民雄→嘉義→新港）。
    *   [x] 新增 `NPC_sugar_inspector`（糖業總辦），日治時代在糖廠/酒館，清領時代 override 消失。
    *   [x] 修正 `incident_d.c` `string[]` 語法問題。

### 🌪️ 第五階段：動態世界事件 (Dynamic World Event)
*   **P15 — 動態環境與突發事件**
    *   [x] 實作世界事件守護進程 `world_event_d.c`，自 YAML 載入世界事件定義。
    *   [x] 支援定時（scheduled）與隨機（random）事件觸發與全伺服器廣播通知。
    *   [x] 整合 `site.c`：事件發生時在 `look` 附加地標突發描述。
    *   [x] 整合 `do_travel` 移動限制：當路線或目標地標被事件封鎖時限制通行。
    *   [x] 整合 `timeline_d.c` 與 `npc.c`：世界事件觸發時切換 global_event 旗標，進而觸發 NPC override 突發日程。
    *   [x] 提供 3 個 YAML 世界事件示範：颱風警報（封鎖路線並覆寫各地標描述）、大士爺祭典、糖鐵出軌事故。
    *   [x] 整合 `time` 指令顯示當前活躍的世界事件，並在 `test_world_events.c` 中撰寫核心流程驗證。

---

### 🚀 第六階段：探索進度與歷史拼圖 (Exploration & Evidence)
*   **P16 — 歷史事件進度系統 (Incident Progress)**
    *   [x] 設計 `Incident Progress` 機制，展示歷史事件的調查完成率（如「糖業興衰：15%」）。
    *   [x] 串接各 Sites 點位的完成狀態（宿舍 + 糖廠 + 糖鐵 = 40%），達 100% 時解鎖隱藏的「歷史真相」。
*   **P17 — 記憶碎片實體化與證據鏈 (Memory to Evidence)**
    *   [x] 記憶不再只是收集，而是可作為證據（Evidence），例如：照片、日記、報紙、官方紀錄、口述等。
    *   [x] 實作多個證據的收集鏈，最後透過玩家的「歷史推理」解鎖真正的 Historical Truth。
*   **P18 — 地點與聚落探索度 (Site & Settlement Completion)**
    *   [x] 實作單一地標的探索度（0% ~ 100%），如民雄市場 100%、鬼屋 63%。
    *   [x] 實作宏觀聚落探索度（Settlement Completion，如民雄 83%、嘉義 15%、鹿港 5%），做為玩家的長期探索目標。
*   **P19 — 歷史大事紀圖鑑 (Chronicle Encyclopedia)**
    *   [x] 將 Chronicle 升級為玩家的「歷史百科圖鑑」。
    *   [x] 當玩家首次造訪或解鎖特定項目時，自動更新圖鑑內的 Memory、Quest 與 Incident 百科欄位。

### 👥 第七階段：動態世界狀態與人際網絡 (World State & Relationships)
*   **P20 — NPC 好感度與關係網 (NPC Relationship)**
    *   [x] 實作 NPC 好感度層級（熟識 ➔ 朋友 ➔ 信任 ➔ 知己）。
    *   [x] 隨著好感度提升，NPC 對話與解鎖任務將動態改變（如老站長從一開始的客套，變為拜託私事，最後吐露心聲）。
*   **P21 — 歷史事件驅動的世界狀態 (World State Evolution)**
    *   [x] 實作由 Incident 進度推動的世界動態變化（如：修好糖廠 ➔ 糖鐵重新開放 ➔ NPC 開始搭車 ➔ 市場新增商人 ➔ Quest 改變 ➔ Memory 更新）。
    *   [x] 降低 Era（時代）的生硬切換，改由 Incident 觸發 World State 動態漸變。

### 📋 聚落驗收標準 (Settlement Definition of Done)
*   **分級驗收規範**（詳見 [docs/settlement-standards.md](settlement-standards.md)）：
    *   [x] 建立 Tier S/A/B/C 分級門檻表（依行政層級與故事地位）
    *   [x] 全島 34 個聚落 Tier 對照表已建立
    *   [x] 卷一完成條件定義（6 個聚落達各自 Tier 80%）

*   **P23.1 — 民雄（Tier B）驗收狀態**（`make audit-settlement SETTLEMENT=minxiong`）：
    *   [x] Sites（地標）：52 / 12 ✅
    *   [x] NPCs（登場人物）：13 / 12 ✅（廟祝、農夫、剃頭師、照相館、教師、藥舖、糖廠工人、更夫、米店老闆娘）
    *   [x] Incidents（歷史事件）：3 / 3 ✅（鬼屋傳說、糖業事件、皇民化運動）
    *   [x] Memories（記憶碎片）：8 / 8 ✅
    *   [x] Quests（任務故事）：5 / 5 ✅（含「失去的名字」、「照相館後的秘密」、「更夫的最後一班」）
    *   [x] Rumors（傳言）：6 / 4 ✅
    *   [x] Dynamic Events（動態事件）：3 / 2 ✅
    *   [x] Hidden Areas（隱藏地區）：1 / 1 ✅
    *   [x] Era ≥3 / Site 品質：10 / 10（20% 達標）✅
    *   **🎉 9/9 全部通過！民雄 Tier B 達標**

*   **下一個聚落目標**（卷一完成條件）：
    *   [x] `chiayi_city` → Tier A 80%（**🎉 9/9 通過！**）
    *   [x] `singang` → Tier B 80%（**🎉 9/9 通過！**）
    *   [x] `tainan` → Tier S 50%（**🎉 9/9 通過，已達到 100% 超高標！**）
    *   [x] `lukang` → Tier B 80%（**🎉 9/9 通過！**）
    *   [x] `changhua_city` → Tier B 80%（**🎉 9/9 通過！**）
    *   [x] **全台灣 37/37 個聚落已全數通過 check_settlement.py 的 100% 綠燈驗收！**


---

### 🎨 未來中長程規劃
*   **P22 — 內容可視化配置工具 (Content Authoring Tools)**
    *   [x] 實作 `author.html` 獨立網頁工具，提供地標網路可視化。
    *   [x] 整合 YAML 瀏覽與線上編輯 (Monaco Editor)。
    *   [x] 實作 Dashboard 概覽數據統計。
*   **P23 — 卷一聚落驗收標準落地 (Settlement Definition of Done)**
    *   **P23.1 — CI 自動稽核腳本**（Layer 1：每次 commit 自動跑）✅ 完成
        *   [x] 實作 `check_settlement.py`：掃描 YAML 目錄，輸出 9 項驗收指標進度報告。
        *   [x] 支援分級門檻（Tier S/A/B/C）自動偵測與 `--tier` 強制指定。
        *   [x] 支援 `python3 check_settlement.py <settlement_id>` 與 `--all` 模式。
        *   [x] 加入 `Makefile` 目標 `make audit-settlement`。
        *   [x] **民雄 Tier B 全部通過（9/9 項）**
    *   **P23.2 — Wizard 遊戲內稽核指令**（Layer 2：開服前人工確認）
        *   [x] 實作 `cmd/wizard/audit.c`：在 MUD 內執行 `audit minxiong`。
        *   [ ] 輸出哪些 Site 沒有記憶/任務觸發（孤立地標）。
    *   **P23.3 — 玩家實測流程文件**（Layer 3：Beta 測試）
        *   [ ] 建立 `docs/todo-list/README-P23.md` 人工測試清單。
        *   [ ] 定義探索度 100% 的通關路徑地圖。




