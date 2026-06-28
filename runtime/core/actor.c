// /runtime/core/actor.c
// FSE 通用 Actor 基底 (Actor Base)
// 代表具備自主行為、狀態與互動能力的個體 (如 NPC、Monster、Agent)
inherit "/runtime/core/entity";

private mapping attributes;

void create() {
    ::create();
    set_entity_type("actor");
    attributes = ([]);
}

void set_attribute(string key, mixed val) { attributes[key] = val; }
mixed query_attribute(string key) { return attributes[key]; }
