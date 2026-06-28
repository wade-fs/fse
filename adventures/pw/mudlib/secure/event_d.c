// /secure/event_d.c
// FSE 通用 EventBus (事件總線)
inherit "/std/object";

private nosave mapping listeners;

void create() {
    ::create();
    listeners = ([]);
}

// 訂閱事件
void subscribe(string event_name, string callback_func) {
    if (!listeners) listeners = ([]);
    if (!listeners[event_name]) listeners[event_name] = ({});

    // 儲存呼叫者的物件與對應的回呼函式
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
            catch(call_other(ob, func, ([ "name": event_name, "data": event_data ])));
        }
    }
}
