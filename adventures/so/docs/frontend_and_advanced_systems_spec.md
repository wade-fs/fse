# FSE 2.0 前端通訊協議與進階玩法設計規範

本文件定義了 FSE 平台的「前端 WebSocket 實時互動協議」與「Reality 評估雷達圖設計」，並規劃了業力循環、傳承玉簡及宗門共鳴等進階玩法的實現路徑。

---

## 一、前端 WebSocket 實時通訊協議

為了在網頁前端實時更新玩家的感知狀態、Observations 及修仙屬性，我們設計了一套基於 JSON 的 WebSocket 協議。

```
                    ┌──────────────┐
                    │  Web Client  │
                    └──────┬───────┘
                           │  1. Predict Action (JSON)
                           ▼
                    ┌──────────────┐
                    │ MUD WebSocket│
                    └──────┬───────┘
                           │  2. Evaluate & Evolve (Resolver)
                           ▼
                    ┌──────────────┐
                    │ State Push   │ (node_state / observations / karma)
                    └──────────────┘
```

### 1. 伺服器推播訊息 (Server Push Events)
每當玩家切換節點、屬性變更或感知更新時，MUD 伺服器會通過 WebSocket 主動推送以下狀態訊息：

#### A. 節點與感官狀態推播 (`node_state`)
```json
{
  "event": "node_state",
  "data": {
    "node_id": "meditation_cliff",
    "name": "靜慮崖",
    "desc": "蜀山深處一處孤懸的峭壁。四周松濤如海，冷風刺骨...",
    "paths": {
      "down": "/nodes/mountain_path/node"
    },
    "sensory_signals": {
      "wind": "峭壁上冷風刺骨，松濤如怒吼般在耳邊迴盪。",
      "ground": "青石平整，上面覆蓋著薄薄的冰霜。"
    }
  }
}
```

#### B. 觀察 Evidence 與修仙狀態更新 (`status_update`)
```json
{
  "event": "status_update",
  "data": {
    "spiritual_energy": 120,
    "karma": 45,
    "is_meditating": true,
    "observations": {
      "howling_wind": 1782928372,
      "physical_tension": 1782928375
    }
  }
}
```

#### C. Reality 評估結果推播 (`reality_evaluation`)
當玩家發起 Predict 行動並由 Resolver 完成判定後發送：
```json
{
  "event": "reality_evaluation",
  "data": {
    "challenge_id": "meditation_cliff_first_contact",
    "action": "meditate",
    "final_state": "MISUNDERSTANDING",
    "branch_results": {
      "spiritual": {
        "status": "MISUNDERSTANDING",
        "world_change": "actor_tension_persists",
        "new_signals": ["chest_tightness"]
      },
      "natural": {
        "status": "UNDERSTANDING",
        "world_change": "temperature_felt"
      }
    }
  }
}
```

---

## 二、前端感官介面與 Reality 雷達圖設計

### 1. 多感官主動交互按鈕 (Active Sensory Panel)
網頁端不只提供傳統的文字輸入，還提供一排「感官發現快捷鍵」：
* 👁️ **「觀察四周/地面」**：發送 `action: "observe", target: "ground"`
* 👂 **「靜心傾聽」**：發送 `action: "listen", target: "surroundings"`
* 👃 **「細細嗅聞」**：發送 `action: "smell", target: "air"`

點擊後，MUD 伺服器會依據節點 YAML 的 `sensory_signals` 過濾器判定，若條件滿足，則回傳新的 Observations，並在前端播放「感官震動」動畫與音效。

### 2. 三維 Reality 評估雷達圖 (Radar Chart)
前端面板角落設有一個由 HTML5 Canvas (或 Chart.js) 渲染的 **「天道理解雷達圖」**。三個頂點分別代表：
* **Natural (自然物理理解度)**
* **Social (紅塵社會因果度)**
* **Spiritual (精神靈性共鳴度)**

#### 雷達圖數值更新邏輯：
* 數值計算來源於玩家擁有的 `factors` (知識點) 數量與分類比率。
* 當玩家在挑戰中獲得 `UNDERSTANDING` 時，對應維度會擴張，伴隨金色脈衝特效。
* 當遭受 `MISCONCEPTION` 天雷時，Spiritual 維度會劇烈收縮，並呈現紊亂的紅色電氣動畫。

---

## 三、進階機制實現路線圖 (Advanced Systems)

### 1. 業力循環 (Karmic Loop & Heart Demons)
* **道路隱藏**：若業力 `karma > 60`，節點的 `paths` 列表中會隱藏部分非主要出口（如隱藏秘境或逃生通道）。
* **心魔 NPC 生成**：
  * 當玩家在 `desire_pavilion` (煙雨青樓) 等紅塵深重節點打坐且 `karma > 70` 時，心跳會觸發 presence 自動 spawn 出心魔對象（如 `Disguised NPC: 執念分身`）。
  * 該 NPC 會主動糾纏玩家發起 dialogue，強制要求玩家進行心境對話（Resolver 挑戰），若失敗則扣減大量的 `spiritual_energy`。

### 2. 傳承系統 (Sect Heritage & Jade Slips)
* 玩家達到一定境界（如 `spiritual_energy > 500`），可以使用 `craft` 指令將自己已解鎖的關鍵 `Factors` (領悟) 封印進一個實體道具 **「傳承玉簡 (Jade Slip)」**。
* 玩家可以將玉簡留在 `sutra_pavilion` (藏經閣) 的石台上。
* 後來的玩家（其他玩家物件）拾取並閱讀此玉簡後，可以直接繼承（解鎖）該 Factor，並留下一個「拜謝前人」的因果業力連結（減少贈與者的業力）。

### 3. 宗門共鳴 (Sect Resonance Bonus)
* 當多個玩家同時出現在同一個 `cultivation` 類型節點（如靜慮崖）中一起打坐時：
  * 節點的 `environmental_multipliers` 靈氣恢復倍率會觸發 **共鳴疊加 (Sect Resonance)**：
    $$\text{Spiritual Recovery} = 1.5 + (0.2 \times N)$$
    （$N$ 為同時打坐的同門人數）。
  * 並且打坐時隨機頓悟 `wind_insight` 等 Observation 的機率會提高。
