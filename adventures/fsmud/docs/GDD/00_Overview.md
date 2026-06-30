# 00. Overview - 《源流福爾摩沙》遊戲企劃與設計總綱 (Overview)

## 📌 作品定位與世界觀
《源流福爾摩沙》 (Formosa Saga) 是一款以台灣歷史、文化、地方風土為內核的現代文字化身角色扮演遊戲 (MUD)。

*   **副標**：每一枚踏印，都是島嶼留下的記憶。
*   **宗旨**：這是一部介紹台灣的遊戲，不是歷史課本，也不是旅遊書，而是一個關於「人如何在土地上留下痕跡」的世界模擬。
*   **核心載體**：基於 Golang 自研的 MudOS v22 模擬直譯器與現代 LPC MUD 框架，完全打通資料（YAML 驅動）與邏輯的分離。

---

## 🏛️ 三大開發支柱 (Three Pillars)
1.  **以「踏印」取代等級 (Footprints over Levels)**：本作無傳統 RPG 數值無限膨脹的等級鎖鏈，成長指針為地理、人情、技藝、聚落、時代、源流六大踏印。
2.  **記憶是第一類狀態 (Memory as First-Class State)**：聚落歷史記憶（Memory %）是影響世界運行的核心變數，決定了聚落的興衰與危機的爆發。
3.  **時間層疊的空間網格 (Time-Layered Space)**：同一個地理坐標在不同歷史時代擁有不同的歷史層空間。

---

## 📂 GDD 文件結構目錄
本設計文檔（Game Design Document）旨在為後續開發者提供最嚴謹、最具擴展性的開發規範：
*   **[00_Overview.md](file:///home/wade/src/github/FormosaSaga/docs/Game_Design_Document/00_Overview.md)**：作品定位、核心願景與設計總綱。
*   **[01_Core_Loop.md](file:///home/wade/src/github/FormosaSaga/docs/Game_Design_Document/01_Core_Loop.md)**：世界核心運作循環與基礎指令對應。
*   **[02_World_Model.md](file:///home/wade/src/github/FormosaSaga/docs/Game_Design_Document/02_World_Model.md)**：聚落 (Settlement) 與地標 (Site) 的架構模型。
*   **[03_Era_System.md](file:///home/wade/src/github/FormosaSaga/docs/Game_Design_Document/03_Era_System.md)**：歷史時代推展與 `EraShifted` 機制。
*   **[04_Site_System.md](file:///home/wade/src/github/FormosaSaga/docs/Game_Design_Document/04_Site_System.md)**：YAML 驅動地標與動態 Reveal 浮現層。
*   **[05_Memory_System.md](file:///home/wade/src/github/FormosaSaga/docs/Game_Design_Document/05_Memory_System.md)**：歷史記憶碎片解鎖與共鳴。
*   **[06_Quest_System.md](file:///home/wade/src/github/FormosaSaga/docs/Game_Design_Document/06_Quest_System.md)**：自訂引導與勢力委託任務鏈。
*   **[07_NPC_System.md](file:///home/wade/src/github/FormosaSaga/docs/Game_Design_Document/07_NPC_System.md)**：NPC 屬性範本與多時代對話機制。
*   **[08_Faction_System.md](file:///home/wade/src/github/FormosaSaga/docs/Game_Design_Document/08_Faction_System.md)**：勢力聲望與影響力系統。
*   **[09_Career_System.md](file:///home/wade/src/github/FormosaSaga/docs/Game_Design_Document/09_Career_System.md)**：農、商、匠、文四大職涯與專屬技能。
*   **[10_Specter_System.md](file:///home/wade/src/github/FormosaSaga/docs/Game_Design_Document/10_Specter_System.md)**：失源者危機與遺忘浪潮機制。
*   **[11_Historical_Incident_System.md](file:///home/wade/src/github/FormosaSaga/docs/Game_Design_Document/11_Historical_Incident_System.md)**：歷史事件動態編年史與地方誌。
*   **[12_Volume1_Content.md](file:///home/wade/src/github/FormosaSaga/docs/Game_Design_Document/12_Volume1_Content.md)**：小說卷一主線地理網絡（民雄至彰化）實作現狀。
*   **[13_Content_Pipeline.md](file:///home/wade/src/github/FormosaSaga/docs/Game_Design_Document/13_Content_Pipeline.md)**：地景與記憶碎片擴展的流程指南。
*   **[Appendix_YAML_Style_Guide.md](file:///home/wade/src/github/FormosaSaga/docs/Game_Design_Document/Appendix_YAML_Style_Guide.md)**：YAML 欄位格式與靜動態分離開發規範。
*   **[Appendix_B_LPC_Style_Guide.md](file:///home/wade/src/github/FormosaSaga/docs/Game_Design_Document/Appendix_B_LPC_Style_Guide.md)**：LPC 程式撰寫規範（型別、陣列語法、命名慣例、測試）。
