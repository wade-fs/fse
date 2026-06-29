// driver/efun_system.go
package driver

import (
	"fmt"
	"io/fs"
	"os"
	"path/filepath"
	"regexp"
	"runtime"
	"strings"
	"time"

	"github.com/google/uuid"
	"mudscript/lpc-vm/evaluator"
	"mudscript/lpc-vm/object"
)

// ==========================================
// 9. 系統與檔案 (System & Files)
// ==========================================
func (d *Driver) registerSystemAndFiles(obj *object.LPCObject) {
	// 語法: int rm(string file)
	// 說明: 刪除檔案。回傳 1 成功，0 失敗。
	// 範例: rm("/log/old.log");
	obj.Vars.Set("rm", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Integer{Value: 0}
			}
			file, ok := args[0].(*object.String)
			if !ok {
				return &object.Integer{Value: 0}
			}

			resolvedPath := d.ResolvePath(obj.Filename, file.Value)
			allowed, errMsg := d.checkWritePermission(obj, resolvedPath, "rm")
			if !allowed {
				if p := d.GetCurrentPlayer(); p != nil {
					p.Send(fmt.Sprintf("\r\n⚠️ 系統安全攔截：%s\r\n", errMsg))
				}
				return &object.Integer{Value: 0}
			}

			fullPath := d.GetPhysicalPath(resolvedPath)
			err := os.Remove(fullPath)
			if err != nil {
				return &object.Integer{Value: 0}
			}
			return &object.Integer{Value: 1}
		},
	})

	// 語法: int rename(string from, string to)
	// 說明: 移動或重新命名檔案。回傳 1 成功，0 失敗。
	// 範例: rename("/tmp/test", "/data/test");
	obj.Vars.Set("rename", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return &object.Integer{Value: 0}
			}
			from, ok1 := args[0].(*object.String)
			to, ok2 := args[1].(*object.String)
			if !ok1 || !ok2 {
				return &object.Integer{Value: 0}
			}

			resolvedFrom := d.ResolvePath(obj.Filename, from.Value)
			resolvedTo := d.ResolvePath(obj.Filename, to.Value)

			allowed, errMsg := d.checkWritePermission(obj, resolvedFrom, "rename_from")
			if !allowed {
				if p := d.GetCurrentPlayer(); p != nil {
					p.Send(fmt.Sprintf("\r\n⚠️ 來源權限拒絕：%s\r\n", errMsg))
				}
				return &object.Integer{Value: 0}
			}
			allowed, errMsg = d.checkWritePermission(obj, resolvedTo, "rename_to")
			if !allowed {
				if p := d.GetCurrentPlayer(); p != nil {
					p.Send(fmt.Sprintf("\r\n⚠️ 目標權限拒絕：%s\r\n", errMsg))
				}
				return &object.Integer{Value: 0}
			}

			fullFrom := d.GetPhysicalPath(resolvedFrom)
			fullTo := d.GetPhysicalPath(resolvedTo)

			os.MkdirAll(filepath.Dir(fullTo), 0755)
			err := os.Rename(fullFrom, fullTo)
			if err != nil {
				return &object.Integer{Value: 0}
			}
			return &object.Integer{Value: 1}
		},
	})

	// 語法: int mkdir(string path)
	// 說明: 建立目錄。回傳 1 成功，0 失敗。
	// 範例: mkdir("/data/new_dir");
	obj.Vars.Set("mkdir", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Integer{Value: 0}
			}
			path, ok := args[0].(*object.String)
			if !ok {
				return &object.Integer{Value: 0}
			}

			resolvedPath := d.ResolvePath(obj.Filename, path.Value)
			allowed, errMsg := d.checkWritePermission(obj, resolvedPath, "mkdir")
			if !allowed {
				if p := d.GetCurrentPlayer(); p != nil {
					p.Send(fmt.Sprintf("\r\n⚠️ 系統安全攔截：%s\r\n", errMsg))
				}
				return &object.Integer{Value: 0}
			}

			fullPath := d.GetPhysicalPath(resolvedPath)
			err := os.MkdirAll(fullPath, 0755)
			if err != nil {
				return &object.Integer{Value: 0}
			}
			return &object.Integer{Value: 1}
		},
	})

	// 語法: int rmdir(string path)
	// 說明: 移除目錄。回傳 1 成功，0 失敗。
	// 範例: rmdir("/data/old_dir");
	obj.Vars.Set("rmdir", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Integer{Value: 0}
			}
			path, ok := args[0].(*object.String)
			if !ok {
				return &object.Integer{Value: 0}
			}

			resolvedPath := d.ResolvePath(obj.Filename, path.Value)
			allowed, errMsg := d.checkWritePermission(obj, resolvedPath, "rmdir")
			if !allowed {
				if p := d.GetCurrentPlayer(); p != nil {
					p.Send(fmt.Sprintf("\r\n⚠️ 系統安全攔截：%s\r\n", errMsg))
				}
				return &object.Integer{Value: 0}
			}

			fullPath := d.GetPhysicalPath(resolvedPath)
			err := os.Remove(fullPath)
			if err != nil {
				return &object.Integer{Value: 0}
			}
			return &object.Integer{Value: 1}
		},
	})

	// 語法: int cp(string from, string to)
	// 說明: 複製檔案。回傳 1 成功，0 失敗。
	// 範例: cp("/tmp/test", "/data/test");
	obj.Vars.Set("cp", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return &object.Integer{Value: 0}
			}
			from, ok1 := args[0].(*object.String)
			to, ok2 := args[1].(*object.String)
			if !ok1 || !ok2 {
				return &object.Integer{Value: 0}
			}

			resolvedFrom := d.ResolvePath(obj.Filename, from.Value)
			resolvedTo := d.ResolvePath(obj.Filename, to.Value)

			allowed, errMsg := d.checkReadPermission(obj, resolvedFrom, "cp_from")
			if !allowed {
				if p := d.GetCurrentPlayer(); p != nil {
					p.Send(fmt.Sprintf("\r\n⚠️ 來源權限拒絕：%s\r\n", errMsg))
				}
				return &object.Integer{Value: 0}
			}
			allowed, errMsg = d.checkWritePermission(obj, resolvedTo, "cp_to")
			if !allowed {
				if p := d.GetCurrentPlayer(); p != nil {
					p.Send(fmt.Sprintf("\r\n⚠️ 目標權限拒絕：%s\r\n", errMsg))
				}
				return &object.Integer{Value: 0}
			}

			fullFrom := d.GetPhysicalPath(resolvedFrom)
			fullTo := d.GetPhysicalPath(resolvedTo)

			content, err := os.ReadFile(fullFrom)
			if err != nil {
				return &object.Integer{Value: 0}
			}

			os.MkdirAll(filepath.Dir(fullTo), 0755)
			err = os.WriteFile(fullTo, content, 0644)
			if err != nil {
				return &object.Integer{Value: 0}
			}
			return &object.Integer{Value: 1}
		},
	})

	// 語法: string object_name(object ob)
	// 說明: 取得該實體物件的檔案路徑與識別名稱。
	// 範例: write(object_name(this_player()));
	obj.Vars.Set("object_name", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) == 0 {
				return &object.String{Value: obj.Filename}
			}
			if o, ok := args[0].(*object.LPCObject); ok {
				return &object.String{Value: o.Filename}
			}
			return &object.String{Value: ""}
		},
	})

	// 語法: object find_object(string path)
	// 說明: 尋找記憶體中是否已經載入該路徑的藍圖物件。
	// 範例: object room = find_object("/d/city/square");
	obj.Vars.Set("find_object", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) == 0 {
				return &object.Nil{}
			}
			if path, ok := args[0].(*object.String); ok {
				resolvedPath := d.ResolvePath(obj.Filename, path.Value)
				res, err := d.LoadObject(resolvedPath)
				if err == nil {
					return res
				}
			}
			return &object.Nil{}
		},
	})

	// 語法: object *objects()
	// 說明: 回傳目前記憶體中載入的所有物件。
	// 範例: object *obs = objects();
	obj.Vars.Set("objects", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			d.mu.RLock()
			defer d.mu.RUnlock()

			var objs []object.Object
			for _, ob := range d.ObjectTable {
				if ob != nil && !ob.IsDestructed {
					objs = append(objs, ob)
				}
			}
			return &object.Array{Elements: objs}
		},
	})

	// 語法: int query_idle(object ob)
	// 說明: 傳回物件自最後一次活動以來經過的秒數。
	// 範例: write("Idle: " + query_idle(this_player()));
	obj.Vars.Set("query_idle", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := obj
			if len(args) > 0 {
				if t, ok := args[0].(*object.LPCObject); ok {
					target = t
				}
			}
			if target == nil {
				return &object.Integer{Value: 0}
			}
			idle := time.Now().Unix() - target.LastActivity
			return &object.Integer{Value: idle}
		},
	})

	// 語法: object *livings()
	// 說明: 回傳目前記憶體中所有的活物。
	// 範例: object *all_livings = livings();
	obj.Vars.Set("livings", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			d.mu.RLock()
			defer d.mu.RUnlock()
			var res []object.Object
			for _, ob := range d.ObjectTable {
				if ob != nil && ob.IsLiving && !ob.IsDestructed {
					res = append(res, ob)
				}
			}
			return &object.Array{Elements: res}
		},
	})

	// 語法: int pcre_match(string text, string pattern)
	// 說明: 使用正規表示式進行比對。
	// 範例: if (pcre_match(input, "^[a-z]+$")) {}
	obj.Vars.Set("pcre_match", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return &object.Integer{Value: 0}
			}
			text, ok1 := args[0].(*object.String)
			pattern, ok2 := args[1].(*object.String)
			if !ok1 || !ok2 {
				return &object.Integer{Value: 0}
			}

			matched, err := regexp.MatchString(pattern.Value, text.Value)
			if err != nil || !matched {
				return &object.Integer{Value: 0}
			}
			return &object.Integer{Value: 1}
		},
	})

	// 語法: string pcre_replace(string text, string pattern, string replacement)
	// 說明: 使用正規表示式進行替換。
	// 範例: pcre_replace("Hello 123", "[0-9]+", "World") -> "Hello World"
	obj.Vars.Set("pcre_replace", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 3 {
				return args[0]
			}
			text, ok1 := args[0].(*object.String)
			pattern, ok2 := args[1].(*object.String)
			repl, ok3 := args[2].(*object.String)
			if !ok1 || !ok2 || !ok3 {
				return args[0]
			}

			re, err := regexp.Compile(pattern.Value)
			if err != nil {
				return text
			}
			res := re.ReplaceAllString(text.Value, repl.Value)
			return &object.String{Value: res}
		},
	})

	// 語法: string resolve_path(string base, string rel)
	// 說明: 將相對路徑轉換為絕對路徑。
	// 範例: resolve_path("/area/newbie/room_0_0.c", "./room_0_1.c") -> "/area/newbie/room_0_1.c"
	obj.Vars.Set("resolve_path", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			rel, ok := args[0].(*object.String)
			if !ok {
				return &object.Nil{}
			}

			base := obj.Filename
			if len(args) > 1 {
				if b, ok := args[1].(*object.String); ok {
					base = b.Value
				}
			}

			return &object.String{Value: d.ResolvePath(base, rel.Value)}
		},
	})

	// 語法: int write_file(string file, string text, [int overwrite])
	// 說明: 將文字寫入實體硬碟的檔案中。
	// 範例: write_file("/log/debug.log", "發生錯誤\n");
	obj.Vars.Set("write_file", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return &object.Integer{Value: 0}
			}
			file, ok1 := args[0].(*object.String)
			text, ok2 := args[1].(*object.String)
			if !ok1 || !ok2 {
				return &object.Integer{Value: 0}
			}

			resolvedPath := d.ResolvePath(obj.Filename, file.Value)
			allowed, errMsg := d.checkWritePermission(obj, resolvedPath, "write_file")
			if !allowed {
				if p := d.GetCurrentPlayer(); p != nil {
					p.Send(fmt.Sprintf("\r\n⚠️ 系統安全攔截：%s\r\n", errMsg))
				} else {
					fmt.Printf("🚫 寫入拒絕: %s\n", errMsg)
				}
				return &object.Integer{Value: 0}
			}

			flag := os.O_APPEND | os.O_CREATE | os.O_WRONLY
			if len(args) > 2 {
				if i, ok := args[2].(*object.Integer); ok && i.Value == 1 {
					flag = os.O_TRUNC | os.O_CREATE | os.O_WRONLY
				}
			}

			fullPath := d.GetPhysicalPath(resolvedPath)
			os.MkdirAll(filepath.Dir(fullPath), 0755)
			f, err := os.OpenFile(fullPath, flag, 0644)
			if err != nil {
				return &object.Integer{Value: 0}
			}
			defer f.Close()

			f.WriteString(text.Value)
			return &object.Integer{Value: 1}
		},
	})

	// 語法: object load_object(string file)
	// 說明: 載入並回傳指定路徑的藍圖物件。
	// 範例: object room = load_object("/d/city/square");
	obj.Vars.Set("load_object", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return evaluator.NilValue
			}
			pathObj, ok := args[0].(*object.String)
			if !ok || pathObj.Value == "" || pathObj.Value == "/" {
				return evaluator.NilValue
			}
			path := pathObj.Value

			resolvedPath := d.ResolvePath(obj.Filename, path)
			if !strings.HasSuffix(resolvedPath, ".c") {
				resolvedPath += ".c"
			}
			res, err := d.LoadObject(resolvedPath)
			if err != nil {
				return object.NewError("%s", err.Error())
			}
			return res
		},
	})

	// 語法: string read_file(string file, [int start, int lines])
	// 說明: 讀取並回傳檔案的文字內容。可以指定起始行號和讀取行數。
	// 範例: string issue = read_file(ISSUE_FILE, 1, 10);
	obj.Vars.Set("read_file", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			fileName, ok := args[0].(*object.String)
			if !ok {
				return object.NewError("read_file 需要字串參數")
			}

			resolvedPath := d.ResolvePath(obj.Filename, fileName.Value)
			allowed, errMsg := d.checkReadPermission(obj, resolvedPath, "read_file")
			if !allowed {
				if p := d.GetCurrentPlayer(); p != nil {
					p.Send(fmt.Sprintf("\r\n⚠️ 系統安全攔截：%s\r\n", errMsg))
				}
				return &object.Nil{}
			}

			content, err := d.ReadFile(resolvedPath)
			if err != nil {
				return &object.Nil{}
			}

			// 處理可選的 start 和 lines 參數
			startLine := 1
			lineCount := -1

			if len(args) > 1 {
				if start, ok := args[1].(*object.Integer); ok {
					startLine = int(start.Value)
				}
			}
			if len(args) > 2 {
				if lines, ok := args[2].(*object.Integer); ok {
					lineCount = int(lines.Value)
				}
			}

			if startLine > 1 || lineCount >= 0 {
				lines := strings.Split(string(content), "\n")
				
				// 調整 startLine (1-based)
				startIdx := startLine - 1
				if startIdx < 0 { startIdx = 0 }
				if startIdx >= len(lines) { return &object.String{Value: ""} }

				endIdx := len(lines)
				if lineCount >= 0 {
					endIdx = startIdx + lineCount
					if endIdx > len(lines) { endIdx = len(lines) }
				}

				return &object.String{Value: strings.Join(lines[startIdx:endIdx], "\n") + "\n"}
			}

			return &object.String{Value: string(content)}
		},
	})

	// 語法: int file_size(string file)
	// 說明: 取得檔案大小。若不存在回傳 -1，若為目錄回傳 -2。
	// 範例: if (file_size("/data/user/wade.o") > 0) { ... }
	obj.Vars.Set("file_size", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Integer{Value: -1}
			}
			fileName, ok := args[0].(*object.String)
			if !ok {
				return &object.Integer{Value: -1}
			}

			resolvedPath := d.ResolvePath(obj.Filename, fileName.Value)
			fullPath := d.GetPhysicalPath(resolvedPath)
			if info, err := os.Stat(fullPath); err == nil {
				if info.IsDir() {
					return &object.Integer{Value: -2}
				}
				return &object.Integer{Value: info.Size()}
			}
			if d.Config.EmbeddedFS != nil {
				embedPath := filepath.Join("mudlib", strings.TrimPrefix(resolvedPath, "/"))
				if info, err := fs.Stat(d.Config.EmbeddedFS, embedPath); err == nil {
					if info.IsDir() {
						return &object.Integer{Value: -2}
					}
					return &object.Integer{Value: info.Size()}
				}
			}
			return &object.Integer{Value: -1}
		},
	})

	// 語法: object *users()
	// 說明: 回傳目前線上所有玩家的實體物件陣列。
	// 範例: object *onlines = users();
	obj.Vars.Set("users", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			var userObjs []object.Object
			d.interactiveObjects.Range(func(key, value interface{}) bool {
				if conn, ok := value.(*PlayerConnection); ok && conn.IsActive {
					userObjs = append(userObjs, conn.Object)
				}
				return true
			})
			return &object.Array{Elements: userObjs}
		},
	})

	// 語法: string mud_status()
	// 說明: 回傳 MUD 伺服器的運行狀態資訊。
	// 範例: write(mud_status());
	obj.Vars.Set("mud_status", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			d.mu.RLock()
			objCount := len(d.ObjectTable)
			hbCount := len(d.Heartbeats)
			coCount := len(d.CallOuts)
			d.mu.RUnlock()
			var stats runtime.MemStats
			runtime.ReadMemStats(&stats)
			res := fmt.Sprintf("MUD Status:\n  Objects: %d\n  Heartbeats: %d\n  Pending CallOuts: %d\n  Goroutines: %d\n  Allocated Memory: %v MB\n",
				objCount, hbCount, coCount, runtime.NumGoroutine(), stats.Alloc/1024/1024)
			return &object.String{Value: res}
		},
	})

	// 語法: mapping memory_summary()
	// 說明: 回傳記憶體使用狀況摘要。
	// 範例: mapping mem = memory_summary();
	obj.Vars.Set("memory_summary", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			var stats runtime.MemStats
			runtime.ReadMemStats(&stats)
			m := &object.Mapping{Pairs: make(map[object.HashKey]object.HashPair)}
			set := func(k string, v int64) {
				ks := &object.String{Value: k}
				vs := &object.Integer{Value: v}
				m.Pairs[ks.HashKey()] = object.HashPair{Key: ks, Value: vs}
			}
			set("alloc", int64(stats.Alloc))
			set("total_alloc", int64(stats.TotalAlloc))
			set("sys", int64(stats.Sys))
			set("num_gc", int64(stats.NumGC))
			set("objects", int64(stats.HeapObjects))
			return m
		},
	})

	// 語法: void replace_program(string path)
	// 說明: 將當前物件的所有函式替換為指定物件的函式。
	// 範例: replace_program("/std/room");
	obj.Vars.Set("replace_program", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return evaluator.NilValue
			}
			path, ok := args[0].(*object.String)
			if !ok {
				return evaluator.NilValue
			}
			resolved := d.ResolvePath(obj.Filename, path.Value)
			target, err := d.LoadObject(resolved)
			if err != nil {
				return object.NewError("replace_program error: %s", err.Error())
			}
			obj.Functions = target.Functions
			obj.Inherits = target.Inherits
			return evaluator.NilValue
		},
	})

	// 語法: void input_to(mixed func, [int flag, ...args])
	// 說明: 設定玩家下一個輸入將會傳遞給指定的函式處理。
	// func 可以是字串 (函式名稱) 或 閉包 (Closure)。
	// 範例: input_to("check_password", 1);
	// 範例: input_to((: get_id :), 0, ob, 1);
	obj.Vars.Set("input_to", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return object.NewError("input_to 需要函式名稱或閉包作為參數")
			}
			p := d.GetCurrentPlayer()
			if p == nil && obj.IsInteractive {
				p = d.GetConnectionFromObject(obj)
			}
			if p == nil {
				return &object.Integer{Value: 0}
			}

			// 處理第一個參數 (字串或閉包)
			switch fn := args[0].(type) {
			case *object.String:
				p.NextInputFunc = fn.Value
				p.NextInputClosure = nil
				p.NextInputObj = obj // 🚀 關鍵修正：記錄發起呼叫的物件
			case *object.Closure:
				p.NextInputFunc = ""
				p.NextInputClosure = fn
				p.NextInputObj = nil // 閉包本身包含物件資訊
			default:
				return object.NewError("input_to 第一個參數必須是字串或閉包")
			}

			p.InputHidden = false
			// 處理其餘參數
			p.NextInputArgs = nil
			if len(args) > 1 {
				// 第二個參數如果是整數且不為 0，則隱藏輸入
				if flag, ok := args[1].(*object.Integer); ok && flag.Value != 0 {
					p.InputHidden = true
					p.Send("__INPUT_HIDDEN__")
				}
				// 第三個參數開始為傳遞給 callback 的額外參數
				if len(args) > 2 {
					p.NextInputArgs = args[2:]
				}
			}

			return &object.Integer{Value: 1}
		},
	})

	// 語法: void shutdown([int exit_code])
	// 說明: 關閉 MUD 伺服器並結束進程。
	// 範例: shutdown();
	obj.Vars.Set("shutdown", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			exitCode := 0
			if len(args) > 0 {
				if i, ok := args[0].(*object.Integer); ok {
					exitCode = int(i.Value)
				}
			}
			fmt.Printf("🛑 收到關閉指令 (Code: %d)，伺服器準備關閉...\n", exitCode)
			go func() { time.Sleep(500 * time.Millisecond); os.Exit(exitCode) }()
			return &object.Nil{}
		},
	})

	// 語法: string getenv(string var)
	// 說明: 取得系統環境變數。
	// 範例: if (getenv("MUD_TEST_MODE")) { ... }
	obj.Vars.Set("getenv", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			varName, ok := args[0].(*object.String)
			if !ok {
				return &object.Nil{}
			}
			val := os.Getenv(varName.Value)
			if val == "" {
				return &object.Nil{}
			}
			return &object.String{Value: val}
		},
	})

	// 語法: int link(string from, string to)
	obj.Vars.Set("link", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return &object.Integer{Value: 0}
			}
			from, ok1 := args[0].(*object.String)
			to, ok2 := args[1].(*object.String)
			if !ok1 || !ok2 {
				return &object.Integer{Value: 0}
			}

			resolvedFrom := d.ResolvePath(obj.Filename, from.Value)
			resolvedTo := d.ResolvePath(obj.Filename, to.Value)

			allowed, _ := d.checkReadPermission(obj, resolvedFrom, "link_from")
			if !allowed { return &object.Integer{Value: 0} }
			allowed, _ = d.checkWritePermission(obj, resolvedTo, "link_to")
			if !allowed { return &object.Integer{Value: 0} }

			fullFrom := d.GetPhysicalPath(resolvedFrom)
			fullTo := d.GetPhysicalPath(resolvedTo)

			err := os.Link(fullFrom, fullTo)
			if err != nil {
				return &object.Integer{Value: 0}
			}
			return &object.Integer{Value: 1}
		},
	})

	// 語法: string generate_uuid()
	// 說明: 產生一個隨機的 UUID。
	// 範例: string id = generate_uuid();
	obj.Vars.Set("generate_uuid", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.String{Value: uuid.New().String()}
		},
	})
}

