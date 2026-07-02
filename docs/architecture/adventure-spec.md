# FSE 2.0 冒險結構與一級知識模型規範 (Adventure Specification)

本文件定義了一個 FSE 2.0 冒險模組（Adventure Module）的標準結構、目錄約束，以及跨冒險共用的第一級知識模型（Knowledge Model）。

---

## 一、 冒險解剖學與標準目錄結構

每個冒險模組必須遵循以下標準目錄結構，以確保引擎工具鏈能正確引導與編譯：

```
adventures/<adventure_id>/
├── manifest.yaml          # Identity 宣告與全路徑配置單一真理來源
├── novice_map.yaml        # 宣告式地圖結構定義
├── scaffold_node.py       # 宣告式節點與挑戰編譯工具
├── generate_map_viz.py    # 地圖聯通 Mermaid & SVG 可視化工具
│
└── mudlib/
    ├── master.c           # 冒險啟動入口 (Master Object)
    ├── std/               # 專屬繼承類別 (user.c, presence.c, fse_room.c)
    ├── services/          # 專屬加載服務 (如 karma_service.c)
    └── content/
        ├── nodes/         # 所有場景節點定義 (包含 challenges/ 與 discoveries/)
        ├── npcs/          # 存在體 NPC 定義
        ├── objects/       # 實體物品定義
        ├── factors/       # 永久記憶概念與 Factor 定義
        ├── progression/   # 進度階段 YAML
        └── locales/       # 多國語言 i18n 對譯表
```

---

## 二、 知識架構 (Knowledge Architecture)

在 FSE 2.0 中，**知識 (Knowledge) 是第一級概念**。
挑戰（Challenge）僅是檢驗知識的窗口；**知識才是真正可重複利用、跨節點甚至跨冒險共用的核心資產。**

為了讓所有冒險共用同一套知識模型，我們將 Knowledge 定義為以下五大類型：

```
          ┌──────────────────────────────────────────┐
          │            Knowledge (知識)              │
          └────┬──────────┬──────────┬─────────┬─────┘
               │          │          │         │
               ▼          ▼          ▼         ▼
          ┌────────┐ ┌─────────┐ ┌───────┐ ┌──────────┐
          │Concept │ │Principle│ │Skill  │ │Procedure │ ... (Relationship)
          └────────┘ └─────────┘ └───────┘ └──────────┘
```

### 1. `Concept` (概念類知識)
對某一客觀存在或現象的定性認知。
* **示例**：`stillness_resonance` (靜慮共鳴：理解什麼是心境的平靜)。
* **YAML 結構**：
  ```yaml
  id: "stillness_resonance"
  type: "Concept"
  desc: "理解風靜山平，心水無波的本質概念。"
  ```

### 2. `Skill` (技能/百工類知識)
完成特定物理/心智動作的熟練度與控制能力。
* **示例**：`blacksmithing` (打鐵鍛造)、`mining` (採礦)、`herbal_identification` (百草鑑別)。
* **YAML 結構**：
  ```yaml
  id: "blacksmithing"
  type: "Skill"
  desc: "掌控火候與鐵錘敲擊的物理技巧。"
  ```

### 3. `Principle` (原理/因果類知識)
多個變量或現象之間的定量、邏輯或因果關係。
* **示例**：`fair_exchange` (等價物換之理：商賈定價與凡欲的平衡)。
* **YAML 結構**：
  ```yaml
  id: "fair_exchange"
  type: "Principle"
  desc: "市井行情與人心偏誤的因果關係。"
  ```

### 4. `Procedure` (程序類知識)
完成特定複雜目標所必須遵循的嚴格、有序的步驟。
* **示例**：`alchemy_brew_step` (煉丹法門：先文火、後武火、再封爐)。
* **YAML 結構**：
  ```yaml
  id: "alchemy_brew_step"
  type: "Procedure"
  desc: "引火煉丹所必須對齊的三步運行法。"
  ```

### 5. `Relationship` (關係/共鳴類知識)
個體與周遭環境、靈力波動或 NPC 之間的契合與共鳴關係。
* **示例**：`sect_resonance` (宗門共鳴：多人於同地修煉時產生的契合倍率)。
* **YAML 結構**：
  ```yaml
  id: "sect_resonance"
  type: "Relationship"
  desc: "同門師兄弟共同冥想時引起的靈力共鳴波動。"
  ```
