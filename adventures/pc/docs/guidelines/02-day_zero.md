# Stage 02: 第零天 (Day Zero)

## 📖 故事與情境
你被推入了更為致命的低氧與高溫地熱峽谷。這裡的氣溫高達 55°C，大氣含氧量僅 16%，且低窪處充斥著高濃度的二氧化碳。任何劇烈奔跑或原地發呆都會導致迅速窒息與灼傷。生存的最高原則在此展現：失敗是最高密度的信息。

---

## 🗺️ 地圖結構
本階段圍繞著「乾燥峽谷」展開：
*   **[乾燥峽谷](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/desert_canyon/room.yaml) (desert_canyon)**：起點。高溫低氧地熱谷底。
    *   ➔ **[地熱噴口](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/thermal_vents/room.yaml) (thermal_vents)**：高溫噴口區（`go vents`，需領悟 `heat_regulation`）。可收集引火材料硫磺結晶。
    *   ➔ **[乾涸古河道](file:///home/wade/fse/adventures/pc/mudlib/content/rooms/dried_riverbed/room.yaml) (dried_riverbed)**：沉積岩層（`go drybed`）。可找到遠古貝殼化石並採集燧石 (flint)。
    *   ➔ **[天然石拱門](file:///home/wade/fse/adventures/pc/docs/guidelines/02-day_zero.md) (stone_arch)**：風蝕石拱（`go arch`）。可觀察翼手龍祖先的飛行動態，取得蛋殼碎片。

---

## 🔍 解謎與領悟路徑 (Discovery Loop)

### 1. 致命的嘗試（以死求知）
*   **奔跑的後果**：玩家試圖快跑（`run`），會因缺氧瞬間昏厥濒死（HP-100），但獲得了認知因子：`oxygen_scarcity` (大氣缺氧)。
*   **觸摸的後果**：玩家觸摸高溫曝曬的紅砂岩（`touch ground`），雙手被重度灼傷（HP-100），但解鎖了因子：`heat_regulation` (熱調節規律)。
*   **原地等待的後果**：在二氧化碳沈降的谷底發呆呼吸（`wait`/`breathe`），會因二氧化碳中毒而失去知覺（HP-100），但解鎖了因子：`co2_toxicity` (二氧化碳中毒)。

### 2. 領悟與避難 (Aha! Moment)
*   **熱調節 Reveal**：玩家在解鎖 `heat_regulation` 後，再次 `look` 世界，會發現隱藏在峭壁深處的 `shade` (背光洞窟陰影) 安全避難所。
*   **進入安全區**：輸入 `go shade` 躲避烈日與地表高溫，成功度過第零天求生考驗。
