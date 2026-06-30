# P23 — 小說第二卷地理擴展 (Volume 2 Expansion)

> **🎉 最新里程碑進展**：全台灣 37/37 個聚落已於 2026-06-28 全部 100% 通過 `check_settlement.py` 綠燈驗收，卷二地理、任務與 NPC 設定已全部補完並高質量去數字化重構！



> **卷名**：v1.2～v1.5｜民變年代・清領晚期
> **主題**：動盪中的根——反抗、械鬥、近代化開端
> **對應章節**：第 101～200 章

---

## 🎯 目標與概述

卷一（v0.1～v1.1）建立了嘉南走廊的主幹道，P23 的任務是打通**中台灣走廊**，讓玩家從彰化、雲林、台中、苗栗一路深入，並延伸至屏東牡丹鄉，見證清領時期三大民變（朱一貴、林爽文、戴潮春）與近代化轉折（牡丹社事件、台灣建省）。

本階段聚焦於：
1. **地理骨幹補全**：補齊第二卷核心聚落（高雄岡山/鳳山、彰化市、霧峰、雲林、苗栗、屏東牡丹、台北大稻埕）的 Sites 密度
2. **歷史事件 Incidents**：建立五大 Incident 系統檔案（朱一貴、林爽文、戴潮春、牡丹社事件、台灣建省）
3. **記憶碎片補全**：對應每個 Incident 設計 3～5 個多視角 Memories
4. **地理路線串聯**：完成中台灣路線網（彰化←→台中←→苗栗←→雲林），並在南北各端接入卷一地圖
5. **新職涯玩法整合**：「義氣踏印」、「口述採集者」、「修復師」、「說書人」的實際行當觸發點

---

## 📋 子任務分解

---

### P23.1 — 卷二核心聚落補完 (Settlement Expansion)

#### 📍 高雄岡山聚落 (`kaohsiung/gangshan`)
*對應章節：第 102～113 章（朱一貴事件）*

- [x] `gangshan.yaml` 補全：現為空骨架，補齊 name/eras/sites/industries
- [x] 新增 Sites：
  - `gangshan_duck_pond`（鴨母王養鴨地）：朱一貴故地，觸發「鴨母王的日常」記憶碎片
  - `gangshan_tian_di_hui`（天地會集會所）：v1.2 時代，觸發天地會暗流 Incident 入場點
- [x] 更新 `kaohsiung.yaml` settlement 的 sites 列表

#### 📍 鳳山聚落 (`kaohsiung/fengshan`)
*對應章節：第 103～109 章（府城自治）*

- [x] `fengshan.yaml` 補全：設定 eras（v1.0～v1.2），補齊 sites
- [x] 新增 Sites：
  - `fengshan_fort`（鳳山縣城遺址）：清廷官府視角，觸發「知縣的公文」記憶碎片
  - `tainan_zhuxingwang`（府城朱興王行宮）：v1.2 時代短暫自治首府，觸發「義軍的短暫春天」

#### 📍 彰化市聚落 (`changhua_city`) ⭐ 卷二主戰場
*對應章節：第 114～153 章（林爽文、戴潮春事件）*

- [x] `changhua_city.yaml` 補全（現有 496 bytes，需大幅擴充）：
  - 補齊 eras：`v1.0`, `v1.2`, `v1.5`, `v2.0`
  - 補齊 industries、temples、specters_active 初始設定
- [x] 新增 Sites：
  - `changhua_county_fort`（彰化縣城遺址）：v1.2 林爽文圍城、v1.5 戴潮春四年圍城的核心舞台
  - `changhua_nanputuo_temple`（南瑤宮廟埕）：說書人行當觸發點，玩家可在此「廟埕開講」
  - `changhua_confucian_temple`（彰化孔廟）：文人職涯的文獻存放地與地方誌提交點
  - `changhua_yi_min_temple`（彰化義民廟）：林爽文事件後義民信仰的萌芽地，觸發「義民信仰的萌芽」記憶
  - `changhua_old_market`（彰化舊市集）：「府城內的悲歡」普通百姓視角觸發點

