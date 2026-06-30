# 《史前文明》(Prehistoric Civilization) 史詩級開發藍圖與系統規劃

本規劃旨在將現有的 PC 冒險原型擴展為一個深度整合物理規律、生態演進、工具鍛造與文明傳承的史詩級沙盒生存模擬系統。

---

## 一、文明演進四大紀元與擴展 Stage 鏈結

```mermaid
graph TD
    classDef triassic fill:#e05b5b,stroke:#333,stroke-width:2px,color:#fff;
    classDef jurassic fill:#e0a05b,stroke:#333,stroke-width:2px,color:#fff;
    classDef cretaceous fill:#5be0a0,stroke:#333,stroke-width:2px,color:#fff;
    classDef apocalypse fill:#5b5be0,stroke:#333,stroke-width:2px,color:#fff;

    A[三疊紀：荒原適應與大氣對抗] :::triassic --> B[侏羅紀：巨型生態與樹冠天網] :::jurassic
    B --> C[白堊紀：富氧鍛造與聚落崛起] :::cretaceous
    C --> D[末日紀元：小行星撞擊與文明火種] :::apocalypse
```

---

## 二、紀元深度擴展與核心物理謎題

````carousel
### 紀元一：三疊紀（適應與生命防線）
*   **新場景**：`lava_tube` (火山熔岩管)、`alkali_flats` (鹽鹼荒灘)、`wind_canyon` (風切大峽谷)。
*   **物理謎題**：
    *   **低氧奔跑阻抗**：計算運動帶來的氧氣消耗速率，引導玩家利用岩縫產生的氣流漩渦（白努利定律）尋找高氧聚集點。
    *   **沙塵暴偏振導航**：玩家必須打製薄片雲母石，透過偏振光學原理穿透蔽日沙塵暴，辨識恆星方位進行荒野定位。
*   **解鎖因子**：`polarization_navigation` (偏振導航)、`convection_currents` (熱對流引導)。
<!-- slide -->
### 紀元二：侏羅紀（樹冠天網與聲學操控）
*   **新場景**：`swamp_forest` (巨型針葉沼澤)、`canopy_bridge` (樹冠吊橋群)、`raptor_run` (迅猛龍迴廊)。
*   **物理謎題**：
    *   **張力吊橋搭建**：在百米高空樹冠搭建懸索吊橋，玩家必須利用槓桿與動滑輪原理（計算拉力係數），否則強風與重力會使結構崩塌。
    *   **次聲波生物頻率定位**：透過中空的竹管（亥姆霍茲共振器）接收遠方大型恐龍的次聲波，在暴風雨前夕計算聲學波長避開雷區。
*   **解鎖因子**：`tension_engineering` (張力工程學)、`subsonic_resonance` (次聲波共振)。
<!-- slide -->
### 紀元三：白堊紀（富氧鍛造與文明萌芽）
*   **新場景**：`volcanic_vent` (火山地熱口)、`fertile_delta` (肥沃三角洲)、`coal_fields` (天然煤炭層)。
*   **物理謎題**：
    *   **30% 高氧山火擴散**：在極度易燃的富氧空氣中，火種一旦落地會引發擴散（流體熱動力學模擬）。玩家必須設計防火隔離帶或利用反向引火（Backfire）撲滅火勢。
    *   **地熱低溫熔煉**：利用火山噴氣孔的溫差與陶製鼓風管，對黃銅礦進行還原焙燒，鍛造人類第一支史前青銅工具。
*   **解鎖因子**：`oxygen_combustion` (富氧燃燒)、`extractive_metallurgy` (提取冶金術)。
<!-- slide -->
### 紀元四：末日紀元（文明火種與熵對抗）
*   **新場景**：`impact_crater` (小行星撞擊點)、`underground_vault` (地下傳承避難所)。
*   **物理謎題**：
    *   **撞擊冬夜光照模擬**：小行星撞擊後陽光被遮蔽，地表進入零下 40°C 的撞擊冬夜。玩家必須建造熱能儲蓄室與菌菇培養槽以維持生命。
    *   **石板符號防腐與電阻歸檔**：為了讓億年後的後代能讀懂人類的歷史，玩家必須以特定配比的黑曜石玻璃與石墨層進行多晶矽結構刻蝕，確保信息能抵抗小行星產生的強大電磁脈衝（EMP）。
*   **解鎖因子**：`impact_winter` (撞擊冬夜)、`information_preservation` (信息永存技術)。
````

---

## 三、未來引擎擴展機制 (ADR 提案)

> [!IMPORTANT]
> 為了支撐上述史詩級擴展，FSE 核心引擎需要升級三個關鍵子系統：

### 1. 動態大氣與熱力學引擎 (Dynamic Atmosphere Engine)
在 `fse_room.c` 之上建立動態環境狀態變量：
```c
struct atmosphere {
    float oxygen_percent;     // 氧氣濃度（如三疊紀 15%，白堊紀 30%）
    float co2_percent;        // 二氧化碳濃度（影響玩家最大耐力）
    float ambient_temp;       // 環境溫度（受晝夜與地熱動態影響）
    float wind_velocity;      // 風速向量（動態改變逆風潛行成功率）
};
```

### 2. 物理結構負載校驗器 (Structural Load Validator)
支持玩家使用 `build` 指令以材料為參數進行結構搭建，引擎進行力矩解算：
```c
int validate_construction(string material, int span_length, int load_weight) {
    // 依據 fiber_strength 與 structural_load 的解鎖狀態
    // 計算材料的最大許用應力與撓度變形，決定平台或吊橋是否損壞
}
```

### 3. 跨世代繼承與「壁畫記憶」存檔系統 (Legacy Archiving System)
當白堊紀末期撞擊發生後，玩家死亡將不再只是普通復活，而是「紀元更迭」：
*   **石板遺產**：玩家在 `ancestor_cave` 中刻下的符號壁畫與文字，將作為數據永久寫入專案的 `data/world_history.json`。
*   **次世代發現**：在新一輪遊戲或新玩家進入時，可以直接透過 `look painting` 或 `read stone` 解鎖上一輪文明遺留下來的高階 Factors，形成跨世代的知識反饋閉環。

---

## 四、下一階段開發規劃 (Roadmap)

- [ ] **Phase 1 (三疊紀深挖)**：開發 `desert_dunes` (沙丘) 和 `lava_tube` (火山熔岩管) 房間配置，實作低氧限制與導航謎題。
- [ ] **Phase 2 (樹冠天網)**：開發高空懸索與次聲波感知。
- [ ] **Phase 3 (火山金屬冶煉)**：實作陶器焙燒的後續——地熱鼓風爐與礦石冶煉。
- [ ] **Phase 4 (歷史歸檔)**：撰寫傳承文件讀寫服務 `/runtime/services/legacy_archiver.c`，儲存並解讀上一次小行星撞擊前的玩家雕刻遺產。
