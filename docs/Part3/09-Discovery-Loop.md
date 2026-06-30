# 10-Discovery-Loop.md

# 第 9 章　Discovery Loop 模式

這是 FSE 最核心的運作循環。

**Discovery Loop** 描述了理解在世界中自然形成的完整過程。

---

### Discovery Loop 的完整流程
```
World（情境）
↓
Actor 採取行動（Act）
↓
產生後果（Consequence）
↓
產生困惑（Confusion）
↓
重試與探索（Retry）
↓
觸發 Discovery（領悟）
↓
開啟新的 Path（新可能）
↓
回到 World（以新視角繼續探索）
```


這是一個**閉環**，而不是線性流程。  
真正的理解就是在這個循環中反覆迭代而形成的。

---

### 各階段詳細說明

#### 1. World 提供情境
世界必須是豐富的、有因果的、值得探索的。它不直接給答案，而是提供讓人想行動的情境。

#### 2. Actor 採取行動
Actor 是主動的。他不是被動接收知識，而是透過行動提出問題。

#### 3. Consequence（後果）
行動必須有真實、可感知的後果。這是理解的燃料。

#### 4. Confusion（困惑）
這是整個循環中最關鍵也最容易被忽略的階段。

困惑代表「現有認知模型無法解釋當前現實」。  
一個好的世界不會急著消除困惑，而是允許它存在，並溫柔地引導 Actor 去探索。

#### 5. Retry（重試）
帶著困惑再次行動。這是理解開始形成的時刻。

#### 6. Discovery（領悟）
「啊，原來如此」的瞬間。  
這不是系統發放的獎勵，而是 Actor 自己悟到的。

#### 7. New Path（新路徑）
Discovery 開啟新的可能，讓世界對 Actor 展現新的面貌。

---

### 為什麼這個 Loop 如此重要？

- 它符合人類自然的理解過程
- 它把失敗（Confusion）變成有價值的階段
- 它讓進度真正反映理解深度，而不是任務完成數
- 它讓世界成為「會教會人的老師」

---

### 在 Runtime 中的實作

目前 FSE 已經透過以下機制實現 Discovery Loop：

- `node_executor.c` 負責 Consequence 與 Confusion 的處理
- `factor_service.c` 負責 Discovery 的觸發與前置條件
- `progress_manager.c` 負責 Path 的開啟與 Stage 推進
- `reveal_layer.c` 負責根據 Discovery 改變 Presentation
- `event_bus.c` 負責各階段之間的因果傳遞

---

**本章總結**

Discovery Loop 是 FSE 的心跳。

所有設計都應該服務於這個循環的順暢運作。

當一個 Adventure 能讓玩家自然地、反覆地走完這個 Loop 時，我們就說：**這個世界活了。**

---

**下一章預告**  
第 10 章　Reveal 作為設計原語

我們將深入探討 Reveal Layer 如何成為 FSE 中最優雅也最強大的設計機制。

