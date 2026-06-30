// /include/formosa.h
//
// 《源流福爾摩沙》全域常數、Daemon 路徑、Macro 定義。
// 所有 LPC 物件 #include 此檔。
//
// Canon 參照：docs/mudlib/02_domain_model.md

#ifndef FORMOSA_H
#define FORMOSA_H

// ── Daemon 路徑 ────────────────────────────────────────
#define EVENT_D       load_object("/secure/event_d.c")
#define SETTLEMENT_D  load_object("/services/settlement_d.c")
#define REVEAL_D      load_object("/services/reveal_d.c")
#define FOOTPRINT_D   load_object("/services/footprint_d.c")
#define MEMORY_D      load_object("/services/memory_d.c")
#define TIMELINE_D    load_object("/services/timeline_d.c")
#define CAREER_D      load_object("/services/career_d.c")
#define FACTION_D     load_object("/services/faction_d.c")
#define OBLIVION_D    load_object("/services/oblivion_d.c")
#define RESONANCE_D   load_object("/services/resonance_d.c")
#define CHRONICLE_D   load_object("/services/chronicle_d.c")
#define INCIDENT_D    load_object("/services/incident_d.c")
#define SCHEDULE_D    load_object("/services/schedule_d.c")
#define NPC_D         load_object("/services/npc_d.c")
#define ERA_D         load_object("/services/era_d.c")
#define SITE_D        load_object("/services/site_d.c")
#define ROUTE_D       load_object("/services/route_d.c")
#define EVENT_VIEWER_D load_object("/services/event_viewer_d.c")
#define WORLD_EVENT_D  load_object("/services/world_event_d.c")
#define COMPLETION_D   load_object("/services/completion_d.c")
#define CODEX_D        load_object("/services/codex_d.c")
#define RELATION_D     load_object("/services/relation_d.c")
#define WORLD_STATE_D  load_object("/services/world_state_d.c")

// ── 踏印類型 ───────────────────────────────────────────
#define FP_GEO        "geo"         // 地理踏印
#define FP_RELATION   "relation"    // 人情踏印
#define FP_CRAFT      "craft"       // 技藝踏印
#define FP_SETTLEMENT "settlement"  // 聚落踏印
#define FP_ERA        "era"         // 時代踏印
#define FP_SOURCE     "source"      // 源流踏印

// ── 踏印品質 ───────────────────────────────────────────
#define FQ_GLIMPSE    1   // 初識
#define FQ_FAMILIAR   2   // 熟悉
#define FQ_DEEP       3   // 深入
#define FQ_SOURCE     4   // 源流

// ── 聚落六維 key ───────────────────────────────────────
#define DIM_POPULATION "population"
#define DIM_INDUSTRY   "industry"
#define DIM_CULTURE    "culture"
#define DIM_MEMORY     "memory"
#define DIM_TRADE      "trade"
#define DIM_COHESION   "cohesion"

// ── 聚落等級 ───────────────────────────────────────────
#define TIER_VILLAGE  4
#define TIER_CITY3    3
#define TIER_CITY2    2
#define TIER_CITY1    1

// ── 記憶品質 ───────────────────────────────────────────
#define MQ_FRAGMENT   1   // 殘片
#define MQ_RECORD     2   // 記錄
#define MQ_HERITAGE   3   // 傳承
#define MQ_EPIC       4   // 史詩
#define MQ_SOURCE     5   // 源流

// ── 失源者型態 ─────────────────────────────────────────
#define SP_LOST_NAME    "lost_name"     // 失名者
#define SP_LOST_PLACE   "lost_place"    // 失鄉者
#define SP_LOST_TONGUE  "lost_tongue"   // 失語者
#define SP_LOST_HISTORY "lost_history"  // 失史者

// ── 遺忘閾值 ───────────────────────────────────────────
#define OBLIVION_WARN     30    // 記憶值低於此，預警
#define OBLIVION_SPECTER  20    // 記憶值低於此，生成失源者
#define OBLIVION_CRISIS   10    // 記憶值低於此，危機狀態

// ── Reveal 條件類型 ────────────────────────────────────
#define RC_FOOTPRINT      "has_footprint"
#define RC_FOOTPRINT_MIN  "footprint_min_quality"
#define RC_ERA_DONE       "era_completed"
#define RC_MEMORY_DONE    "memory_completed"
#define RC_CAREER_RANK    "career_rank"
#define RC_SETTLEMENT_MEM "settlement_memory_gte"
#define RC_TALENT         "has_talent"
#define RC_GLOBAL_EVENT   "global_event_triggered"
#define RC_HAS_QUEST      "has_quest"
#define RC_IN_PARTY       "in_party"
#define RC_ANY            "any"   // 任一條件成立即顯示
#define RC_ALL            "all"   // 所有條件成立才顯示

// ── 天賦 ───────────────────────────────────────────────
#define TALENT_ISLAND_MEMORY "island_memory"  // 主角唯一天賦

// ── ANSI 顏色快捷 ──────────────────────────────────────
#define C_RESET   "$NOR$"
#define NOR       "$NOR$"
#define C_TITLE   "$HIW$"
#define C_DIM     "$CYN$"
#define C_WARN    "$HIR$"
#define C_GOOD    "$HIG$"
#define C_MEMORY  "$YEL$"
#define C_HISTORY "$MAG$"
#define C_REVEAL  "$HIB$"
#define C_CODEX   "$HIB$"

// ── 資料路徑 ───────────────────────────────────────────
#define YAML_SETTLEMENTS  "/content/settlements/"
#define YAML_SITES        "/content/sites/"
#define YAML_MEMORIES     "/content/memories/"
#define YAML_ERAS         "/content/eras/"
#define YAML_CAREERS      "/content/careers/"
#define YAML_FACTIONS     "/content/factions/"
#define STATE_SETTLEMENTS "/data/state/settlements/"
#define STATE_SYSTEM      "/data/state/system/"

#endif