#### 📍 霧峰聚落 (`taichung/wufeng`)
*對應章節：第 125 章（霧峰林家的雛形）*

- [x] 新增 settlement `wufeng.yaml`（霧峰鄉，台中市轄）
- [x] 新增 Sites：
  - `wufeng_lin_mansion`（霧峰林家宅邸）：地方豪族文教影響力，觸發「霧峰林家的雛形」記憶碎片
  - `wufeng_academy`（萊園私塾）：文人職涯進階觸發點

#### 📍 雲林聚落 (`yunlin`) — 補齊中部走廊
*對應章節：第 114～116 章（漳泉械鬥）*

- [x] `yunlin.yaml` 補全：補齊 sites、eras（v1.0, v1.2, v1.5）
- [x] 新增 Sites：
  - `yunlin_douliu_street`（斗六老街）：清領縣治中心，漳泉械鬥的衝突地帶
  - `yunlin_hakka_village`（雲林客家庄）：「轉移族譜」任務觸發地，協助客家老婦轉移族譜
  - `yunlin_irrigation_field`（漳泉水圳爭奪地）：「陳仔的鋤頭」任務觸發地，農人視角械鬥事件

#### 📍 苗栗聚落 (`miaoli`) — 近代化路線北端
*對應章節：第 174、181 章（劉銘傳鐵路、開山撫番）*

- [x] `miaoli.yaml` 補全：補齊 sites、eras（v1.5, v2.0）
- [x] 新增 Sites：
  - `miaoli_old_railway`（苗栗舊鐵路遺址）：劉銘傳鐵路路線的一段，觸發「苦力的汗水」記憶碎片
  - `miaoli_hakka_yi_min`（苗栗義民廟）：客家義民信仰北部支脈，與林爽文事件 Incident 連動
  - `dakeng_aboriginal_village`（大嵙崁原住民部落）：「開山撫番的代價」原住民視角記憶觸發點

#### 📍 屏東牡丹聚落 (`pingtung/mudan`) ⭐ 牡丹社事件核心
*對應章節：第 160～172 章（牡丹社事件）*

- [x] 新增 settlement `mudan.yaml`（牡丹鄉，屏東縣轄）
- [x] 新增 Sites：
  - `mudan_paiwan_village`（牡丹社排灣族部落）：主舞台，阿祿古斯 NPC 所在地，觸發「阿祿古斯的規矩」
  - `mudan_japanese_landing`（日軍車城登陸地）：1874 年日本遠征軍登陸點，觸發「日軍的足跡」記憶碎片
  - `mudan_stone_gate`（石門古戰場）：排灣族抵禦日軍的決戰地，觸發「牡丹社的怒吼」記憶碎片
  - `checheng_harbor`（射寮港）：清廷欽差沈葆楨的巡視入港點，觸發「欽差的考量」記憶

#### 📍 恆春鎮 (`pingtung/hengchun`) — 牡丹社輔助聚落
*對應章節：第 172 章（恆春半島地理踏印）*

- [x] 新增 settlement `hengchun.yaml`（恆春鎮，屏東縣轄）
- [x] 新增 Sites：
  - `hengchun_old_town`（恆春古城）：1875 年清廷在牡丹社事件後建立的邊防城池
  - `hengchun_south_gate`（恆春南門）：面向南海的地理踏印觸發點

#### 📍 台北大稻埕 (`taipei/dadaocheng`)
*對應章節：第 189 章（大稻埕的繁華）*

- [x] 現有 `taipei` 聚落補齊大稻埕 Sites：
  - `dadaocheng_tea_warehouse`（大稻埕茶行倉庫）：國際貿易場景，商人職涯觸發點
  - `dadaocheng_yanping_pier`（延平碼頭）：茶葉出口的重要港口，v1.5 時代核心

---

### P23.2 — 歷史事件 Incidents 建立

每個 Incident 作為多個 Sites、NPCs、Memories、Quests 的聚合器。

#### 🔥 EP001 — 朱一貴事件 (`incidents/ep001_zhu_yigui.yaml`)

