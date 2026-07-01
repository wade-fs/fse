# 🏔️ 蜀山奧德賽：天劫、心魔與境界認知博弈系統落成報告

> [!NOTE]
> 本報告彙整了近期針對蜀山因果業力、天劫挑戰與心魔幻境的重大修正與實體落地成果。所有機制皆已整合至 FSE 認識論（Observe-Predict-Evaluate-Evolve）框架中，並通過 100% 的自動化綠燈整合測試。

---

## 🛠️ 重大修正與落成內容

### 1. 核心認知解析器重構 (`reality_resolver.c` & `karma.yaml`)
*   **消除 LPC 引用傳遞限制**：繞過了當前 LPC VM 不支援 `&` 或 `ref` 參數傳遞引用的底層限制，將分支評估 `evaluate_knowledge_branch` 重構為傳回 `mapping` 結構的純值傳遞模式，確保了 100% 的編譯相容與安全。
*   **心魔對抗攔截邏輯**：在精神因果律中追加了 `greedy_attachment`（貪）、`hatred_forgiveness`（嗔）與 `ego_dissolution`（痴）的 Knowledge 定義。當玩家在心魔中採取 `fight/resist` 的蠻力對抗時，Resolver 會直接判定為 `MISCONCEPTION`，並施加 `ego_clinging`（自我執著）成見糾纏；唯有採取 `observe/let_go/forgive` 禪修才能順利化解。

### 2. 修仙境界與天劫境界跌落機制 (`user.c` & `reality_resolver.c`)
*   **境界 API 落地**：在 [user.c](file:///home/wade/fse/adventures/pc/mudlib/std/user.c) 中新增了境界屬性（`query_realm` / `set_realm`），支援 `"golden_core"`（金丹）、`"nascent_soul"`（元嬰）與 `"great_ascension"`（大乘）。
*   **天劫修為倒退懲罰**：當玩家天劫挑戰失敗時，將遭受雷劫重創與境界崩碎懲罰：
    *   **大乘飛升天劫失敗**：跌落至元嬰期，最大生命值重創 `-80 HP`。
    *   **元嬰天劫失敗**：元嬰被天雷撕裂，跌落至金丹期，重創 `-50 HP`。
    *   **金丹天劫失敗**：金丹蛛網般裂紋，跌回入道期（`initiation`），重創 `-30 HP`。

### 3. 九枚天劫雷法符文與三幕敘事結構 (`tribulation_cloud/room.yaml`)
*   **符文因子落地**：在 `content/factors/` 下物理建立了 9 枚象徵不同認知階段的雷法符文定義檔（`rune_zhen` 至 `rune_zhong`），讓天劫挑戰擁有視覺化的認知意義。
*   **三幕天劫敘事**：天劫雷雲重構為醞釀前兆、高潮對抗（`observe_rune zhen/li/kan` 觀照符文）與落幕昇華（天雷化為靈雨）的三幕結構。

### 4. 貪嗔痴三毒心魔幻境流程 (`heart_demon_realm/room.yaml` & Challenges)
*   **三毒魔王具現**：
    *   **貪執魔**：`release greed_shadow` (放下一得空)、`grab` (強抓逆流重創)。
    *   **嗔恨魔**：`forgive wrath_shadow` (寬恕塵緣消散)、`fight` (交戰越戰越強)。
    *   **痴妄魔**：`accept self` (直面接納自我)、`deny` (Deny 炸裂元神)。
*   **三魔 Challenge YAML 落地**：物理建立了 [heart_demon_greed.yaml](file:///home/wade/fse/adventures/pc/mudlib/content/challenges/heart_demon_greed.yaml)、[heart_demon_hatred.yaml](file:///home/wade/fse/adventures/pc/mudlib/content/challenges/heart_demon_hatred.yaml) 與 [heart_demon_delusion.yaml](file:///home/wade/fse/adventures/pc/mudlib/content/challenges/heart_demon_delusion.yaml)，並在完全降服三魔後，方可藉由 `let_go obsession` 統合了結，獲得心魔抵抗（`demon_heart_resistance`）因子。

---

## 📈 測試驗證概覽
在 [test_shushan_karma.c](file:///home/wade/fse/adventures/pc/mudlib/tests/test_shushan_karma.c) 中新增的**「測試 6、7-2、8、9」**，完整覆蓋了以下斷言：
1.  金丹期三枚基礎雷符（震、離、坎）的觀照與因子解鎖。
2.  三毒心魔（貪、嗔、痴）分步降服、因子獲得與 `let_go` 統合了結。
3.  三魔 Challenge YAML 與三境界天劫 YAML 檔案存在性與加載健全度。
4.  在心魔挑戰中執意對抗（`fight`）被 Resolver 攔截並施加成見懲罰。
5.  在天劫失敗時，元嬰期玩家境界自動跌落回金丹期且 HP 重傷受損。

> [!TIP]
> 執行 `make test-pc` 測試套件，21 個大項、數十個小項測試已全線 100% 綠燈成功通關。
