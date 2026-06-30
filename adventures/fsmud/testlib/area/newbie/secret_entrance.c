// /area/newbie/secret_entrance.c
inherit "/std/room";

void create() {
    ::create();
    set_short("幽暗的山腳下");
    set_long("這裡是一處偏僻的山腳，四周雜草叢生，亂石堆疊。\n" +
             "北邊是一座高聳入雲的山峰，看起來無法攀爬。\n");
    
    set_coordinate(10, 10, 0);
    
    add_exit("south", "./room_0_0.c"); // 假設連接回起點
    
    // 加入隱藏出口
    add_hidden_element("north", ([
        "type": "exit",
        "value": "./hidden_cave.c",
        "msg": "你撥開了一叢茂密的雜草，發現後方竟然隱藏著一個狹窄的山洞入口！"
    ]));

    add_item("雜草", "這些雜草長得異常高大，似乎掩蓋了什麼。");
    add_item("亂石", "堆積如山的亂石，沒什麼特別的。");
}
