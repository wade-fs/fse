// /runtime/services/event_bus.c
// FSE 通用核心事件總線 (EventBus)
// 不包含任何特定冒險的業務邏輯

private nosave mapping listeners;

void create() {
    listeners = ([]);
}

// 訂閱事件
void subscribe(string event_name, string callback_func) {
    if (!listeners) listeners = ([]);
    if (!listeners[event_name]) listeners[event_name] = ({});

    // 儲存呼叫者物件與對應的回呼函式
    listeners[event_name] += ({ ({ previous_object(), callback_func }) });
}

// 發送事件
void publish(string event_name, mapping event_data) {
    if (!listeners || !listeners[event_name]) return;

    mixed *list = listeners[event_name];
    foreach (mixed *listener in list) {
        object ob = listener[0];
        string func = listener[1];
        if (ob) {
            safe_call(ob, func, ([ "name": event_name, "data": event_data ]));
        }
    }
}
