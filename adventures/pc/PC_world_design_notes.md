# 《史前文明》(PC) — FSE 完整規劃案

> 本文件依循 FSE 設計哲學（Discovery-First Progression）與 `adventures/pw` 的架構慣例撰寫。

---

## 一、世界核心理念

### 1.1 世界的教學本質
在 FSE 中，**World 本身就是老師**。史前文明世界的「老師」是：

- **大自然的物理法則**（熵增、氣壓、燃燒三要素、槓桿）
- **生態的殘酷規律**（捕食者感知、獵物行為模式、食物腐敗）
- **人類群體的集體記憶**（符號、儀式、傳承的誕生）

玩家從一無所有的智人開始，**不是透過說明書學知識，而是透過痛苦的代價逼出理解**。

### 1.2 探索循環（Discovery Loop）
```
World（三疊紀荒原）
↓ 提供感官訊號：氣味、震動、溫度、風向
Actor 採取行動（rub branches / focus ground / smell wind）
↓
Consequence（失溫 / 被捕食者鎖定 / 食物腐爛）
↓
Confusion（「為什麼下雨火就熄了？」「為什麼我跑兩步就喘？」）
↓
Discovery（「乾燥介質是燃燒的必要條件」「大氣氧氣濃度 15%」）
↓
New Path（解鎖「營火」→ 照亮黑暗邊界 → 進入下一區域）
↓
回到 World（新感知能力 → 新探索空間）
```

---

## 二、地質時代三幕結構

PC 世界橫跨三個地質時代，每個時代即一個完整的 **Stage 族群**，對應不同的生存挑戰層次：

| 時代 | 年代 | 氧氣濃度 | 主要捕食者 | 主題 |
|------|------|----------|-----------|------|
| 三疊紀 | ~2.5億年前 | 15%（低） | Herrerasaurus、大型蜻蜓 | 蠻荒求生：感知 + 火 |
| 侏羅紀 | ~2億年前 | 19%（接近現代） | 蠻龍（Torvosaurus）、Diplodocus | 工具與馴化：石器 + 生態鏈 |
| 白堊紀 | ~1.4億年前 | 30%（高，火災頻繁） | 暴龍（T-Rex）、Spinosaurus | 聚落與文字：保存 + 傳承 |

> 玩家的 Stage 推進透過 **Factor 的累積** 驅動，而非直接的時代切換。

---

## 三、Stages（進度階段）完整規劃

### Stage 1：`novice` — 初來乍到（三疊紀）

**情境**：玩家甦醒在三疊紀荒原，徒手、無知。大雨將至，氣溫驟降。

**核心困惑**：
- 「為什麼我跑兩步就喘？」
- 「為什麼遠處震動我就必須趴下？」
- 「為什麼下雨火就熄了？」

**需要完成的 Quest**：
- `first_fire`（鑽木取火）

**推進至**：`survivor`

---

### Stage 2：`survivor` — 初級生還者（三疊紀後期）

**情境**：玩家已能生火，開始嘗試主動狩獵與探索黑暗邊境。

**核心困惑**：
- 「石頭為什麼砸不碎骨頭？」
- 「怎麼讓小動物靠近而不逃跑？」
- 「那隻始祖巨獸主要靠嗅覺還是視覺？」

**需要完成的 Quest**：
- `first_hunt`（第一次狩獵）
- `wind_stealth`（利用風向隱蔽）

**推進至**：`tool_maker`

---

### Stage 3：`tool_maker` — 工具製造者（侏羅紀前期）

**情境**：玩家進入侏羅紀，生態更複雜，必須以工具超越純粹的體能。

**核心困惑**：
- 「槓桿怎麼用？」
- 「什麼樣的石頭最適合敲砸？」
- 「動物的遷徙有規律嗎？」

**需要完成的 Quest**：
- `stone_tool_craft`（打製石器）
- `migration_pattern`（觀察動物遷徙）

**推進至**：`domesticator`

---

### Stage 4：`domesticator` — 馴化者（侏羅紀後期）

**情境**：玩家開始不再只是獵殺，而是嘗試與生態共生。

**核心困惑**：
- 「翼龍的哨兵能力怎麼借用？」
- 「怎麼讓動物不逃跑？」

**需要完成的 Quest**：
- `pterosaur_bond`（與翼龍建立信任）
- `first_trap`（設置陷阱）

**推進至**：`settler`

