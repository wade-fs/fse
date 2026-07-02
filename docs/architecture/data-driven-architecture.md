# FSE 2.0 資料驅動架構與虛擬物件編譯規範 (FSE Data-Driven & Virtualization Specification)

Flow State Engine (FSE) 2.0 採用完全的「資料驅動 (Data-Driven)」架構，所有場景節點、存在體、挑戰與物品均以 YAML 定義，並透過 LPC 虛擬物件 (Virtual Object) 機制動態實體化。

---

## 一、 虛擬編譯與對稱注入機制 (Virtual Object Compilation)

為了在 LPC 中將一個虛擬路徑（例如 `/nodes/town_center/node`）動態轉換為一個實體物件，FSE 使用了對稱注入機制：

```
1. load_object("/nodes/town_center/node") 觸發
   │
   ▼
2. compile_object() 攔截，呼叫 virtual.c
   │
   ▼
3. clone_object("/std/node.c") 實體化
   │
   ▼
4. ob->setup_virtual_by_path("/nodes/town_center/node") 注入
   │
   ▼
5. 讀取 /content/nodes/town_center/node.yaml 載入配置
```

### 1. 虛擬路徑對齊與 YAML 解析
由 `setup_virtual_by_path(string file_path, string prefix, string config_file)` 實現：
* 將虛擬路徑（如 `/nodes/town_center/node.c`）拆解，定位出上一層目錄 ID（即 `"town_center"`）。
* 計算出實體 YAML 存放路徑（即 `/content/nodes/town_center/node.yaml`）。
* 解析 YAML 並賦予實體 `virtual_config`。

### 2. 終極防禦性屬性對齊 (Metadata Overriding)
為了避免菱形繼承造成的 `private` 變數隔離，所有繼承 `virtual_object` 的實體均透過動態覆寫方法獲取屬性，保證 100% 資料驅動：
```c
// 覆寫 query_short()
string query_short() {
    mapping config = query_virtual_config();
    if (config && config["name"]) return config["name"];
    return short_desc; // 預設 Fallback
}

// 覆寫 query_entity_id()
string query_entity_id() {
    mapping config = query_virtual_config();
    if (config && config["node_id"]) return config["node_id"];
    return ::query_entity_id();
}
```

---

## 二、 認識論挑戰與雙重證據鏈 (Challenge & Evidence Chain)

一個 Challenge YAML 宣告了多個 realities 分支，並通過 `evaluate` 執行「證據鏈（Required Observations）」的嚴格校驗。

### 1. 雙重證據鏈定義示例 (`/content/nodes/town_center/challenges/town_center_first_contact.yaml`)
```yaml
challenge_id: "town_center_first_contact"
executor: "reality_resolver"

# 宣告並行分支
realities:
  social:
    knowledges:
      - "social.commerce.fair_exchange"

discover_factor: "commerce_master"
success_msg: "🎉 你看穿了商賈眼中的貪欲，並對齊了市井的真實行情！\n"
failure_warning: "【 🌀 交易衝突 】你對紅塵俗事一無所知，強行砍價引起了集市商販的反感！\n"

# Evolve 因果副作用
evolve:
  social:
    understanding:
      world_change: "transaction_completed_with_trust"
      new_signals: ["market_trust_established"]
      adventure_effects:
        spiritual_energy: 10
        karma_change: -10 # 了結塵緣，消除 10 點業力
    misconception:
      world_change: "trade_rejected"
      new_signals: ["humiliated"]
      adventure_effects:
        spiritual_energy: -5
        karma_change: 10 # 執迷紅塵，增加 10 點業力
```

### 2. 客觀規律 Law 定義 (`/runtime/realities/social/commerce.yaml`)
定義了 Knowledge `fair_exchange` 的三態判定與 Aligned Action：
```yaml
law: commerce
knowledges:
  fair_exchange:
    description: "等價物換之理：參透市井行情與凡人貪欲，方能完成無漏交易。"
    evaluate:
      required_observations:
        - "market_price"    # 證據 A：市井行情
        - "trader_greed"    # 證據 B：商賈貪念
      aligned_action: "trade_fair"
      misunderstanding_patterns:
        - has: ["market_price"]
          missing: ["trader_greed"]
          yields: "blind_bargaining"
      default_misconception: "haggle_by_force"
```

---

## 三、 行動分發服務 (ActionExecutor Specification)

當玩家執行一個交互時，`fse_room.c` 會調用全域 `ActionExecutor` 服務進行分發。這徹底避免了在互動層硬寫死成敗。

```
1. 玩家輸入 `trade_fair trader`
   │
   ▼
2. fse_room.c 攔截，調用 ActionExecutor->dispatch_action()
   │
   ▼
3. 根據 action ("trade_fair") 找到對應的專屬 Resolver (reality_resolver.c)
   │
   ▼
4. 解析該節點的挑戰 YAML 路徑:
   "/content/nodes/" + query_entity_id() + "/challenges/" + res_challenge + ".yaml"
   │
   ▼
5. 載入評估，執行 side effects，回傳 1 攔截傳統物理交互
```
