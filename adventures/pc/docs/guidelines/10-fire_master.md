# Stage 10: 火的主人 (Fire Master)

## 📖 故事與情境
你來到了高聳入雲的翼龍峭壁。高空海風極度狂烈，你點燃的每一堆火都會被狂風瞬間吹滅。而峭壁頂端盤旋著一頭巨大的史前風神翼龍，牠飢腸轆轆且性情狂躁。你必須學會利用石壁防風機制來「控制火焰」，烹烤新鮮的蜥蜴肉，與這頭史前巨獸建立靈魂上的共生鏈接。

---

## 🗺️ 地圖結構
本階段圍繞著「翼龍峭壁」展開：
*   **[翼龍峭壁](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/pterosaur_cliff/room.yaml) (pterosaur_cliff)**：起點。狂風峭壁、風神翼龍巢穴。
    *   ➔ **[火山口](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/volcanic_crater/room.yaml) (volcanic_crater)**：活火山口（`go crater`，需與翼龍建立鏈接後騎乘飛過去）。可採集朱砂礦石。
    *   ➔ **[火山灰平原](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/ash_plain_fire/room.yaml) (ash_plain)**：火山灰堆（`go plain`）。可在此挖掘黑曜石並打製鋒利黑曜石刃。
    *   ➔ **[火圈聖地](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/fire_circle/room.yaml) (fire_circle)**：古人類火圈（`go circle`，需領悟 `fire_control`）。可研讀古老符號。
    *   ➔ **[白堊紀海岸](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/cretaceous_shore/room.yaml) (cretaceous_shore)**：海濱起點（需騎乘翼龍飛過去）。

---

## 🔍 解謎與領悟路徑 (Discovery Loop)

### 1. 石圈控火與防風 (Fire Control)
*   **風吹火熄的後果**：若玩家直接在峭壁空地上點火（`light fire`），狂烈的高空風會瞬間將火苗吹滅，無法進行烹飪。
*   **物理搭建**：玩家必須先在地面用石塊堆疊起一個環形防風石圈（`build circle` / `make stone_circle`）。
*   **控火阻風**：在石圈內部點火，火焰平穩燃燒並提供持續高溫，解鎖因子：`fire_control` (控火防風技術)。

### 2. 馴化翼龍與飛越 (Pterosaur Bonding)
*   **食物誘餌**：利用鋒利石片在石壁縫中捕獲一隻史前蜥蜴（`hunt lizard`）。
*   **熟食烹調**：將生蜥蜴肉放在控溫的石圈火堆上烤熟（`cook lizard` / `roast lizard`），散發出肉脂香氣。
*   **投餵信任**：將烤熟的蜥蜴肉投餵給飢餓的風神翼龍（`feed pterosaur` / `feed`）。
*   **巨獸鏈接**：撫摸翼龍溫熱的羽毛，與其建立靈魂共生鏈接，解鎖因子：`pterosaur_bond` (翼龍共生鏈接)。
*   **飛越海洋**：騎乘風神翼龍展翅高飛（`ride pterosaur` / `fly`），狂風呼嘯下，你飛越了險惡的海洋，降落在 `cretaceous_shore` (白堊紀海岸)！
