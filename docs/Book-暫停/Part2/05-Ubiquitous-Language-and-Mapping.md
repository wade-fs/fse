# 06-Ubiquitous-Language-and-Mapping.md

# 第 5 章　Ubiquitous Language 與 Mapping

Runtime 必須保持中性。  
Adventure 負責進行語意映射。

這是 FSE 能夠長期保持乾淨與可擴展的核心設計原則。

---

### 為什麼需要 Ubiquitous Language？

如果 Runtime 直接使用各 Adventure 的領域名詞，會發生什麼事？

- Programming World 用 `Lesson`、`Challenge`
- Prehistoric Civilization 用 `Biome`、`Survival Event`
- Formosa 用 `Settlement`、`Site`、`Era`

Runtime 就會充滿各種特定領域的詞彙，變得混亂且難以維護。

**解決方案**：Runtime 永遠只使用一組中性、抽象的語言。

---

### FSE 的 Ubiquitous Language（通用語言）

Runtime 只認識以下核心名詞：

- **World**：整體世界
- **Node**：一個具體的情境地標（Lesson / Site / Biome）
- **Actor**：行動者（玩家、NPC）
- **Factor / Discovery**：理解的單元
- **Path**：因理解而開啟的新可能
- **Progress**：理解的累積狀態
- **Event**：因果傳遞的訊息
- **Reveal**：感知的條件式顯現

這些詞彙在所有 Adventure 中保持一致。

---

### Mapping（映射）的角色

Adventure 的責任是把自己的領域語言「映射」到 FSE 的通用語言。

**例子**：

| Runtime（中性） | Formosa          | Programming World     | Prehistoric Civilization |
|-----------------|------------------|-----------------------|--------------------------|
| Node            | Site / Settlement| Lesson                | Biome / Location         |
| Discovery       | Memory Fragment  | Concept Insight       | Fossil / Survival Insight|
| Path            | Reveal Layer     | New Toolbox Category  | New Safe Zone            |
| Progress        | Era Advancement  | Stage Progression     | Survival Level           |

這種映射讓 Runtime 保持純淨，Adventure 則可以自由使用自己最自然的語言。

---

### 為什麼這個設計如此重要？

1. **可擴展性**：新增第三、第四個 Adventure 時，不需要修改 Runtime 核心。
2. **概念清晰**：開發者永遠知道哪些東西屬於 Runtime，哪些屬於特定 Adventure。
3. **避免耦合**：Runtime 不會「知道」任何 Adventure 的業務細節。
4. **易於理解**：新加入的開發者只需學習一套通用語言，就能理解整個系統。

---

### 實作中的 Mapping 機制

目前主要透過以下方式實現：

- `master.c` 中的註冊機制（register_progression_path、register_discovery_path 等）
- `compile_object()` 的虛擬物件映射
- YAML 資料驅動的配置
- Adventure 專屬的 service（如 `blockly_service.c`）

未來可以進一步抽象成 `manifest.yaml` 來宣告映射規則。

---

**本章總結**

**Runtime 提供骨架與通用語言。**  
**Adventure 提供血肉與特定語境。**

透過 Ubiquitous Language + Mapping，我們實現了「強抽象 + 靈活表現」的平衡。這是 FSE 能夠同時支援多種不同類型世界的重要基礎。

---

**下一章預告**  
第 6 章　Discovery-First Progression

我們將討論為什麼進度應該由 Discovery 驅動，而不是傳統的任務 / 獎勵系統。

