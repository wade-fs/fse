// /fse/std/node.c
// FSE 核心通用節點基底 (Node Base)
inherit "/fse/std/entity";
inherit "/std/reveal_layer.c";

void create() {
    ::create();
    set_entity_type("node");
}
