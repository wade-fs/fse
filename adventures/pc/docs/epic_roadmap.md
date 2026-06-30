# 《史前文明》(PC) 史詩級開發藍圖與系統規劃 (Epic Roadmap)

本規劃旨在將現有的 PC 冒險原型擴展為一個深度整合物理規律、生態演進、工具與文明傳承的史詩級沙盒生存模擬系統。

本設計遵循 FSE 的核心精神：**以離散的資料驅動判定取代過度設計的物理引擎，並以「觀察、困惑、領悟」的 Discovery 循環取代任何形式的數值蠻幹（包括工程公式計算）**。

---

## 一、文明演進四大紀元與擴展 Stage 鏈結

```mermaid
graph TD
    classDef triassic fill:#e05b5b,stroke:#333,stroke-width:2px,color:#fff;
    classDef jurassic fill:#e0a05b,stroke:#333,stroke-width:2px,color:#fff;
    classDef cretaceous fill:#5be0a0,stroke:#333,stroke-width:2px,color:#fff;
    classDef apocalypse fill:#5b5be0,stroke:#333,stroke-width:2px,color:#fff;

    A[三疊紀：荒原適應與大氣對抗] :::triassic --> B[侏羅紀：巨型生態與樹冠天網] :::jurassic
    B --> C[白堊紀：富氧火控與聚落崛起] :::cretaceous
    C --> D[末日紀元：小行星撞擊與文明火種] :::apocalypse
```

---

## 二、紀元深度擴展與核心物理謎題

````carousel
### 紀元一：三疊紀（適應與生命防線）
*   **新場景**：`lava_tube` (火山熔岩管)、`alkali_flats` (鹽鹼荒灘)、`wind_canyon` (風切大峽谷)。
*   **離散物理謎題**：
    *   **低氧與對流觀測**：玩家處於缺氧區時，必須尋找牆上標註「有微弱熱空氣流出」的裂縫。透過 `focus wind` 觀察煙霧的對流偏折，領悟並使用 `harness convection` 以進入富氧的安全夾層，而非讓引擎去實時模擬氣體流體力學。
    *   **沙塵暴偏振定位**：藉由偏振雲母石片觀測天空（`look clouds through mica`），在沙塵暴中找出隱約的偏振光條紋以校對出南北磁極，領悟 `polarization_navigation`。
*   **解鎖因子**：`polarization_navigation` (偏振導航)、`convection_currents` (熱對流引導)。
<!-- slide -->
### 紀元二：侏羅紀（樹冠天網與聲學操控）
*   **新場景**：`swamp_forest` (巨型針葉沼澤)、`canopy_bridge` (樹冠吊橋群)、`raptor_run` (迅猛龍迴廊)。
*   **離散物理謎題**：
    *   **張力吊橋結構**：在百米高空樹冠搭建懸索吊橋，若玩家在未領悟 `tension_engineering` 時強行前進，吊橋會發出「嘎吱斷裂聲」並崩塌（玩家墜落至地表受傷，觸摩斷口觸發 `bridge_collapsed` 困惑）。玩家必須觀察藤蔓受拉力時的纖維剝落，解鎖張力工程學。這完全不需要寫公式解算器，而是靠描述與狀態鎖定。
    *   **次聲波生物頻率定位**：透過中空的竹管（`look through bamboo`）對焦遠方的雷鳴，感知到空氣中細微的壓迫感，從而避免走入大型雷暴區。
*   **解鎖因子**：`tension_engineering` (張力工程學)、`subsonic_resonance` (次聲波共振)。
<!-- slide -->
### 紀元三：白堊紀（富氧控火與文明萌芽）
*   **新場景**：`volcanic_vent` (火山地熱口)、`fertile_delta` (肥沃三角洲)、`coal_fields` (天然煤炭層)。
*   **離散物理謎題**：
    *   **30% 富氧火控隔離**：大氣中富氧導致山火一觸即燃。玩家點火後若不進行防火帶挖掘（`dig trench`），火焰會迅速蔓延並封鎖出口。這只需要在 `room.yaml` 中利用 `temp_state` 的流轉和定時器變量來表達，不需要實時物理引擎計算。
    *   **還原焙燒與冶煉**：將孔雀石（銅礦）與陶罐、煤炭一同置入地熱口（`smelt ore`），觀察不同火焰顏色與殘留礦渣，發現青銅相變。