---

### Stage 5：`settler` — 聚落者（白堊紀前期）

**情境**：玩家開始定居，但食物保存與知識傳承成為新的瓶頸。

**核心困惑**：
- 「為什麼食物會腐爛？」
- 「鹽分是從哪裡來的？」
- 「怎麼讓火在大氣含氧量高的白堊紀不引發失控火災？」

**需要完成的 Quest**：
- `food_preservation`（食物保存）
- `pottery_craft`（製作陶器）

**推進至**：`chronicler`

---

### Stage 6：`chronicler` — 文明記錄者（白堊紀後期）

**情境**：玩家達到史前文明的巔峰——知識可以跨世代傳遞。

**核心困惑**：
- 「如何讓後代知道危險在哪裡？」
- 「符號能代表規律嗎？」
- 「記憶如何對抗遺忘的 Entropy？」

**需要完成的 Quest**：
- `cave_painting`（壁畫繪製）
- `ritual_altar`（祭壇儀式）

**推進至**：`（結局 / 開放延伸）`

---

## 四、Factors（知識因子）完整規劃

### Category: `survival`（生存感知）

| factor_id | 名稱 | 觸發條件 | 描述 |
|-----------|------|---------|------|
| `predator_scent` | 捕食者氣味解讀 | 在荒原中被攻擊後 focus smell | 從腥臭風中辨識捕食者威脅等級與距離 |
| `thermodynamics` | 摩擦生熱與引火 | 完成 first_fire 閉環 | 乾燥介質 + 劇烈摩擦 = 熱能引燃 |
| `oxygen_scarcity` | 低氧呼吸節律 | 三疊紀中奔跑後喘不過氣 | 三疊紀大氣氧濃度 15%，過度耗氧會窒息 |
| `vibration_reading` | 地震波解讀 | focus ground 時觀察大型生物腳步 | 地表震動頻率與強度對應捕食者體型與距離 |
| `wind_direction` | 風向掌控 | 被嗅覺型捕食者發現後 | 獵手的氣味會被風帶走，與風向同行等於自殺 |

### Category: `tools`（工具製造）

| factor_id | 名稱 | 觸發條件 | 描述 |
|-----------|------|---------|------|
| `lever_principle` | 槓桿原理 | 搬不動大石頭後嘗試分叉樹枝 | 支點 + 施力 = 倍增力量，改變工具設計 |
| `hardness_index` | 硬度差異 | 用軟石砸骨頭失敗後 | 莫氏硬度高的材質才能切削低硬度材質 |
| `flint_knapping` | 燧石打製 | 發現黑曜石後嘗試敲擊 | 特定角度施力使石材貝殼狀斷裂形成刃口 |
| `fiber_binding` | 纖維綑綁 | 工具頭脫落後觀察蕨類莖部 | 植物纖維扭轉後具有張力，可固定複合工具 |

### Category: `ecology`（生態規律）

| factor_id | 名稱 | 觸發條件 | 描述 |
|-----------|------|---------|------|
| `prey_behavior` | 獵物行為模式 | 多次跟蹤同一物種後 | 不同物種的逃跑模式、休息時間都有規律性 |
| `migration_cycle` | 遷徙週期 | 連續觀察 3 次動物足跡方向 | 動物遷徙受季節、水源、捕食者遷移影響 |
| `predator_territory` | 捕食者領域 | 進入特定區域被追逐後 | 大型捕食者有固定的覓食領域與標記範圍 |
| `symbiosis` | 共生關係 | 觀察小鳥停在大型草食恐龍身上 | 某些生物種間存在互利共生可以被模擬利用 |

### Category: `civilization`（文明原語）

| factor_id | 名稱 | 觸發條件 | 描述 |
|-----------|------|---------|------|
| `salt_preservation` | 鹽分保存 | 食物在海岸附近不腐爛後 | 高濃度鹽分抑制微生物繁殖，可保存食物 |
| `fermentation` | 發酵現象 | 果實在密閉陶罐中變質後 | 酵母菌在無氧環境下分解糖分產生酒精 |
| `symbol_abstraction` | 符號抽象化 | 嘗試在石壁上畫出捕食者形狀 | 固定符號可以代指真實世界的物件與規律 |
| `fire_control` | 火的控制 | 在白堊紀高氧環境中引燃後失控 | 大氣含氧 30% 時燃燒速度激增，需控制氧氣供給 |

