// /runtime/services/color_service.c
// FSE 核心顏色與字元格式化服務
#include "/runtime/include/ansi.h"

private nosave mapping ansi_map = ([
    "$NOR$": "\x1b[0m",
    "$BLK$": "\x1b[30m",
    "$RED$": "\x1b[31m",
    "$GRN$": "\x1b[32$N$", // Wait, no, just normal escape
    "$GRN$": "\x1b[32m",
    "$YEL$": "\x1b[33m",
    "$BLU$": "\x1b[34m",
    "$MAG$": "\x1b[35m",
    "$CYN$": "\x1b[36m",
    "$WHT$": "\x1b[37m",
    "$GRA$": "\x1b[1;30m",
    "$HIK$": "\x1b[1;30m",
    "$HIR$": "\x1b[1;31m",
    "$HIG$": "\x1b[1;32m",
    "$HIY$": "\x1b[1;33m",
    "$HIB$": "\x1b[1;34m",
    "$HIM$": "\x1b[1;35m",
    "$HIC$": "\x1b[1;36m",
    "$HIW$": "\x1b[1;37m",
]);

void create() {}

// 解析文字中的顏色標記，轉換為 ANSI 控制碼 (適用於終端輸出)
string to_ansi(string text) {
    if (!text || !stringp(text)) return text;
    foreach (string tag, string esc in ansi_map) {
        text = replace_string(text, tag, esc);
    }
    return text;
}

// 移除文字中所有的顏色標記 (過濾乾淨文字)
string strip_color(string text) {
    if (!text || !stringp(text)) return text;
    foreach (string tag, string esc in ansi_map) {
        text = replace_string(text, tag, "");
    }
    // 同時移除可能殘留的 ANSI 控制碼
    foreach (string tag, string esc in ansi_map) {
        text = replace_string(text, esc, "");
    }
    return text;
}

// 動態解析，可依據客戶端類型 (如 "web" 或 "telnet") 自動決定如何呈現顏色
string parse_color(string text, string client_type) {
    if (client_type == "telnet" || client_type == "terminal") {
        return to_ansi(text);
    }
    // 預設為 "web" 模式：保留原始 $TAG$ 標記讓瀏覽器前端進行高效的 CSS 色彩解析
    return text;
}
