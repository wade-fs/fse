# ADR-0003: Discovery-First Progression

**狀態**：已採納（Accepted）
**日期**：2026
**作者**：Wade

---

## 背景

大多數學習遊戲或 RPG 的 Progression 系統，都是任務驅動的：

```
完成 Quest → 獲得 Reward → Level Up → 解鎖新內容
```

這個模型的問題：

1. 玩家為了完成任務而行動，不是為了理解而行動
2. Reward 成為目的，世界成為背景
3. 玩家可以在不理解任何事的情況下「通關」

在 FSE 的設計中，我們遭遇了一個根本問題：

**Progress 到底應該代表什麼？**

---

## 決策

採用 **Discovery-First Progression**：

Progress 是 Discovery 的累積，而不是任務數量的累積。

```
✗ 舊模型：
   Quest 完成 → Reward → Level Up → 解鎖新內容

✓ 新模型：
   Explore（探索）
       ↓
   Discover（領悟）
       ↓
   Understanding（理解）
       ↓
   New Possibility（新的可能）
       ↓
   Explore（繼續探索）
```

---

## 理由

### 1. 理解才是真正的進步

玩家在 FSE 的世界裡「前進」，

意思不是「移動到下一個關卡」，

而是「對這個世界的理解變深了」。

理解深了之後，世界對他展示新的面貌。

新的面貌帶來新的困惑，新的困惑帶來新的 Discovery。

這才是 FSE 的 Progression。

### 2. Quest 可以存在，但不是驅動力

Quest 在 FSE 裡不是不存在。

但 Quest 應該是 Discovery 的容器，而不是 Progress 的單位。

```
Quest：「找到老站長」
→ 這個 Quest 的完成，不是 Progress
→ Progress 是：在尋找老站長的過程中，玩家理解了什麼
```

### 3. 失敗是 Progress 的一部分

在 Discovery-First Progression 裡，

「失敗了七次，第八次突然懂了」

這七次失敗，**是 Progress，不是浪費**。

因為每一次失敗都在縮小玩家的困惑範圍，

直到 Discovery 發生。

---

## 後果

- `progress_manager` 的進度單位是 Factor（理解），而不是 Quest 數量
- Stage 晉級條件支援 `factors`（解鎖特定概念）和 `quests`（完成特定任務）的組合
- 設計師在設計 Adventure 時，應該先問「玩家能在這裡發現什麼」，而不是「玩家應該在這裡完成什麼」
- Reward 作為次要機制存在，但不能成為玩家行動的主要驅動力

---

## 替代方案（被否決）

**方案 A：純 Quest-based Progression**

被否決原因：玩家為任務而行動，Discovery 變成偶然發生的副產品，而不是核心驅動力。

**方案 B：純時間/自由探索 Progression**

被否決原因：沒有任何結構，玩家可能不知道往哪裡走，反而無法觸發 Discovery。

**最終決策**：Discovery 驅動，Quest 輔助結構。兩者不矛盾，但主從關係明確。