---

## 五、Rooms（空間）完整規劃

### Stage 1-2 區域：三疊紀

```
triassic_plains（三疊紀荒原）[起始點]
  └── [reveal: 解鎖 thermodynamics 後] → fern_forest（巨型蕨類森林）
        └── [reveal: 解鎖 wind_direction 後] → predator_canyon（捕食者峽谷）
              └── [reveal: 解鎖 vibration_reading 後] → dark_cave（黑暗洞穴）[Stage 2 spawn]
```

#### `triassic_plains`（三疊紀荒原）【已存在】
- **描述**：橘紅天空的硫磺荒原，紅棕砂岩，始祖小雞出沒
- **感官訊號**：smell（腥臭）/ sound（地鳴）/ wind（東南風）/ ground（三趾腳印）
- **Reveal**：黑暗的森林邊界，火光解鎖後才可進入
- **Monsters**：proto_chicken（始祖小雞）

#### `fern_forest`（巨型蕨類森林）【已存在】
- **描述**：高大蕨類，濕潤腐植土，黑曜石散落
- **感官訊號**：smell（腐植土）/ sound（葉片摩擦）/ wind（微弱）/ ground（鬆軟落葉）
- **Exits**：back → triassic_plains

#### `predator_canyon`（捕食者峽谷）【待建】
- **描述**：深紅砂岩峽谷，地面刻滿大型捕食者爪痕，空氣稀薄
- **感官訊號**：smell（血腥 + 尿液領域標記）/ sound（低吼迴響）/ ground（深陷爪印 + 骨骸碎片）
- **Monsters**：herrerasaurus（奔跑捕食者）
- **Reveal**：破解風向後，可沿峭壁小路穿越

#### `dark_cave`（黑暗洞穴）【待建】
- **描述**：天然石灰岩洞，火把是唯一光源，地面有前人壁畫雛形
- **感官訊號**：smell（石灰 + 舊灰燼）/ sound（水滴聲）/ ground（石英砂礫）
- **Stage 2 spawn point**

### Stage 3-4 區域：侏羅紀

```
jurassic_valley（侏羅紀河谷）[Stage 3 spawn]
  └── [reveal: 解鎖 flint_knapping 後] → obsidian_quarry（黑曜石採石場）
  └── [reveal: 解鎖 migration_cycle 後] → migration_trail（遷徙走廊）
        └── pterosaur_cliff（翼龍峭壁）[Stage 4 spawn]
```

#### `jurassic_valley`（侏羅紀河谷）【待建】
- **描述**：廣闊的蕨類河谷，Diplodocus 在遠處緩慢移動，植被更高更茂密
- **感官訊號**：smell（青草 + 大型草食恐龍糞便）/ sound（河流 + 遠方巨獸步伐）/ ground（植物根系形成天然網格）

#### `obsidian_quarry`（黑曜石採石場）【待建】
- **描述**：火山噴發形成的黑曜石礦脈，地面高溫，玩家可以打製石器
- **Challenge**：stone_tool_craft（選擇正確角度、材料打製刃口）

#### `migration_trail`（遷徙走廊）【待建】
- **描述**：動物遷徙踩出的古老路徑，每隔一段距離有不同足跡
- **Challenge**：migration_pattern（連續觀察 3 次足跡方向分析規律）

#### `pterosaur_cliff`（翼龍峭壁）【待建】
- **描述**：高聳的砂岩峭壁，翼龍在崖邊築巢
- **Challenge**：pterosaur_bond（以食物和聲音訊號建立翼龍信任）

### Stage 5-6 區域：白堊紀

```
cretaceous_shore（白堊紀海岸）[Stage 5 spawn]
  └── [reveal: 解鎖 salt_preservation 後] → salt_flats（鹽灘）
  └── [reveal: 解鎖 pottery_craft 後] → settlement_ridge（聚落山脊）[Stage 6 spawn]
        └── [reveal: 解鎖 symbol_abstraction 後] → ancestor_cave（祖先洞穴）
```

#### `cretaceous_shore`（白堊紀海岸）【待建】
- **描述**：白堊紀海岸，大氣含氧量 30%，植被茂盛但極易起火
- **感官訊號**：smell（海鹽 + 焦木）/ sound（浪濤）/ ground（細白砂）
- **特殊機制**：高氧環境下火炬生命週期縮短，需要更謹慎控制火源

