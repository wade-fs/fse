# Appendix B. LPC Coding Style Guide - LPC 程式撰寫規範

本規範定義了 FormosaSaga 專案中所有 LPC (`.c`) 原始碼的撰寫慣例，目標是確保：
- 與本專案 Golang 自研 LPC 直譯器 (fsmud) 100% 相容
- 所有協作開發者（人類或 AI）撰寫的程式碼風格一致
- 避免使用「現代方言 LPC」語法造成 parse 錯誤

> [!IMPORTANT]
> 本直譯器的語法基礎為 **MudOS v22 傳統 LPC**，不支援 FluffOS 或現代 LPC 方言中的部分擴展語法。

---

## 🔢 型別與陣列宣告 (Type & Array Declarations)

### ❌ 禁用：後綴式陣列語法 (`type[]`)

本直譯器**不支援**以下後綴式陣列型別宣告（屬 PHP/C#/TypeScript 風格，非傳統 LPC）：

```c
// ❌ 不要使用 — 將導致 PARSER ERROR
string[] files = get_dir(dir);
int[]    scores = ({ 1, 2, 3 });
object[] npcs   = ({});
mapping[] rows  = ({});
mixed[]  args   = ({});
```

### ✅ 正確：前綴星號式陣列語法 (`type *`) 或 `mixed`

傳統 LPC 使用 **C-style 指標星號** 來宣告陣列型別：

```c
// ✅ 正確 — 型別明確，適合「已知內容皆為該型別」的陣列
string *files   = get_dir(dir);
int    *scores  = ({ 1, 2, 3 });
object *npcs    = ({});
mixed  *args    = ({});

// ✅ 也正確 — mixed 適合「內容型別不固定」或「來自外部 API 的陣列」
mixed files   = get_dir(dir);  // get_dir 可能回傳混合型別
mixed results = query_something();
```

### 👉 決策原則

| 情境 | 建議寫法 |
|:---|:---|
| 內容一定是字串（如自己建構的清單） | `string *` |
| 來自系統 API（如 `get_dir`、`keys`、`all_inventory`） | `mixed`（因為型別無法保證） |
| 函數參數不確定型別 | `mixed` |
| 回傳值宣告為陣列 | `mixed *` 或 `string *`（看情境） |
| foreach 迭代變數 | 直接用 `string`、`object` 等不加 `*` |

---

## 📦 其他常見型別規範

### 函數回傳型別

```c
// ✅ 正確
string  query_name()    { return name; }
int     query_level()   { return level; }
mapping query_data()    { return data; }
void    do_something()  { ... }
mixed   query_mixed()   { return anything; }

// ✅ 回傳陣列的函數
mixed  *query_list()    { return list; }
string *query_names()   { return names; }
```

### 映射 (Mapping) 宣告

```c
// ✅ 正確
mapping data = ([]);
private mapping npc_configs = ([]);

// ❌ 不支援泛型語法（非 LPC 標準）
// mapping<string, int> scores = ([]);   // 禁用
```

### foreach 寫法

```c
// ✅ 正確 — 型別宣告在括號內
foreach (string file in files) { ... }
foreach (object ob in all_inventory(env)) { ... }
foreach (string key, mixed val in data) { ... }

// ❌ 不要用 C-style for loop 模擬 foreach（可讀性差）
// for (int i = 0; i < sizeof(files); i++) { ... }  // 僅在需要 index 時才用
```

---

## 📏 命名慣例 (Naming Conventions)

| 對象 | 規範 | 範例 |
|:---|:---|:---|
| 檔案名稱（daemon） | `snake_case_d.c` | `schedule_d.c`, `npc_d.c` |
| 檔案名稱（std） | `snake_case.c` | `npc.c`, `site.c` |
| 函數名稱 | `snake_case` | `query_npc_name()` |
| 全域變數 | `snake_case` | `npc_configs` |
| 私有變數 | 加 `private` 修飾 | `private mapping npc_configs` |
| 常數（define） | `ALL_CAPS` | `BEHAV_PATROL`, `DIM_MEMORY` |
| NPC ID（YAML）| `NPC_` 前綴 + `snake_case` | `NPC_old_stationmaster_minxiong` |
| Site ID（YAML） | `snake_case`（聚落前綴） | `minxiong_old_station` |

---

## 📂 #include 規範

```c
// ✅ 所有 mudlib 程式碼的標準 include（包含所有 daemon define 與常數）
#include "/include/formosa.h"

// ✅ 若需要引用 config 常數（如 STAT_STR、BEHAV_PATROL）
#include "/include/config.h"

// ❌ 不要相對路徑 include
// #include "formosa.h"    // 禁用
// #include "../include/formosa.h"  // 禁用
```

---

## 🛡️ 防禦性程式碼慣例

```c
// ✅ 呼叫外部 daemon 或物件前，先確認非 null
if (!data || !data["id"]) return;
if (!arrayp(files) || sizeof(files) == 0) return;

// ✅ 存取可能不存在的 mapping key，用 || 給預設值
mapping quests = player->query("quests") || ([]);
mixed *list    = player->query("some_list") || ({});

// ✅ 在 catch 中呼叫可能失敗的跨物件操作
catch(npc_ob->move(dest));
catch(INCIDENT_D->check_trigger(player, "site_look", id, nil));
```

---

## 🧪 測試規範

每個新增的 daemon 或重要系統，應於 `mudlib/tests/` 建立對應的整合測試：

```
test_{system_name}.c
```

例如：`test_npc_schedule.c`、`test_quest_loop.c`。

執行方式：
```bash
make docker-test-fsmud TEST_FILE=test_npc_schedule.c
```
