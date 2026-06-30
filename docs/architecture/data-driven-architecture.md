# FSE 資料驅動架構演進規劃 (FSE Data-Driven Architecture Plan)

為了讓 FSE (Flow Space Engine) 真正成為一個通用、中立且高度擴展的 MUD/Web 遊戲引擎，我們規劃將所有冒險 (Adventures) 徹底轉向 **「YAML 定義 + Runtime 核心服務」** 的資料驅動架構。

本規劃書旨在定義後續的核心服務 API 規範以及 LPC 虛擬物件 (Virtual Object) 的編譯映射機制。

---

## 🗺️ 系統架構全景 (Architecture Overview)

```mermaid
graph TD
    %% Base Layer
    subgraph Data Layer (YAML Files)
        M_YAML[content/monsters/*.yaml]
        R_YAML[content/rooms/*.yaml]
        Q_YAML[content/quests/*.yaml]
        D_YAML[content/dialogues/*.yaml]
    end

    %% Virtualization Layer
    subgraph LPC Virtual Compilation
        VO[master.c :: compile_object]
        V_MONSTER[std/virtual/monster.c]
        V_ROOM[std/virtual/room.c]
    end

    %% Core Services
    subgraph FSE Runtime Services
        CS[runtime/services/combat_service.c]
        QS[runtime/services/quest_service.c]
        IS[runtime/services/inventory_service.c]
        DS[runtime/services/dialogue_service.c]
    end

    %% Mapping
    M_YAML -->|yaml_decode| VO
    R_YAML -->|yaml_decode| VO
    VO -->|Generate Instance| V_MONSTER
    VO -->|Generate Instance| V_ROOM

    V_MONSTER -->|Resolve Actions| CS
    V_ROOM -->|Resolve Items| IS
    V_ROOM -->|Trigger Talk| DS
    QS -->|Complete Triggers| QS
```

---

## 🛠️ 核心服務規格定義 (Core Services Specification)

### 1. 戰鬥服務 (`combat_service.c`)
負責處理非同步、基於回合的戰鬥計算。不再將戰鬥公式硬編碼在 `living.c` 或怪物物件中。

* **核心 API**：
  * `void start_combat(object attacker, object defender)`
  * `void stop_combat(object actor)`
  * `int calculate_damage(object attacker, object defender, mapping combat_data)`
  * `void process_combat_round(object actor)`
* **資料驅動定義示例 (`/content/monsters/proto_chicken.yaml`)**：
  ```yaml
  id: "proto_chicken"
  name: "始祖小雞"
  hp: 15
  stats:
    attack: 4
    defense: 1
    speed: 10              # 用於決定行動順序
  combat:
    type: "melee"
    interval: 2           # 每2秒一回合
    skills:
      - id: "peck"
        chance: 30
        damage_multiplier: 1.2
  ```

---

### 2. 任務服務 (`quest_service.c`)
管理複雜的非線性任務線、前置條件、玩家任務狀態（未接取、進行中、已完成、已過期）與獎勵發放。

* **核心 API**：
  * `int accept_quest(object player, string quest_id)`
  * `int check_quest_eligibility(object player, string quest_id)`
  * `int update_quest_progress(object player, string quest_id, string objective_id, int value)`
  * `void claim_rewards(object player, string quest_id)`
* **資料驅動定義示例 (`/content/quests/first_kill.yaml`)**：
  ```yaml
  id: "first_kill"
  name: "第一滴血"
  prerequisites:
    level: 0
  objectives:
    - id: "kill_chicken"
      type: "kill_monster"
      target_id: "proto_chicken"
      required_count: 1
  rewards:
    exp: 20
    factors:
      - "combat_survival"
  ```

---

### 3. 背包與道具服務 (`inventory_service.c`)
提供通用的道具承載、負重、容器嵌套以及屬性隨機化（例如裝備副屬性）的管理。

* **核心 API**：
  * `int move_item(object item, object from, object to)`
  * `int check_weight_limit(object container, object item)`
  * `mapping query_inventory(object container)`
  * `object create_item_from_yaml(string item_yaml_path)`

---

### 4. 對話與 NPC 互動服務 (`dialogue_service.c`)
驅動分支對話樹 (Branching Dialogue Trees)，支援根據玩家的 Progression 階段、已解鎖的 Factors 或是任務狀態動態改變對話內容與分支選項。

* **核心 API**：
  * `void start_dialogue(object player, object npc, string dialogue_id)`
  * `void choose_option(object player, int option_index)`
  * `mapping get_current_node(object player)`
* **資料驅動定義示例 (`/content/dialogues/village_elder.yaml`)**：
  ```yaml
  npc_id: "village_elder"
  nodes:
    start:
      text: "歡迎來到這個蠻荒的世界...你準備好開始求生了嗎？"
      conditions:
        stage: "novice"
      options:
        - text: "我準備好了！(開始挑戰)"
          next_node: "tutorial_start"
          action: "accept_quest:first_kill"
        - text: "我還需要再想想。"
          next_node: "exit"
    exit:
      text: "活下去是唯一的目標，注意安全。"
      options: []
  ```

---

## 🔄 虛擬物件 (Virtual Object) 編譯器映射

為了避免為每隻怪物、每個房間建立一個 `.c` 檔案，我們將利用 LPC `master.c` 的 `compile_object` 攔截器，將 YAML 檔案動態渲染為虛擬 LPC 物件：

1. **檔案結構**：
   * 所有設定檔放於 `/content/rooms/`、`/content/monsters/`。
   * 沒有實體 `/rooms/triassic_plains.c` 或 `/monsters/proto_chicken.c`。

2. **`compile_object(string file)` 攔截邏輯**：
   ```c
   object compile_object(string file) {
       string *parts = explode(file, "/");
       
       // 攔截怪物載入
       if (parts[0] == "monsters") {
           string id = parts[1]; // 例如 "proto_chicken"
           string yaml_path = sprintf("/content/monsters/%s.yaml", id);
           if (file_size(yaml_path) > 0) {
               // 載入通用虛擬怪物基底，並注入 YAML 資料
               object vo = clone_object("/std/virtual/monster.c");
               vo->initialize_from_yaml(yaml_path);
               return vo;
           }
       }
       
       // 攔截房間載入
       if (parts[0] == "rooms") {
           string id = parts[1];
           string yaml_path = sprintf("/content/rooms/%s.yaml", id);
           if (file_size(yaml_path) > 0) {
               object vo = clone_object("/std/virtual/room.c");
               vo->initialize_from_yaml(yaml_path);
               return vo;
           }
       }
       return 0;
   }
   ```

---

> [!NOTE]
> 透過上述設計，FSE 的 LPC 程式碼將縮減 80% 以上，開發一個新的 Adventure 將完全轉變為「編寫 YAML 設定檔 + 設計前端 UI」，大幅降低開發門檻。