```yaml
event_id: ep001_zhu_yigui
name: "朱一貴事件"
short_name: "鴨母王之亂"
era: "v1.2"
period: "1721年"
scope: [gangshan, tainan, chiayi_city]
phases:
  - id: "起義"       # 第102～108章
  - id: "自治"       # 第103章府城短暫自治
  - id: "鎮壓"       # 第109～110章清軍反攻
  - id: "尾聲"       # 第111～113章義氣踏印
key_npcs: [zhu_yigui, du_junying, zheng_zai, magistrate]
memories: [zhu_yigui_daily, short_spring, hakka_rift, magistrate_document, city_joys]
footprint_reward: "義氣踏印（朱一貴）"
```

- [x] 建立 `incidents/ep001_zhu_yigui.yaml`
- [x] 4個關鍵 NPC YAML：朱一貴、杜君英、鄭仔、倉皇知縣
- [x] 5個記憶碎片 YAML（見 P23.5）

#### 🔥 EP002 — 林爽文事件 (`incidents/ep002_lin_shuangwen.yaml`)

```yaml
event_id: ep002_lin_shuangwen
name: "林爽文事件"
short_name: "清領最大民變"
era: "v1.2"
period: "1786年～1788年"
scope: [changhua_city, yunlin, taichung, chiayi_city, tainan]
phases:
  - id: "天地會崛起"    # 第129章
  - id: "失控的版圖"    # 第131章
  - id: "南路莊大田"    # 第132章
  - id: "義民對決"      # 第133章
  - id: "彰化圍城"      # 第134章
  - id: "清軍反攻"      # 第141章
  - id: "全視角拼圖"    # 第142章
key_npcs: [lin_shuangwen, zhuang_datian, yi_min_leader, changhua_magistrate, old_wang]
memories: [spread_map, south_route, yi_min_opposition, changhua_fear, oxcart_bones]
footprint_reward: "義氣踏印（林爽文全視角）"
```

- [x] 建立 `incidents/ep002_lin_shuangwen.yaml`
- [x] 5個關鍵 NPC YAML
- [x] 5個記憶碎片 YAML

#### 🔥 EP003 — 戴潮春事件 (`incidents/ep003_dai_chaochun.yaml`)

```yaml
event_id: ep003_dai_chaochun
name: "戴潮春事件"
short_name: "四年的圍城"
era: "v1.5"
period: "1862年～1865年"
scope: [changhua_city, taichung, yunlin]
phases:
  - id: "八卦會崛起"    # 第145章
  - id: "烽火四年"      # 第146～149章
  - id: "平靜的渴望"    # 第151章
key_npcs: [dai_chaochun, clan_leader, chen_a_tu]
memories: [four_year_siege, clan_game, city_changes_hands, yearn_for_peace]
footprint_reward: "義氣踏印（戴潮春・宗族時代）"
```

- [x] 建立 `incidents/ep003_dai_chaochun.yaml`
- [x] 3個關鍵 NPC YAML
- [x] 4個記憶碎片 YAML

#### 🔥 EP004 — 牡丹社事件 (`incidents/ep004_mudan_incident.yaml`)

```yaml
event_id: ep004_mudan_incident
name: "牡丹社事件"
short_name: "日本第一次踏台"
era: "v1.5"
period: "1871年～1874年"
scope: [mudan, hengchun, tainan, taipei]
phases:
  - id: "琉球漁民遇難"   # 第157章
  - id: "帝國的藉口"     # 第158章
  - id: "牡丹社的怒吼"   # 第160章
  - id: "石門決戰"       # 核心戰役
  - id: "沈葆楨應對"     # 第167～168章
  - id: "政策轉向"       # 第169章
key_npcs: [arugus, jp_interpreter, shen_baozhen, ryukyu_survivor]
memories: [arugus_rules, japanese_footprints, interpreter_diary, shen_memorial, empire_probe]
footprint_reward: "時代踏印（牡丹社・祖靈認可）"
```

