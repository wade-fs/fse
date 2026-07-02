# Flow State Engine (FSE) 2.0 哲學與架構總覽 (Overview & Philosophy)

> **"If you have not lived in a world, you have not yet understood it."**  
> —— FSE 核心思想

---

## 一、 FSE 的哲學 (Philosophy of FSE)

FSE 不是一個傳統的 Quest Engine (任務引擎)。  
FSE 不是一個簡單的 Dialogue Engine (對話引擎)。  
FSE 不是一個常規的 RPG Framework (角色扮演框架)。  

**FSE 是一個讓玩家透過「生活」、「觀察」、「推理」與「驗證」，逐步理解一個世界的「認識論引擎 (Epistemological Engine)」。**

在 FSE 的世界中，世界不圍繞著「二元對立的成敗」轉動。世界是由一組客觀存在的客觀規律（Realities）組成的。玩家要克服困難，唯有先去犯錯、收集感官線索（Evidence）、在心智中歸納出對應的知識（Knowledge），並在關鍵時刻發起正確的預測行動。

這不是打怪升級的過程，這是心智開悟與理解真相的旅程。

---

## 二、 依賴原則與依賴方向

FSE 的架構嚴格遵守以下**依賴公約 (Core Dependency Principle)**：

> **「Runtime 永遠不知道 Adventure 的存在；Adventure 可以依賴 Runtime，但 Runtime 不得依賴任何 Adventure。」**

這是一條不可逾越的鐵律。底層引擎必須對上層故事保持絕對的中立。

```
  【資料層】（創作者定義，Boot 前即存在）
  ┌────────────────────────┐
  │   Adventure 故事層     │ (尋仙問道 SO、編程世界 PW 等，定義故事、節點、進程)
  └───────────┬────────────┘
              │ 引用
              ▼
  ┌────────────────────────┐
  │ Reality Package 規律包 │ (Law YAML 庫，定義各個規律法則與判定パターン)
  └───────────┬────────────┘
              │ 引用
              ▼
  ┌────────────────────────┐
  │   Knowledge 知識樹     │ (Factor/Knowledge 定義，跨模組共用的一級知識資產)
  └────────────────────────┘

              │ 執行期讀取 (無向上依賴)
              ▼

  【執行層】（引擎核心，Boot 時啟動與加載）
  ┌────────────────────────┐
  │  Runtime Services      │ (如 event_bus, factor_service, progress_manager 等)
  └───────────┬────────────┘
              │ 建構於
              ▼
  ┌────────────────────────┐
  │     Core Runtime       │ (虛擬物件編譯、虛擬機驅動、基本物件繼承基底)
  └────────────────────────┘
```


---

## 三、 FSE 2.0 官方規範矩陣 (Specification Matrix)

本手冊分為四個核心文件，請依據您的開發角色進行閱讀：

1. **[FSE 2.0 哲學與架構總覽 (本文件)](file:///home/wade/src/github/fse/docs/architecture/README.md)**：定義核心哲學與依賴方向原則。
2. **[FSE 2.0 引擎規範與邊界 (runtime-spec.md)](file:///home/wade/src/github/fse/docs/architecture/runtime-spec.md)**：詳述 Core Runtime、服務與運行期生命週期。
3. **[FSE 2.0 冒險結構與知識模型 (adventure-spec.md)](file:///home/wade/src/github/fse/docs/architecture/adventure-spec.md)**：定義冒險解剖學、目錄結構與一級概念「知識模型」。
4. **[FSE 2.0 創作者指南與工作流 (author-guide.md)](file:///home/wade/src/github/fse/docs/architecture/author-guide.md)**：引導創作者從零開始開發、編譯與發布一個全新的冒險模組。
