# ADR-0001 - Bootstrap FSE Before Importing Formosa

**Status:** Accepted **Date:** 2026-06-29

## Context

FSE 已逐步從 fsmud 抽象化成獨立 Runtime。

-   Runtime 已具備 World、Node、Actor、Discovery、Progress、Event
    等核心概念。
-   Programming World (PW) 是第一個 Adventure。
-   Formosa（原 fsmud）暫時移出 Repository。

因此專案已從「重構 MUD」轉變為「建立 Adventure Runtime」。

## Decision

新的開發順序：

1.  Runtime Stabilization : The point where new Adventures require new content, not new concepts.
2.  PW Validation
3.  Runtime Freeze
4.  Import Formosa
5.  Future Adventures

## Runtime Stabilization

完成並穩定：

-   World
-   Node
-   Actor
-   Factor
-   Discovery
-   Progress
-   EventBus

Runtime 不依賴任何 Adventure。

## PW Validation

PW 的角色不是教材，而是 FSE Runtime 的驗證器。

驗證：

-   Discovery Loop
-   Mapping
-   Runtime API
-   YAML Content
-   Input Medium

完成一條完整流程：

World → Action → Consequence → Discovery → Presentation → New Path

## Runtime Freeze

Runtime API 穩定後，不再因任何 Adventure 修改核心。

Adventure 只提供 Mapping 與 Content。

## Import Formosa

將 Formosa 視為第一個大型 Adventure，而不是 Runtime 的來源。

以 Mapping 導入：

-   Settlement → Node
-   Site → Node
-   Memory → Discovery
-   Era → World State

## Vision

不要先教玩家知識。

先建立一個值得探索的世界。

玩家先得到：

-   好奇
-   探索
-   發現

理解會在探索中自然形成。

> Don't teach first. Build a world worth exploring first.
