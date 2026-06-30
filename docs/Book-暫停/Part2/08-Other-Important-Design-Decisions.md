# 09-Other-Important-Design-Decisions.md

# 第 8 章　其他重要設計決策

除了前面幾章的核心概念與原則之外，FSE 還有幾個重要的設計決策，共同支撐整個系統的長期健康與擴展性。

---

### 決策一：Adventure Laboratory（PW 的角色）

Programming World（PW）並不是一個普通的 Adventure。

**它的主要角色是 FSE Runtime 的驗證器與實驗室。**

PW 負責驗證：
- Discovery Loop 是否完整
- AST Mapping 是否有效
- Reveal Layer 是否靈活
- Runtime API 是否足夠抽象

只有當 PW 能順暢跑完「探索 → 困惑 → 發現 → 新路徑」的閉環後，我們才認為 Runtime 已經足夠穩定。

---

### 決策二：Bootstrap FSE Before Importing Formosa

Formosa（原 fsmud）雖然是最初的靈感來源，但我們選擇**先穩定 FSE Runtime，再導入 Formosa**。

原因：
- 避免 Runtime 被單一 Adventure 的需求綁架
- 確保 Runtime 是真正 Adventure-Agnostic（與特定冒險無關）
- 讓 Formosa 以「大型 Adventure」的身份被導入，而不是 Runtime 的來源

這是重要的開發順序決策。

---

### 決策三：Runtime Freeze（凍結原則）

當 Runtime 穩定到「新增 Adventure 只需要內容與 Mapping，而不需要修改核心」時，我們會進行 **Runtime Freeze**。

之後的規則：
- Runtime 只因重大架構需求而修改
- Adventure 不得直接修改 Runtime 核心檔案
- 所有擴展都透過註冊、Mapping、Virtual Object 等機制實現

這能保護 FSE 的長期穩定性。

---

### 決策四：Presentation vs Reveal 的分離

Runtime 只負責 **Reveal**（什麼該顯示、何時顯示）。

**Presentation**（怎麼顯示）由 Adventure 負責。

這讓同一個 Runtime 可以支援：
- 文字 MUD
- 視覺化 Blockly介面
- 環境模擬
- 甚至未來可能的 VR/AR 呈現

---

### 決策五：資料驅動優先

盡可能把內容（Node、Challenge、Factor、Progression）放在 YAML 中定義。

優點：
- 非程式設計師也能參與內容創作
- 容易修改與擴展
- 版本控制友好
- 支援多種實作（YAML、JSON、甚至資料庫）

目前雖然主要使用 YAML，但設計上保持抽象，不鎖死特定格式。

---

**本章總結**

這些設計決策共同確保 FSE 能同時做到：

- **概念純粹**（Adventure-Agnostic）
- **高度擴展**（容易新增新世界）
- **長期維護**（Runtime Freeze + Mapping）
- **靈活表現**（Presentation 分離）

它們不是技術細節，而是讓 FSE 能真正成為「建構理解世界的平台」的關鍵支柱。

---

**下一章預告**  
Part 3　模式與實踐

從下一章開始，我們將進入更具體的模式討論，包括 Discovery Loop、Reveal 作為設計原語等。

