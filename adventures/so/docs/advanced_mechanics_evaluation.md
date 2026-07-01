# FSE 2.0 進階機制實作評估報告 (Advanced Mechanics Evaluation)

本報告針對「業力循環」、「傳承系統」與「宗門共鳴」三個核心進階機制的 LPC 實作可行性、架構對接點以及性能開銷進行評估，並給出具體的類別方法與程式碼結構建議。

---

## 一、 業力循環 (Karmic Loop & Heart Demons)

### 1. 鬼打牆與道路隱藏 (Karmic Exits Redirection)
* **可行性**：**極高 (已具備基礎)**。
* **技術對接**：
  在 `fse_room.c` 的 `query_paths(object actor)` 函數中，我們已經實作了 `karma_loop_threshold` 檢查。
* **升級方案 (道路隱藏)**：
  除了直接將所有路徑重定向回自身（鬼打牆）之外，可以透過以下方式實現「特定路徑隱藏」：
  ```c
  // fse_room.c
  mapping query_paths(object actor) {
      mapping visible = ::query_paths(actor);
      int karma = actor->query_karma();
      
      // 業力大於 60，過濾掉帶有 "requires_purity" 標記的路徑
      if (karma > 60) {
          mapping reveal_cfg = _query_reveal_paths();
          foreach (string dir, mapping data in reveal_cfg) {
              if (data["requires_purity"] && !undefinedp(visible[dir])) {
                  map_delete(visible, dir); // 在玩家視角中移除該路徑
                  tell_object(actor, HIK "【心魔】你濁氣纏身，眼前原本清明的小徑被濃霧遮蔽了。\n" NOR);
              }
          }
      }
      return visible;
  }
  ```

### 2. 心魔 NPC 生成 (Dynamic Heart-Demon Spawning)
* **可行性**：**高 (需擴充 presence.c 與心跳)**。
* **技術對接**：
  在 `user.c` 的 `heart_beat()` 中檢測，當玩家在打坐且 `karma > 70` 時，有機率調用 `spawn_heart_demon()`。
* **程式碼設計**：
  ```c
  void spawn_heart_demon() {
      object env = environment(this_object());
      if (!env) return;
      
      // 複製一個心魔存在體
      object demon = clone_object("/std/presence.c");
      if (demon) {
          demon->set_recognition_rules(
              "【心魔】" + this_object()->query_name() + "的執念分身",
              "一團扭曲的陰影",
              ({ "stillness_resonance" }) // 玩家必須悟出靜慮共鳴才能看清其真身
          );
          move_object(demon, env);
          tell_object(this_object(), HIR "🌀 你的內心執念具現化了！「" + demon->query_name() + "」從陰影中走了出來...\n" NOR);
      }
  }
  ```

---

## 二、 傳承系統 (Sect Heritage & Jade Slips)

### 1. 知識點封印為玉簡 (Factor Packaging)
* **可行性**：**極高 (基於已有的 item.c 結構)**。
* **技術對接**：
  玩家輸入 `seal_insight <factor_id>` 命令。系統檢查玩家是否擁有該 Factor，若有，則扣減 `spiritual_energy`，並複製一個帶有該 Factor 元數據的玉簡物品。
* **程式碼設計**：
  ```c
  // 在 player/commands.c 或 user.c 中
  int do_seal_insight(string fid) {
      if (!this_object()->has_factor(fid)) {
          write("❌ 你尚未領悟這項法則，無法封印傳承。\n");
          return 1;
      }
      if (this_object()->query_spiritual_energy() < 100) {
          write("❌ 你的靈力不足（需要 100 點），無法分神凝聚玉簡。\n");
          return 1;
      }
      
      this_object()->add_spiritual_energy(-100);
      object slip = clone_object("/std/item.c");
      if (slip) {
          slip->set_item_id("heritage_jade_slip");
          slip->set_name("傳承玉簡 (" + fid + ")");
          slip->set_long("一枚溫潤的青色玉簡。裡面封印著前人關於「" + fid + "」的領悟手札。\n輸入 <read_slip> 可參悟其因果。");
          slip->set_temp("stored_factor", fid);
          slip->set_temp("creator_uid", this_object()->query_uuid());
          move_object(slip, this_object());
          write("🎁 成功凝聚傳承玉簡，已放入背包。\n");
      }
      return 1;
  }
  ```

### 2. 玉簡讀取與因果互聯 (Reading & Karma Feedback)
* **可行性**：**高 (需與 factor_service 對接)**。
* **技術對接**：
  當新玩家輸入 `read_slip` 時，`item.c` 讀取暫存的 `stored_factor`。若成功參悟，發布事件，給玉簡創作者進行「因果消業（扣減 karma）」。
* **性能與持久化開銷**：
  * **開銷極小**：玉簡是一個標準物件，只有在複製和玩家讀取時會觸發 `factor_service`。
  * **因果跨線連結**：若玉簡創作者已登出，可透過 `save_state()` 異步儲存其待消的業力（Karmic Backlog），等其下次上線時自動套用。

---

## 三、 宗門共鳴 (Sect Resonance Bonus)

### 1. 多人同節點共鳴判定 (Presence Counting)
* **可行性**：**極高**。
* **技術對接**：
  在 `fse_room.c` 中，透過 `actors`（節點內所有物件的陣列）來動態計算目前有多少「正在打坐的同門玩家」。
* **程式碼設計**：
  ```c
  // fse_room.c
  int count_meditating_players() {
      int count = 0;
      foreach (object ob in actors) {
          if (userp(ob) && ob->query_meditating()) {
              count++;
          }
      }
      return count;
  }

  // user.c 在 heart_beat() 中取得共鳴加乘
  float query_resonance_multiplier() {
      object env = environment(this_object());
      if (!env) return 1.0;
      
      int num = env->count_meditating_players();
      if (num <= 1) return 1.0;
      
      // 每多一人同處打坐，靈氣恢復效率額外提升 20%
      return 1.0 + (0.2 * (num - 1));
  }
  ```
* **性能評估**：
  * **計算開銷**：`count_meditating_players()` 只需遍歷當前房間的 `actors` 陣列（通常在 10 人以內），運算開銷在微秒級別，非常輕量。
  * **事件驅動**：不需要在全域心跳中輪詢，只需在每個玩家打坐心跳恢復時，向當前 Room 查詢 `count_meditating_players` 即可，性能極其優秀。

---

## 四、 實作優先順序與時程建議 (Roadmap)

根據開發難易度與遊玩反饋，建議的實作步驟為：

1. **第一步：宗門共鳴 (Sect Resonance)**：
   * 代碼變動最少（僅需修改 `fse_room.c` 與 `user.c` 恢復公式），能最快實作多人同房的社交 Bonus，建議優先。
2. **第二步：傳承玉簡 (Jade Slips)**：
   * 建立一個新的 `item` 行動，涉及簡單的 Factor 解鎖與屬性扣減。可作為玩家進入新手期後，互相交易與引導的趣味媒介。
3. **第三步：業力循環隱藏路徑與心魔 presence**：
   * 涉及 presence.c 複製與心跳事件監聽，代碼邏輯相對複雜，可作為大版本更新的核心中後期機制。
