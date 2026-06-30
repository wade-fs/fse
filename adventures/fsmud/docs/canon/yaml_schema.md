# 《源流福爾摩沙》YAML 資料欄位規範 (YAML Schema Specifications)

為了承載龐大的歷史世界觀設定，同時確保直譯器引擎 `yaml_decode` 能正確將靜態設定載入並與 LPC 型態融合，本文件定義了專案中所有 YAML 設定檔的正式 Schema 與欄位定義。

---

## 1. 全域設計原則

1. **靜動分離 (Static-Dynamic Separation)**
   - YAML 僅用於存放**唯讀的靜態元數據 (Static Metadata)**。
   - 動態變動數值（如聚落當前六維、失源者狀態）在運行時由 LPC 的 `save_object()` 與 `restore_object()` 寫入 `.o` JSON 存檔中，**不可**備份於靜態 YAML 內。
2. **多語系支援格式 (Multilingual Fields)**
   - 凡是面向玩家呈現的文字欄位（名稱、描述、對話），若需支援多語系，應統一使用 `mapping` 結構：
     ```yaml
     name:
       en: "English Name"
       zh-TW: "繁體中文名稱"
       zh-CN: "简体中文名称"
     ```
3. **路徑規範 (Path Standards)**
   - 聚落與地標設定存放於 `/data/yaml/` 下。
   - 系統規則、NPC、任務、技能與公會等全局實體存放於 `/mudlib/world/` 下。

---

## 2. 聚落 Schema (Settlement Schema)
- **檔案位置**：`/data/yaml/settlements/{settlement_id}.yaml`
- **對應常數/結構**：`SETTLEMENT_D` 載入，與動態存檔融合。

### 欄位定義表
| 欄位名 | 型態 | 必填 | 說明 | 範例 |
| :--- | :--- | :--- | :--- | :--- |
| `id` | 字串 | 是 | 聚落唯一標識符，須與檔名一致。 | `"minxiong"` |
| `name` | 字串 | 是 | 聚落顯示名稱。 | `"民雄鄉"` |
| `tier` | 整數 | 是 | 聚落初始等級（對照 `formosa.h`：`4`=村, `3`=3級城, `2`=2級城, `1`=1級城）。 | `4` |
| `admin_level` | 字串 | 是 | 行政層級（如 鄉、鎮、縣轄市）。 | `"鄉"` |
| `county` | 字串 | 是 | 所屬縣市 ID。 | `"chiayi_county"` |
| `population` | 整數 | 是 | 初始人口值 (0-100)。 | `40` |
| `industry` | 整數 | 是 | 初始產業發展度 (0-100)。 | `35` |
| `culture` | 整數 | 是 | 初始文化底蘊度 (0-100)。 | `48` |
| `memory` | 整數 | 是 | 初始歷史記憶值 (0-100)。 | `61` |
| `trade` | 整數 | 是 | 初始貿易繁榮度 (0-100)。 | `38` |
| `cohesion` | 整數 | 是 | 初始向心凝聚力 (0-100)。 | `52` |
| `specters_active` | 陣列 | 是 | 初始作用中之失源者列表，預設為空陣列。 | `[]` |
| `eras_present` | 陣列 | 是 | 該聚落存在的歷史時期 (Era ID 列表)。 | `["v0.1", "v2.0", "modern"]` |
| `industries` | 陣列 | 否 | 地方特色產業清單。 | `["糖業", "鳳梨"]` |
| `temples` | 陣列 | 否 | 當地重要信仰與廟宇列表，每個項目含 `name` 與 `deity`。 | 參見範例 |
| `sites` | 陣列 | 是 | 該聚落轄下之地標 (Site ID 列表)。 | `["ghost_house", "outskirts"]` |
| `memory_breakdown` | 映射 | 是 | 初始記憶子項目權重分配（`history_events`, `oral_tradition`, `named_persons`, `sites`）。 | 參見範例 |
| `map_art` | 字串 | 否 | 拓撲地圖 ASCII 示意圖（供開發與 look 參考）。 | 參見範例 |
| `map_data` | 映射 | 是 | 站點連接網格拓撲結構（包含與其他 Site 的 connections 陣列）。 | 參見範例 |