- [x] 建立 `incidents/ep004_mudan_incident.yaml`
- [x] 4個關鍵 NPC YAML
- [x] 5個記憶碎片 YAML

#### 🏛️ EP005 — 台灣建省 (`incidents/ep005_taiwan_province.yaml`)

```yaml
event_id: ep005_taiwan_province
name: "台灣建省"
short_name: "劉銘傳的鐵路夢"
era: "v1.5"
period: "1885年～1895年"
scope: [taipei, miaoli, taichung, keelung]
phases:
  - id: "首任巡撫"      # 第173章
  - id: "近代化建設"    # 第174～176章
  - id: "鐵路全線"      # 第177～179章
  - id: "苦力的汗水"    # 第178章
  - id: "建設廢弛"      # 第185～186章
key_npcs: [liu_mingchuan, lao_zhou, aboriginal_chief, tea_merchant_buyer]
memories: [modernization, laborers_sweat, aboriginal_cost, province_glory]
footprint_reward: "時代踏印（台灣建省・鐵路地理）"
```

- [x] 建立 `incidents/ep005_taiwan_province.yaml`
- [x] 4個關鍵 NPC YAML
- [x] 4個記憶碎片 YAML

---

### P23.3 — 地理路線補全 (Route Network)

#### 中台灣主軸（新增 8 條路線）

| 路線 YAML | 起點 | 終點 | 對應章節 |
|---|---|---|---|
| `route_tainan_kaohsiung.yaml` | 台南 | 高雄岡山 | 第 102 章 |
| `route_kaohsiung_pingtung.yaml` | 高雄 | 屏東（恆春/牡丹） | 第 160 章 |
| `route_lukang_changhua.yaml` | 鹿港 | 彰化市 | 第 114 章 |
| `route_changhua_taichung.yaml` | 彰化 | 台中（霧峰） | 第 125 章 |
| `route_changhua_yunlin.yaml` | 彰化 | 雲林（斗六） | 第 115 章 |
| `route_taichung_miaoli.yaml` | 台中 | 苗栗 | 第 174 章 |
| `route_miaoli_taipei.yaml` | 苗栗 | 台北（大稻埕） | 第 174 章 |
| `route_yunlin_chiayi.yaml` | 雲林 | 嘉義市 | 第 114 章 |

> **注意**：各路線需包含 `era_active` 條件，部分路線在特定時代才開通（如鐵路路線僅 v1.5+）

---

### P23.4 — 新 NPC 建立

以下 NPC 需建立 YAML 設定並設計對話與日程：

| NPC ID | 姓名 | 所在地 | 功能 |
|---|---|---|---|
| `npc_zhu_yigui` | 朱一貴 | gangshan → tainan | Incident EP001 主線 NPC |
| `npc_du_junying` | 杜君英 | gangshan | 客家視角，義兵裂痕 |
| `npc_zheng_zai` | 鄭仔（農夫鄰居）| gangshan_duck_pond | 「鴨母王的日常」觸發 |
| `npc_lin_shuangwen` | 林爽文 | changhua_city | Incident EP002 主線 NPC |
| `npc_zhuang_datian` | 莊大田 | tainan/fengshan | 南路義軍視角 |
| `npc_dai_chaochun` | 戴潮春 | changhua_city | Incident EP003 主線 NPC |
| `npc_arugus` | 阿祿古斯 | mudan_paiwan_village | 排灣族視角，牡丹社 NPC |
| `npc_jp_interpreter` | 日軍翻譯官（田中） | mudan_japanese_landing | 日方視角 |
| `npc_shen_baozhen` | 沈葆楨 | checheng_harbor | 清廷欽差 |
| `npc_liu_mingchuan` | 劉銘傳 | taipei → miaoli | 建省巡撫，近代化路線 NPC |
| `npc_lao_zhou` | 苦力老周 | miaoli_old_railway | 底層視角，建省苦工 |
| `npc_lin_chaodong` | 林朝棟（霧峰林家）| wufeng_lin_mansion | 林爽文事件後霧峰林家發跡 |
| `npc_chen_zai` | 漳州農民陳仔 | yunlin_irrigation_field | 漳泉械鬥漳州視角 |
| `npc_a_shui` | 泉州商人阿水 | yunlin_douliu_street | 漳泉械鬥泉州視角 |
| `npc_hakka_old_woman` | 客家老婦 | yunlin_hakka_village | 「轉移族譜」任務 NPC |

