# 14-Programming-World.md

# 第 13 章　Programming World

Programming World（PW）是 FSE Runtime 的第一個完整驗證器與實驗室。

它不是一個普通的「程式教學遊戲」，而是**用程式概念作為情境，讓理解透過探索自然形成的冒險**。

---

### PW 的設計目標

PW 的核心任務是驗證 FSE 的核心循環是否能有效運作：

**探索 → 行動 → 困惑 → 發現 → 新路徑**

在 PW 中，這個循環被具象化為：

- 進入不同程式概念的「地標」（Node）
- 透過 Blockly 或程式邏輯進行行動
- 遭遇程式錯誤或無窮迴圈的困惑
- 領悟程式概念（Factor / Discovery）
- 解鎖新的積木與更複雜的挑戰（Path）

---

### PW 中的核心概念映射

| FSE 核心概念     | PW 中的表現形式                  |
|------------------|----------------------------------|
| World            | 程式執行的概念世界               |
| Node             | 不同程式概念的地標（如無窮迴圈沼澤、計數山谷） |
| Actor            | 正在學習程式的探索者             |
| Discovery        | 領悟程式概念（如「迴圈需要終止條件」） |
| Path             | 新解鎖的積木類別與進階挑戰       |
| Reveal           | 工具箱（Toolbox）的動態更新      |

---

### PW 的技術實現亮點

- **AST 映射**：無論前端使用何種語法，都轉成統一的抽象語法樹進行驗證
- **後端主導 Toolbox**：積木解鎖完全由 Runtime 根據 Discovery 狀態決定
- **Confusion 與 Failure Consequence**：錯誤程式碼會真正導致「記憶體洩漏」等後果
- **Stage Progression**：透過完成關鍵 Discovery 來推進世界階段
- **Virtual Node**：所有關卡地標都由 YAML 資料驅動

---

### PW 對 FSE 的貢獻

PW 作為第一個 Adventure，幫助我們驗證並打磨了許多核心機制：

- 確認 Discovery Loop 在實際世界中可行
- 驗證 Reveal Layer 在視覺化介面上的效果
- 逼迫 Runtime 保持足夠抽象（不依賴特定前端）
- 提供了一個極佳的「概念到實作」的轉化範例

---

### PW 的未來方向

- 支援更多程式語言的 Input Medium（Python、JavaScript 等）
- 加入並行、資料結構、演算法等更進階的概念地標
- 發展多人協作探索模式
- 成為其他教育類 Adventure 的參考模板

---

**本章總結**

Programming World 不只是教程式設計。

它是**用程式作為情境，讓理解透過探索自然形成的實驗場**。

它證明 FSE 的理論可以在具體領域中落地，並為後續 Adventure 樹立了標竿。

---

**下一章預告**  
第 14 章　Prehistoric Civilization

我們將探討如何把環境訊號作為 Input Medium，以及史前世界的設計思考。

