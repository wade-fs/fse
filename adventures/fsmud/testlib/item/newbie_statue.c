// /item/newbie_statue.c
inherit "/std/item";

void create() {
    ::create();
    set_name(([ "en": "Hero Statue", "zh-TW": "勇者雕像", "zh-CN": "勇者雕像" ]));
    set_short(([ "en": "A magnificent hero statue", "zh-TW": "一座宏偉的勇者雕像", "zh-CN": "一座宏伟的勇者雕像" ]));
    set_long(([ "en": "This is a statue cast in pure gold, depicting the legendary hero who saved the world.\nThe base is inscribed: 'Courage is the first step on the path to greatness.'\n", "zh-TW": "這是一座純金鑄造的雕像，刻畫著傳說中拯救世界的英雄。\n基座上刻著：『勇氣是通往偉大之路的第一步。』\n", "zh-CN": "这是一座纯金铸造的雕像，刻画著传说中拯救世界的英雄。\n基座上刻著：『勇气是通往伟大之路的第一步。』\n" ]));
    set_id(({"statue", "雕像"}));
    
    set_no_get(1);        // 搬不動
    set_indestructible(1);// 砸不壞
    set_weight(1000000);
}