---

### P23.5 — 記憶碎片 YAML 建立

**EP001 朱一貴（共 5 個）**
- [x] `memories/gangshan/zhu_yigui_daily.yaml`（鴨母王的日常）
- [x] `memories/tainan/zhu_yigui_short_spring.yaml`（義軍的短暫春天）
- [x] `memories/gangshan/du_junying_hakka_rift.yaml`（客家離心的裂痕）
- [x] `memories/tainan/magistrate_document.yaml`（知縣的公文）
- [x] `memories/tainan/tainan_city_joys_and_sorrows.yaml`（府城內的悲歡）

**EP002 林爽文（共 5 個）**
- [x] `memories/changhua/lin_shuangwen_spread.yaml`（失控的版圖）
- [x] `memories/tainan/zhuang_datian_south_route.yaml`（莊大田的南路）
- [x] `memories/changhua/yi_min_opposition.yaml`（對立的同鄉）
- [x] `memories/changhua/changhua_city_fear.yaml`（彰化縣城的恐懼）
- [x] `memories/changhua/oxcart_and_bones.yaml`（牛車與骸骨・義民廟起源）

**EP003 戴潮春（共 4 個）**
- [x] `memories/changhua/dai_chaochun_four_years.yaml`（四年的圍城）
- [x] `memories/changhua/clan_game.yaml`（宗族的博弈）
- [x] `memories/changhua/city_changes_hands.yaml`（易手的城池）
- [x] `memories/changhua/yearn_for_peace.yaml`（平靜的渴望）

**EP004 牡丹社事件（共 5 個）**
- [x] `memories/mudan/arugus_rules.yaml`（阿祿古斯的規矩）
- [x] `memories/mudan/japanese_footprints.yaml`（日軍的足跡）
- [x] `memories/mudan/interpreter_diary.yaml`（翻譯官的日記）
- [x] `memories/mudan/shen_baozhen_memorial.yaml`（欽差的考量）
- [x] `memories/mudan/empire_probe.yaml`（帝國擴張的試探・全局）

**EP005 台灣建省（共 4 個）**
- [x] `memories/miaoli/railway_laborers_sweat.yaml`（苦力的汗水）
- [x] `memories/taichung/liu_mingchuan_reform.yaml`（近代化的腳步）
- [x] `memories/taichung/aboriginal_cost.yaml`（開山撫番的代價）
- [x] `memories/taipei/province_glory_and_fall.yaml`（建省十年的榮光與落寞）

---

### P23.6 — 職涯玩法觸發點整合

第二卷強化四種行當，需在新 Sites 上設計觸發點：

| 行當 | 觸發地點 | 觸發條件 | 獎勵 |
|---|---|---|---|
| **廟埕開講**（說書人）| `changhua_nanputuo_temple` | 已完成至少 3 個 EP001/EP002 記憶碎片 | 彰化聚落凝聚值 +10 |
| **口述採集者** | `mudan_paiwan_village`, `yunlin_hakka_village` | 好感度：熟識以上 | 觸發特定記憶碎片解鎖 |
| **古蹟修復師** | `changhua_confucian_temple`, `gangshan_fort` | 匠人職涯 lv≥2 | 失源者威脅降低、記憶值提升 |
| **走街商人**（商人）| `yunlin_douliu_street`, `miaoli_old_railway` 沿線 | 商人職涯 lv≥1 | 建立商路踏印，解鎖 NPC 好感度快速提升 |

---

### P23.7 — 失源者場景設定