*   **解鎖因子**：`oxygen_combustion` (富氧燃燒)、`extractive_metallurgy` (提取冶金術)。
<!-- slide -->
### 紀元四：末日紀元（文明火種與熵對抗）
*   **新場景**：`impact_crater` (小行星撞擊點)、`underground_vault` (地下傳承避難所)。
*   **離散物理謎題**：
    *   **撞擊冬夜微光溫室**：地表陷入零下 40°C 的極寒。玩家需要尋找地下地熱裂縫，並利用蠟質葉片搭建溫室，以點狀的局域熱源抵禦失溫。
    *   **石板符號刻蝕防腐**：為使歷史能抵禦衝擊後的強烈電磁脈衝（EMP），玩家必須選擇黑曜石與石墨（而非金屬或骨頭）進行刻蝕，並理解「碳素與玻璃的物理穩定性」，領悟 `information_preservation`。
*   **解鎖因子**：`impact_winter` (撞擊冬夜)、`information_preservation` (信息永存技術)。
````

---

## 三、堅持 FSE 設計哲學：避免過度一般化與數值陷阱

未來開發者在沿著本藍圖擴充時，必須時刻牢記以下兩個防線，防止本冒險偏離 FSE 設計哲學：

### 1. 堅決不將特定冒險邏輯焊死在 Engine 層（拒絕 Premature Generalization）
*   **現狀**：目前 PC 的大氣變量（如溫度、二氧化碳）完全封裝在 PC 的 `room.c` 與對應的 `room.yaml` 離散屬性中。
*   **原則**：FSE 核心引擎（如 `/std/fse_room.c`）只應處理通用的 Reveal 機制、Prerequisites 校驗和 Discovery 框架。
*   **禁止做法**：**嚴禁**在 FSE 核心共用基底上添加 `atmosphere struct`（包含氧氣、風向向量、濕度等）。因為其他 Adventure（如 Formosa 或 PW）完全不需要這些物理屬性。這類生存規則必須僅以離散數據欄位的形式，配置在 PC 專屬的 `room.yaml` 裡，並由 PC 的 `room.c` subclass 透過 `apply_adventure_side_effects` 進行自訂處理。

### 2. 嚴防「數值公式計算」繞過 Discovery 體驗（拒絕工程數值蠻幹）
*   **陷阱**：例如實作「結構負載校驗器」時，若採用 `validate_construction(material, span_length, weight)` 這樣的公式函數，玩家就會被迫上網查公式、套數字，一旦回傳 `true` 即可過關。**這本質上是用「數學公式的數值壓制」取代了傳統 RPG 的「戰鬥力數值壓制」，依然繞過了 Discovery 循環**。
*   **正確做法**：
    *   **利用 Confusion 驅動**：玩家在未解鎖 `tension_engineering` 時強行搭建吊橋，吊橋直接崩塌，玩家受傷。
    *   **利用感官線索引導**：玩家專注觀察斷裂處（`focus ground`），看到「粗藤纖維是由於單向剪力拉伸導致分層剝落，而非直接拉斷」，觸發 `bridge_collapsed` 困惑。
    *   **概念性解鎖**：玩家領悟到「必須反向擰緊兩股藤蔓才能抵消剪力」，進而解鎖 `tension_engineering`。解鎖後，再次搭建便能自動成功。
    *   **結論**：**過關的鑰匙是「解鎖認知因子 (Factor)」，而不是「算對物理公式」**。所有的交互依然是離散的語義判斷。