#### `salt_flats`（鹽灘）【待建】
- **描述**：海水蒸發形成的天然鹽灘，食物在此不腐爛
- **Discovery 觸發點**：salt_preservation

#### `settlement_ridge`（聚落山脊）【待建】
- **描述**：玩家建立的第一個人類聚落，有陶器、火堆、簡單的獸皮遮蔽
- **Challenge**：food_preservation + pottery_craft

#### `ancestor_cave`（祖先洞穴）【待建】
- **描述**：岩壁上密佈壁畫，從粗糙輪廓到精確符號的演進清晰可見
- **Challenge**：cave_painting + ritual_altar（終局）

---

## 六、Monsters（生物）完整規劃

| monster_id | 名稱 | 時代 | HP | ATK | 特殊行為 | 設計意圖 |
|-----------|------|------|-----|-----|---------|---------|
| `proto_chicken` | 始祖小雞 | 三疊紀 | 15 | 4 | respawn | 第一次戰鬥，習得戰鬥基礎 |
| `herrerasaurus` | 奔跑者 | 三疊紀 | 45 | 12 | 追蹤嗅覺 | 觸發 wind_direction Discovery |
| `ramphorhynchus` | 鑲嵌翼龍 | 侏羅紀 | 20 | 8 | 可馴化 | 觸發 pterosaur_bond Discovery |
| `torvosaurus` | 蠻龍 | 侏羅紀 | 120 | 28 | 領域型 | 觸發 predator_territory Discovery |
| `pteranodon` | 無齒翼龍 | 白堊紀 | 30 | 10 | 可作為哨兵 | Stage 4 馴化成就 |
| `tyrannosaurus` | 暴龍 | 白堊紀 | 300 | 60 | 嗅覺強、視覺弱 | 測試玩家對所有感官知識的綜合應用 |

---

## 七、Commands（指令）設計

PC 使用感官導向的指令系統，與 pw 的 blockly 積木指令不同：

| 指令 | 說明 | 觸發的 Discovery |
|------|------|-----------------|
| `look` | 視覺觀察當前空間 | — |
| `focus smell` | 專注嗅覺感知 | predator_scent、salt_preservation |
| `focus sound` | 專注聽覺感知 | vibration_reading |
| `focus ground` | 專注觸地感知 | vibration_reading、migration_cycle |
| `focus wind` | 感知風向與氣息 | wind_direction |
| `rub <A> <B>` | 摩擦兩個物件 | thermodynamics |
| `strike <A> <B>` | 用物件敲擊另一物件 | flint_knapping、hardness_index |
| `pry <A> with <B>` | 用槓桿撬動 | lever_principle |
| `follow tracks` | 跟蹤足跡 | prey_behavior、migration_cycle |
| `hide downwind` | 順風向隱蔽 | wind_direction |
| `draw <symbol>` | 在石壁或地面畫符號 | symbol_abstraction |
| `observe <target>` | 長時間觀察特定目標 | symbiosis、prey_behavior |

---

## 八、Locale（本地化）規劃

`/content/locales/zh_TW.yaml` 需擴充以下條目：

```yaml
# 感官系統
pc.sensory.smell_result: "你深吸一口氣，感知到：{result}"
pc.sensory.sound_result: "你閉上眼睛傾聽，感知到：{result}"
pc.sensory.ground_result: "你俯身貼地，感知到：{result}"
pc.sensory.wind_result: "你感受著風的方向與氣息：{result}"

# 失敗後果
pc.failure.oxygen_deplete: "$RED$你劇烈喘息，氧氣不足，視野開始模糊... 在三疊紀，奔跑是一種奢侈。$NOR$\n"
pc.failure.scent_detected: "$RED$捕食者突然轉頭！牠的鼻孔在抽動——牠聞到了你。$NOR$\n"
pc.failure.fire_wet: "樹枝太潮濕，摩擦產生的熱量全被水分帶走了。\n"
pc.failure.fire_rain: "暴雨澆熄了你的火星。你需要找到更乾燥的引火物。\n"
pc.failure.tool_wrong_material: "石頭彼此碰撞，留下白色粉末——硬度不夠，無法切削。\n"

# Discovery 觸發文字
pc.discovery.thermodynamics: "\n$HIC$【 🔥 領悟：摩擦生熱 】$NOR$\n乾燥的蕨類根部在快速摩擦下冒出橘紅色的火星。你第一次在這個世界點燃了火。\n你理解了：熱量是機械能轉化的結果，而乾燥是燃燒的必要條件。\n"
pc.discovery.wind_direction: "\n$HIC$【 🌬️ 領悟：風向掌控 】$NOR$\n暴龍在你靠近時猛然轉頭——牠是靠鼻子，不是眼睛。\n你理解了：順著風走等於把自己的氣味送進捕食者的鼻腔。\n"
pc.discovery.lever_principle: "\n$HIC$【 ⚖️ 領悟：槓桿原理 】$NOR$\n一根叉狀樹枝抵著石塊，你輕輕一壓——巨石翻動了。\n支點改變了力的方向與大小。\n"
pc.discovery.symbol_abstraction: "\n$HIC$【 ✍️ 領悟：符號誕生 】$NOR$\n你在石壁上畫下了一個代表「危險」的標記。後代或許再也不用親身試錯。\n這是知識脫離個體、成為文明的那一刻。\n"

# 進度推進
pc.progress.stage_shifted: "\n$HIW$【文明階段躍升】$NOR$\n你從「{prev}」成長為：$HIG${name}$NOR$\n\n"
```