- [x] `changhua_county_fort`：民變記憶淡出，設定「亂世失源者」（`memory` 低於閾值觸發）
- [x] `yunlin_hakka_village`：械鬥後族譜燒毀記憶，設定「宗族失源者」
- [x] `mudan_paiwan_village`：原住民文化被壓縮，設定「祖靈失源者」
- [x] `miaoli_old_railway`：苦力客死異鄉無名墓，設定「無名失源者」

---

### P23.8 — 驗收測試

- [x] `tests/test_vol2_routes.c`：驗證所有新路線可正常旅行（含 era_active 過濾）
- [x] `tests/test_vol2_incidents.c`：驗證五大 Incident 的進度推進與完成觸發
- [x] `tests/test_vol2_memories.c`：驗證新記憶碎片能正確觸發（含前置條件）
- [x] `tests/test_vol2_npcs.c`：驗證新 NPC 的好感度系統與日程行動

---

## 🗺️ 地理網絡示意圖（完成後）

```
[台北大稻埕]  ←── 茶葉貿易、劉銘傳時代
     ↑
[苗栗] ←── 劉銘傳鐵路路線 ──→ [基隆]
     ↑
[台中・霧峰林家]
     ↑
[彰化市] ⭐（林爽文、戴潮春主戰場）
   ↑     ↑
[鹿港]  [雲林斗六]  ←── 漳泉械鬥
（卷一）   ↑
         [嘉義市]（卷一終點 / 卷二出發點）
             ↓
         [台南・府城]（朱一貴府城自治）
             ↓
         [高雄岡山]  ← 朱一貴故地、鴨母王養鴨地
             ↓
         [屏東恆春]
             ↓
         [屏東牡丹] ⭐（牡丹社事件・日本第一次踏台）
```

---

## 📊 工作量估計

| 子任務 | YAML 檔案數 | LPC 工作量 | 複雜度 |
|---|---|---|---|
| P23.1 聚落補完 | ~25 個 settlement/site YAML | 低（純資料）| ★★☆ |
| P23.2 Incident 系統 | 5 個 incident YAML | 中（引擎若需新欄位）| ★★★ |
| P23.3 路線補全 | 8 條路線 YAML | 低（純資料）| ★☆☆ |
| P23.4 NPC 建立 | 15 個 NPC YAML | 低（純資料）| ★★☆ |
| P23.5 記憶碎片 | 23 個 Memory YAML | 低（純資料）| ★★☆ |
| P23.6 職涯觸發 | Site YAML 修改 + 少量 LPC | 中 | ★★☆ |
| P23.7 失源者 | Settlement/Site YAML 修改 | 低 | ★☆☆ |
| P23.8 測試 | 4 個 .c 測試檔 | 高 | ★★★ |

**總計**：約 76 個 YAML 檔案，4 個測試檔，部分 LPC 修改

---

## 🔗 依賴關係與前置確認

P23 以純內容（YAML 資料）開發為主，底層引擎均已就緒：

- [x] `incident_d.c` — Incident 進度追蹤（P16 已完成）
- [x] `evidence_d.c` / `memory_d.c` — 多視角記憶碎片觸發（P17 已完成）
- [x] `site_completion_d.c` — 地標探索度（P18 已完成）
- [x] `npc_d.c` + 好感度系統 — NPC 關係網絡（P20 已完成）
- [x] `route_d.c` + `era_active` — 時代路線過濾（P8.1 已完成）
- [x] `world_state_d.c` — 世界狀態演化（P21 已完成）

> **結論**：引擎層已全部就緒，P23 無需修改底層架構，全力投入內容建置。

---

## 📝 相關文件

- [第2卷章節綱要](file:///home/wade/src/github/FormosaSaga/chapter/源流福爾摩沙_第2卷_章節綱要.md)
- [歷史事件資料庫](file:///home/wade/src/github/FormosaSaga/docs/canon/historical_events.md)
- [地名清單](file:///home/wade/src/github/FormosaSaga/docs/sites.md)
- [TODO 主文件](file:///home/wade/src/github/FormosaSaga/docs/TODO.md)
- [地理擴展規劃（卷一）](file:///home/wade/src/github/FormosaSaga/docs/Geographical_Expansion_Plan.md)
