# Appendix. YAML Style Guide - YAML 欄位格式與靜動態開發規範 (Style Guide)

本規範定義了整個專案中 YAML 資料設定的格式規範，旨在與直譯器 `yaml_decode` 及 LPC 資料引擎完全對齊。

> 若你在找 **LPC 程式撰寫規範**（型別宣告、陣列語法、命名慣例），請參閱 [Appendix B — LPC Coding Style Guide](./Appendix_B_LPC_Style_Guide.md)。

---

## 🚫 靜動態分離開發規範 (Mandatory Separation)
*   **YAML 唯讀原則**：YAML 設定檔僅用於存放唯讀的**靜態元數據（Static Metadata）**。
*   **動態狀態禁入 YAML**：任何玩家狀態、當前聚落六維變化、當前生成中的失源者清單，**一律禁止**寫入 YAML 中。運行時變動資料必須寫入 `/data/state/` 下的 `.o` 存檔中。
*   **路徑引用規範**：YAML 中若涉及檔案或地標引用，一律使用**小寫底線 (snake_case)** 或完整的 LPC 檔案絕對路徑。

---

## 📝 欄位格式 Style (Field Styles)

### 1. 多語系文字欄位 (Multilingual Mapping)
涉及呈現給玩家的 UI 名稱與描述，一律使用映射格式：
```yaml
name:
  zh-TW: "繁體中文名稱"
  en: "English Name"
```

### 2. 時代 ID (Era IDs)
所有與時代關聯的字串，統一使用小寫與底線：
*   **正確格式**：`v0_1`, `v0_2`, `v1_0`, `v2_0`, `v2_1`, `modern`
*   注意：在 route 設定中，因為歷史原因也相容以點號分隔的 dotted 格式如 `v1.0` 或 `v2.1`。
