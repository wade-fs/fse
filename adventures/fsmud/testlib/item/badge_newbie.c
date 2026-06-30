// /item/badge_newbie.c
inherit "/std/badge";

void create() {
    ::create();
    set_name(([ "en": "Newbie Badge", "zh-TW": "新手證明徽章", "zh-CN": "新手证明徽章" ]));
    set_long(([ "en": "This is a badge issued by the Adventurer's Guild, symbolizing your first step into adventure.\n", "zh-TW": "這是一枚由冒險者公會頒發的徽章，象徵著你邁出了冒險的第一步。\n", "zh-CN": "这是一枚由冒险者公会颁发的徽章，象征著你迈出了冒险的第一步。\n" ]));
    set_id(({"badge", "newbie badge", "徽章"}));
}
