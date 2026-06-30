// /std/item.c (史前文明通用實體物品基底)
#include "/runtime/include/ansi.h"
inherit "/std/object";

private string item_id;
private string name;
private string desc;
private int durability;  // 耐久度或使用次數 (例如火把燃燒時間，石刀使用次數)

void create() {
    ::create();
    name = "未知物品";
    desc = "一個看不清用途的史前物品。";
    durability = -1; // -1 代表無限耐久
}

void set_item_id(string id) { item_id = id; }
string query_item_id()      { return item_id; }

void set_name(string n) { name = n; }
string query_name()     { return name; }

void set_long(string d) { desc = d; }
string query_long()     { return desc; }

void set_durability(int d) { durability = d; }
int query_durability()     { return durability; }

// 減少使用次數/耐久
int use_item(int amount) {
    if (durability == -1) return 1; // 無限使用
    durability -= amount;
    if (durability <= 0) {
        durability = 0;
        return 0; // 物品損壞/燃盡
    }
    return 1;
}

string query_short() {
    if (durability > 0) {
        return name + " (剩餘: " + durability + ")";
    }
    return name;
}