### 完整範例
```yaml
id: "minxiong"
name: "民雄鄉"
tier: 4
admin_level: "鄉"
county: "chiayi_county"
population: 40
industry: 35
culture: 48
memory: 61
trade: 38
cohesion: 52
specters_active: []
eras_present:
  - "v0.1"
  - "v2.0"
  - "modern"
industries:
  - "糖業"
  - "鳳梨"
temples:
  - name: "民雄大士爺廟"
    deity: "大士爺"
sites:
  - ghost_house
  - outskirts
memory_breakdown:
  history_events: 0.18
  oral_tradition: 0.25
  named_persons: 0.10
  sites: 0.08
map_art: |
  ghost_house -- outskirts
map_data:
  ghost_house:
    connections:
      - outskirts
  outskirts:
    connections:
      - ghost_house
```

---

## 3. 地標 Schema (Site Schema)
- **檔案位置**：`/data/yaml/sites/{settlement_id}/{site_id}.yaml`
- **對應常數/結構**：載入地標拓撲與初始化 Site 物件之用。

### 欄位定義表
| 欄位名 | 型態 | 必填 | 說明 | 範例 |
| :--- | :--- | :--- | :--- | :--- |
| `id` | 字串 | 是 | 地標唯一標識符，須與檔名一致。 | `"minxiong_old_station"` |
| `connections` | 陣列 | 是 | 連接的地標 ID 清單。 | `["minxiong_market"]` |

### 完整範例
```yaml
id: minxiong_old_station
connections:
  - minxiong_market
```

---

## 4. 路線與通道 Schema (Route/Path Schema)
- **檔案位置**：`/data/yaml/routes/{category}/{route_id}.yaml`
- **對應常數/結構**：描述聚落間（聯外）或地標間（聯內）的交通與旅行參數。

### 欄位定義表
| 欄位名 | 型態 | 必填 | 說明 | 範例 |
| :--- | :--- | :--- | :--- | :--- |
| `id` | 字串 | 是 | 路線唯一識別 ID。 | `"route_minxiong_singang"` |
| `name` | 字串 | 否 | 路線可讀名稱。 | `"縱貫線民雄－嘉義段"` |
| `transport_type`| 字串 | 否 | 交通工具形式限制（如 railway, foot, horse）。 | `"railway"` |
| `nodes` | 陣列 | 是 | 途經或端點節點 ID 清單。 | `["outskirts", "singang_market"]` |
| `bidirectional`| 布林 | 是 | 是否為雙向通道（在 LPC 解碼為 0 或 1）。 | `true` |
| `distance_km` | 浮點 | 否 | 路線距離（單位：公里）。 | `11.4` |
| `type` | 字串 | 是 | 道路形式（如 railway, county_road, canal_path）。 | `"county_road"` |
| `travel_time` | 混合 | 是 | 移動耗時。可為單一字串，或載具映射結構。 | `train: 12m` 或 `"2h"` |
| `available_eras`| 陣列 | 否 | 該通路存在的歷史時期列表（若不限則留空）。 | `["japanese", "modern"]` |

### 完整範例
```yaml
id: west_line_minxiong_chiayi
name: "縱貫線民雄－嘉義段"
transport_type: "railway"
nodes:
  - minxiong_station
  - chiayi_station
bidirectional: true
distance_km: 11.4
type: "railway"
travel_time:
  train: "12m"
available_eras:
  - japanese
  - roc
  - modern
```

---

## 5. 時代時期 Schema (Era Schema)
- **檔案位置**：`/mudlib/world/eras/{era_id}.yaml`
- **對應常數/結構**：`TIMELINE_D` 載入，用於驅動全服世界線演進。

### 欄位定義表
| 欄位名 | 型態 | 必填 | 說明 | 範例 |
| :--- | :--- | :--- | :--- | :--- |
| `id` | 字串 | 是 | 時代 ID，例如 `v0_1`。 | `"v0_1"` |
| `name` | 字串 | 是 | 時代可讀名稱。 | `"荒蠻紀"` |
| `time_range` | 字串 | 是 | 歷史年份跨度。 | `"史前～1624"` |
| `theme` | 字串 | 是 | 時代核心主題。 | `"島嶼最初的主人"` |
| `forces` | 陣列 | 否 | 活躍勢力名稱。 | `["西拉雅", "洪雅"]` |
| `mechanics` | 陣列 | 否 | 啟用或關聯之特色機制。 | `["部落聲望", "古道探索"]` |