---

## 九、第一個可執行閉環（MVP）

> 對應現有的 `novice` stage 與 `first_fire` quest，需要完成以下檔案建置：

### 閉環流程
```
玩家進入 triassic_plains
  → look → 看到橘紅天空、恐龍腳印、遠方蕨類森林（黑暗邊緣被遮蔽）
  → 嘗試 rub branches → 失敗：「樹枝太潮濕」
  → 嘗試跑步移動 → 失敗提示：「你喘不過氣」（觸發 oxygen_scarcity confusion）
  → focus ground → 發現「乾枯蕨類根部」
  → rub fern roots → 成功引燃！
  → 解鎖 thermodynamics factor
  → 解鎖 first_fire quest 完成
  → 蕨類森林邊界 Reveal（可進入 fern_forest）
  → novice → survivor stage 推進
```

### 需要新建的檔案

| 檔案路徑 | 類型 | 說明 |
|---------|------|------|
| `content/rooms/triassic_plains/challenges/first_fire.yaml` | Challenge | 鑽木取火挑戰 |
| `content/rooms/triassic_plains/discoveries/thermodynamics_trigger.yaml` | Discovery | 解鎖熱力學領悟 |
| `content/locales/zh_TW.yaml` | Locale | 感官 + Discovery 文字 |
| `content/factors/oxygen_scarcity.yaml` | Factor | 低氧呼吸節律 |
| `content/factors/wind_direction.yaml` | Factor | 風向掌控 |
| `content/factors/vibration_reading.yaml` | Factor | 地震波解讀 |

---

## 十、架構差異：PC vs PW

| 面向 | PW（Programming World） | PC（Prehistoric Civilization） |
|------|------------------------|-------------------------------|
| 核心服務 | `blockly_service`（積木 AST） | 感官指令系統（文字 MUD） |
| Challenge 類型 | `ast_matcher`（積木匹配） | `action_sequence`（指令序列） |
| Node 結構 | `/content/nodes/<name>/` | `/content/rooms/<name>/` |
| Discovery 觸發 | 執行正確積木 | 執行正確感官行動 + 失敗歷史 |
| Factor 前置 | 積木工具箱前置 | 感官能力前置 |
| Reveal 機制 | 工具箱新積木出現 | 空間黑暗邊界被照亮 |
| Locale 主題 | 程式語言（迴圈、記憶體） | 感官、自然現象、文明原語 |

---

## 十一、開發優先序

| 優先級 | 項目 | 說明 |
|--------|------|------|
| P0 | `first_fire` 閉環 | MVP：完整的 novice → survivor 體驗 |
| P0 | `zh_TW.yaml` locale | 所有感官 + Discovery 文字 |
| P1 | `first_hunt` + `wind_stealth` | survivor stage 完整化 |
| P1 | `predator_canyon` + `dark_cave` rooms | 三疊紀地圖完整化 |
| P1 | `herrerasaurus` monster | wind_direction Discovery 觸發體 |
| P2 | Stage 3-4（侏羅紀）全部 | 工具與馴化 |
| P3 | Stage 5-6（白堊紀）全部 | 聚落與文字 |
| P4 | 感官指令系統服務擴充 | 若 runtime 尚未支援 |
