// /fse/std/entity.c
// FSE 核心最基本實體基底
inherit "/fse/std/object";

private string entity_id;
private string entity_type;

void create() {
    ::create();
}

void set_entity_id(string id) { entity_id = id; }
string query_entity_id() { return entity_id; }

void set_entity_type(string type) { entity_type = type; }
string query_entity_type() { return entity_type; }