### 完整範例
```yaml
id: "v0_1"
name: "荒蠻紀"
time_range: "史前～1624"
theme: "島嶼最初的主人"
forces:
  - "西拉雅"
  - "洪雅"
mechanics:
  - "部落聲望"
  - "祖靈踏印"
```

---

## 6. 歷史事件與劇本 Schema (Historical Event Schema)
- **檔案位置**：`/mudlib/world/events/{event_id}.yaml`
- **對應常數/結構**：描述全服或地區性的歷史事件劇本與玩家任務流。

### 欄位定義表
| 欄位名 | 型態 | 必填 | 說明 | 範例 |
| :--- | :--- | :--- | :--- | :--- |
| `id` | 字串 | 是 | 事件 ID。 | `"xilai_an"` |
| `name` | 字串 | 是 | 事件顯示名稱。 | `"西來庵事件"` |
| `era` | 字串 | 是 | 所屬時代。 | `"v2.1"` |
| `location` | 字串 | 是 | 發生地。 | `"噍吧哖"` |
| `type` | 字串 | 否 | 事件分類。 | `"抗日事件"` |
| `critical_npcs`| 陣列 | 否 | 關鍵歷史 NPC 列表。 | `["余清芳", "江定"]` |
| `player_flow` | 陣列 | 是 | 玩家在事件中的情境流或步驟描述。 | 參見範例 |
| `rewards` | 陣列 | 否 | 事件完成可獲得的踏印或其他獎勵。 | `["義氣踏印"]` |

### 完整範例
```yaml
id: "xilai_an"
name: "西來庵事件"
era: "v2.1"
location: "噍吧哖"
type: "抗日事件"
critical_npcs:
  - "余清芳"
  - "江定"
player_flow:
  - "第一階段：加入西來庵，取得神符"
  - "第二階段：武裝起義，襲擊派出所"
  - "第三階段：遭遇日軍，退守山林"
rewards:
  - "義氣踏印"
```

---

## 7. 配方與合成 Schema (Recipe Schema)
- **檔案位置**：`/mudlib/world/recipes/{recipe_id}.yaml`
- **對應常數/結構**：描述合成、鍛造與煉金系統的材料消耗與產出。

### 欄位定義表
| 欄位名 | 型態 | 必填 | 說明 | 範例 |
| :--- | :--- | :--- | :--- | :--- |
| `id` | 字串 | 是 | 配方 ID。 | `"health_potion"` |
| `type` | 字串 | 是 | 合成分類（如 alchemy, forging）。 | `"alchemy"` |
| `name` | 字串 | 是 | 配方成品中文名稱。 | `"初級恢復藥水"` |
| `materials` | 映射 | 是 | 材料消耗列表（鍵為材料 LPC 檔案路徑，值為所需數量）。 | 參見範例 |
| `result` | 字串 | 是 | 成品產出 LPC 檔案路徑。 | `"/item/consumable/health_potion.c"` |
| `msg` | 字串 | 是 | 合成成功時顯示給玩家的過場文字描述。 | `"你將黏液與草藥混合攪拌..."` |

### 完整範例
```yaml
id: "health_potion"
type: "alchemy"
name: "初級恢復藥水"
materials:
  "/item/material/slime_jelly.c": 2
  "/item/material/wood.c": 1
result: "/item/consumable/health_potion.c"
msg: "你將黏液與草藥混合攪拌，瓶中泛起了紅色的微光。"
```

---

## 8. 公會與職業 Schema (Guild/Career Schema)
- **檔案位置**：`/mudlib/world/guilds/{guild_id}.yaml`
- **對應常數/結構**：描述公會組織、加入限制與職涯階級名稱。

### 欄位定義表
| 欄位名 | 型態 | 必填 | 說明 | 範例 |
| :--- | :--- | :--- | :--- | :--- |
| `id` | 字串 | 是 | 公會唯一 ID。 | `"fighter"` |
| `name` | 映射 | 是 | 多語系公會名稱。 | 參見範例 |
| `desc` | 映射 | 是 | 多語系公會說明描述。 | 參見範例 |
| `ranks` | 陣列 | 是 | 職涯等級進度列表，各元素含多語系階級名稱映射。 | 參見範例 |
| `req` | 映射 | 否 | 加入條件限制（如 level, 基礎屬性）。 | `level: 5, str: 20` |

