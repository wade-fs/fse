// /item/treasure_chest.c
inherit "/std/container";

void create() {
    ::create();
    set_name(([ "en": "Heavy Wooden Chest", "zh-TW": "沉重的木箱", "zh-CN": "沉重的木箱" ]));
    set_short(([ "en": "A heavy wooden chest", "zh-TW": "一個沉重的木箱", "zh-CN": "一个沉重的木箱" ]));
    set_long(([ "en": "This is an old wooden chest, covered in moss, seemingly forgotten for a long time.\n", "zh-TW": "這是一個老舊的木箱，上面長滿了青苔，似乎已經被遺忘很久了。\n", "zh-CN": "这是一个老旧的木箱，上面长满了青苔，似乎已经被遗忘很久了。\n" ]));
    set_id(({"chest", "box", "木箱", "箱子"}));
    
    set_max_weight(5000);
    set_max_items(5);
    set_can_open(1); // 可以開啟
    set_closed(1);   // 預設關閉
    set_no_get(1);   // 太重了，拿不動

    // 放入一些獎勵
    object gold = clone_object("/std/object.c"); // 假設有金幣物件，暫用 object 代替
    if (gold) {
        gold->set_name(([ "en": "Gold Coins", "zh-TW": "金幣", "zh-CN": "金币" ]));
        gold->set_short(([ "en": "A pile of shining gold coins", "zh-TW": "一堆閃閃發光的金幣", "zh-CN": "一堆闪闪发光的金币" ]));
        gold->set_id(({"gold", "money", "金幣"}));
        move_object(gold, this_object());
    }
}

// 覆寫 Inspect 顯示狀態
string query_short() {
    return ::query_short() + (query_closed() ? " (關閉)" : " (開啟)");
}
