// /std/site.c
// FSE 空間地標基底 (支援 Reveal Layer)
inherit "/std/entity";
inherit "/std/reveal_layer";

void create() {
    ::create();
    set_entity_type("site");
}