### 完整範例
```yaml
id: "fighter"
name:
  en: "Warriors Covenant"
  zh-TW: "戰士盟約"
desc:
  en: "The home of strength and glory."
  zh-TW: "力量與榮耀的歸宿。"
ranks:
  - en: "Trainee Fighter"
    zh-TW: "見習鬥士"
  - en: "Brave Warrior"
    zh-TW: "勇猛戰士"
req:
  level: 5
  str: 20
```

---

## 9. NPC 模板 Schema (NPC Template Schema)
- **檔案位置**：`/mudlib/world/npcs/{npc_id}.yaml`
- **對應常數/結構**：`NPC_D` 或各區域載入怪物與人物屬性樣板之用。

### 欄位定義表
| 欄位名 | 型態 | 必填 | 說明 | 範例 |
| :--- | :--- | :--- | :--- | :--- |
| `id` | 字串 | 是 | NPC 樣板 ID。 | `"wolf"` |
| `name` | 映射 | 是 | 多語系名稱。 | 參見範例 |
| `short_name` | 映射 | 是 | 多語系短期描述。 | 參見範例 |
| `long_name` | 映射 | 是 | 多語系詳細描述。 | 參見範例 |
| `id_list` | 陣列 | 是 | 玩家可用以指代該 NPC 的 ID 字串列表。 | `["wolf", "野狼"]` |
| `level` | 整數 | 是 | 初始等級。 | `3` |
| `stat_str` | 整數 | 否 | 基礎力量。 | `10` |
| `stat_dex` | 整數 | 否 | 基礎敏捷。 | `12` |
| `stat_con` | 整數 | 否 | 基礎體質。 | `9` |
| `exp_reward` | 整數 | 否 | 擊殺提供之經驗值。 | `90` |
| `gold_reward`| 整數 | 否 | 擊殺提供之金錢（銅幣單位）。 | `15` |
| `drop_list` | 陣列 | 否 | 隨機掉落裝備或物品 LPC 路徑列表。 | `["/item/weapon/short_sword.c"]` |
| `harvest_data`| 映射 | 否 | 採集/剝皮相關設定（含產出 `file` 與機率 `chance`）。 | 參見範例 |
| `respawn_time`| 整數 | 否 | 死亡後重生的冷卻秒數。 | `60` |
| `aggro_msg` | 字串 | 否 | 主動攻擊怪物進入戰鬥時顯示的字串。 | `"野狼嗥叫一聲..."` |

### 完整範例
```yaml
id: "wolf"
name:
  en: "Wolf"
  zh-TW: "野狼"
short_name:
  en: "a fierce wild wolf"
  zh-TW: "一匹兇猛的野狼"
long_name:
  en: "This is a gray-black wolf.\n"
  zh-TW: "這是一匹毛色灰黑的野狼。\n"
id_list:
  - "wolf"
  - "野狼"
level: 3
stat_str: 10
stat_dex: 12
stat_con: 9
exp_reward: 90
gold_reward: 15
drop_list:
  - "/item/weapon/short_sword.c"
harvest_data:
  file: "/item/material/wolf_fur.c"
  chance: 80
respawn_time: 60
aggro_msg: "野狼嗥叫一聲，露出鋒利的牙齒！\n"
```

---

## 10. 任務與劇本 Schema (Quest Schema)
- **檔案位置**：`/mudlib/world/quests/{quest_id}.yaml`
- **對應常數/結構**：`QUEST_D` 管理之任務設定。

### 欄位定義表
| 欄位名 | 型態 | 必填 | 說明 | 範例 |
| :--- | :--- | :--- | :--- | :--- |
| `id` | 字串 | 是 | 任務唯一識別 ID。 | `"newbie_badge"` |
| `name` | 映射 | 是 | 多語系任務名稱。 | 參見範例 |
| `desc` | 映射 | 是 | 多語系任務詳細描述。 | 參見範例 |
| `level` | 整數 | 是 | 建議與限制等級。 | `1` |
| `reward` | 映射 | 是 | 獎勵項目映射（可含 `exp`, `gold`, 以及特定裝備 `badge` 等）。 | 參見範例 |

### 完整範例
```yaml
id: "newbie_badge"
name:
  en: "Newbie Proof"
  zh-TW: "新手證明"
desc:
  en: "Prove your courage to the Guild Master."
  zh-TW: "向公會會長證明你的勇氣。"
level: 1
reward:
  exp: 100
  gold: 50
  badge: "/item/badge_newbie.c"
```