func (d *Driver) registerMonitorEfuns(obj *object.LPCObject) {
	// 語法: mapping rusage()
	obj.Vars.Set("rusage", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			var stats runtime.MemStats
			runtime.ReadMemStats(&stats)
			m := &object.Mapping{Pairs: make(map[object.HashKey]object.HashPair)}
			set := func(k string, v int64) {
				ks := &object.String{Value: k}
				vs := &object.Integer{Value: v}
				m.Pairs[ks.HashKey()] = object.HashPair{Key: ks, Value: vs}
			}
			set("utime", time.Now().Unix()) // Placeholder for user time
			set("stime", time.Now().Unix()) // Placeholder for system time
			set("maxrss", int64(stats.Alloc))
			return m
		},
	})

	// 語法: int clonep(object ob)
	obj.Vars.Set("clonep", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := getTarget(args, obj)
			if strings.Contains(target.Filename, "#") {
				return &object.Integer{Value: 1}
			}
			return &object.Integer{Value: 0}
		},
	})

	// 語法: object *children(string path)
	obj.Vars.Set("children", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Array{Elements: []object.Object{}}
			}
			path, ok := args[0].(*object.String)
			if !ok {
				return &object.Array{Elements: []object.Object{}}
			}

			resolved := d.ResolvePath(obj.Filename, path.Value)
			if !strings.HasSuffix(resolved, ".c") {
				resolved += ".c"
			}

			d.mu.RLock()
			defer d.mu.RUnlock()

			var elements []object.Object
			for _, ob := range d.ObjectTable {
				if ob != nil && !ob.IsDestructed && strings.HasPrefix(ob.Filename, resolved) {
					elements = append(elements, ob)
				}
			}
			return &object.Array{Elements: elements}
		},
	})

	// 語法: void reclaim_objects()
	obj.Vars.Set("reclaim_objects", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			runtime.GC()
			return &object.Nil{}
		},
	})

	// 語法: mixed stat(string path)
	obj.Vars.Set("stat", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			path, ok := args[0].(*object.String)
			if !ok {
				return &object.Nil{}
			}

			resolved := d.ResolvePath(obj.Filename, path.Value)
			fullPath := d.GetPhysicalPath(resolved)
			info, err := os.Stat(fullPath)
			if err != nil {
				return &object.Nil{}
			}

			// 回傳格式: ({ size, mtime, is_dir })
			isDir := 0
			if info.IsDir() {
				isDir = 1
			}
			return &object.Array{Elements: []object.Object{
				&object.Integer{Value: info.Size()},
				&object.Integer{Value: info.ModTime().Unix()},
				&object.Integer{Value: int64(isDir)},
			}}
		},
	})

	// 語法: void tail(string path)
	obj.Vars.Set("tail", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			path, ok := args[0].(*object.String)
			if !ok {
				return &object.Nil{}
			}

			resolved := d.ResolvePath(obj.Filename, path.Value)
			content, err := d.ReadFile(resolved)
			if err != nil {
				return &object.Nil{}
			}

			lines := strings.Split(string(content), "\n")
			start := len(lines) - 10
			if start < 0 {
				start = 0
			}
			d.TellObject(obj, strings.Join(lines[start:], "\n"))
			return &object.Nil{}
		},
	})

	// 語法: void set_debug_level(int level)
	obj.Vars.Set("set_debug_level", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.Nil{}
		},
	})
}
