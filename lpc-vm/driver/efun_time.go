// driver/efun_time.go
package driver

import (
	"strings"
	"time"

	"mudscript/lpc-vm/object"
)

func (d *Driver) registerTimeAndScheduling(obj *object.LPCObject) {
	// 語法: int time()
	// 說明: 回傳目前的 Unix 時間戳 (從 1970 年開始的秒數)。
	// 範例: int t = time();
	obj.Vars.Set("time", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.Integer{Value: time.Now().Unix()}
		},
	})

	// 語法: void sleep(int ms)
	// 說明: 暫停執行指定的毫秒數。
	// 範例: sleep(3000); // 暫停 3 秒
	obj.Vars.Set("sleep", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			if ms, ok := args[0].(*object.Integer); ok {
				return &object.AsyncPause{Duration: time.Duration(ms.Value) * time.Millisecond}
			}
			return &object.Nil{}
		},
	})

	// 語法: void call_out(mixed func, int delay, [mixed args...])
	// 說明: 延遲 delay 秒後，自動呼叫 func 函式 (字串或閉包)，並可傳入參數。
	// 範例: call_out("destroy_self", 5); // 5秒後呼叫 destroy_self()
	// 範例: call_out((: destruct :), 5, this_object());
	obj.Vars.Set("call_out", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return object.NewError("call_out 至少需要 2 個參數")
			}

			delay, ok := args[1].(*object.Integer)
			if !ok {
				return object.NewError("call_out 第二個參數必須是整數")
			}

			// 處理第一個參數 (字串或閉包)
			switch fn := args[0].(type) {
			case *object.String:
				d.CallOut(obj, fn.Value, time.Duration(delay.Value)*time.Second, args[2:]...)
			case *object.Closure:
				// 🚀 關鍵強化：支援閉包呼叫
				// 我們將閉包包裝成一個匿名的 Builtin 函式存入，或者擴充 ScheduledCall 結構
				// 這裡我們先用一個簡單的技巧：在當前物件暫時註冊一個隨機名稱的 Function 指向該閉包
				// 但更好的做法是讓 CallOut 結構支援直接執行 Closure。
				// 為了保持穩定，我們暫時只實作字串形式，若要支援閉包則需修改 ScheduledCall。
				// 既然 fs/ 常用，我們來擴充 ScheduledCall 吧。
				d.CallOutClosure(obj, fn, time.Duration(delay.Value)*time.Second, args[2:]...)
			default:
				return object.NewError("call_out 第一個參數必須是字串或閉包")
			}

			return &object.Nil{}
		},
	})

	// 語法: int remove_call_out(string func_name)
	// 說明: 移除排程中準備呼叫的 func_name。回傳移除的數量。
	// 範例: remove_call_out("respawn");
	obj.Vars.Set("remove_call_out", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Integer{Value: 0}
			}
			funcName, ok := args[0].(*object.String)
			if !ok {
				return &object.Integer{Value: 0}
			}

			d.mu.Lock()
			defer d.mu.Unlock()
			removedCount := 0
			var pending []*ScheduledCall
			for _, call := range d.CallOuts {
				if call.Caller == obj && call.FuncName == funcName.Value {
					removedCount++
				} else {
					pending = append(pending, call)
				}
			}
			d.CallOuts = pending
			return &object.Integer{Value: int64(removedCount)}
		},
	})

	// 語法: int find_call_out(string func_name)
	// 說明: 尋找指定排程函式還有多少秒會執行。若找不到回傳 -1。
	// 範例: int left = find_call_out("respawn");
	obj.Vars.Set("find_call_out", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Integer{Value: -1}
			}
			funcName, ok := args[0].(*object.String)
			if !ok {
				return &object.Integer{Value: -1}
			}

			d.mu.Lock()
			defer d.mu.Unlock()
			for _, call := range d.CallOuts {
				if call.Caller == obj && call.FuncName == funcName.Value {
					timeLeft := int64(time.Until(call.FireTime).Seconds())
					if timeLeft < 0 {
						timeLeft = 0
					}
					return &object.Integer{Value: timeLeft}
				}
			}
			return &object.Integer{Value: -1}
		},
	})

	// 語法: mixed *call_out_info()
	// 說明: 取得目前所有排程中 (call_out) 的詳細清單。
	// 範例: mixed *info = call_out_info();
	obj.Vars.Set("call_out_info", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			d.mu.Lock()
			defer d.mu.Unlock()

			var elements []object.Object
			for _, call := range d.CallOuts {
				// 格式: ({ object caller, string function, int time_left })
				timeLeft := int64(time.Until(call.FireTime).Seconds())
				if timeLeft < 0 {
					timeLeft = 0
				}

				elements = append(elements, &object.Array{Elements: []object.Object{
					call.Caller,
					&object.String{Value: call.FuncName},
					&object.Integer{Value: timeLeft},
				}})
			}
			return &object.Array{Elements: elements}
		},
	})

	// 輔助：時間轉換為 Mapping
	timeToMap := func(t time.Time) *object.Mapping {
		m := &object.Mapping{Pairs: make(map[object.HashKey]object.HashPair)}
		set := func(k string, v int) {
			ks := &object.String{Value: k}
			vs := &object.Integer{Value: int64(v)}
			m.Pairs[ks.HashKey()] = object.HashPair{Key: ks, Value: vs}
		}
		set("sec", t.Second())
		set("min", t.Minute())
		set("hour", t.Hour())
		set("mday", t.Day())
		set("mon", int(t.Month())-1) // LPC mon 0-11
		set("year", t.Year())
		set("wday", int(t.Weekday()))
		set("yday", t.YearDay())
		return m
	}

	// 語法: mapping localtime(int timestamp)
	// 說明: 取得本地時間。
	// 範例: mapping t = localtime(time());
	obj.Vars.Set("localtime", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			ts := time.Now().Unix()
			if len(args) > 0 {
				if i, ok := args[0].(*object.Integer); ok {
					ts = i.Value
				}
			}
			return timeToMap(time.Unix(ts, 0).Local())
		},
	})

	// 語法: mapping gmtime(int timestamp)
	// 說明: 取得 GMT 時間。
	// 範例: mapping t = gmtime(time());
	obj.Vars.Set("gmtime", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			ts := time.Now().Unix()
			if len(args) > 0 {
				if i, ok := args[0].(*object.Integer); ok {
					ts = i.Value
				}
			}
			return timeToMap(time.Unix(ts, 0).UTC())
		},
	})

	// 語法: string strftime(string format, [int timestamp])
	// 說明: 格式化時間。
	// 範例: strftime("%Y-%m-%d %H:%M:%S", time());
	obj.Vars.Set("strftime", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			format := "%Y%m%d_%H%M%S"
			if len(args) > 0 {
				if s, ok := args[0].(*object.String); ok {
					format = s.Value
				}
			}
			ts := time.Now().Unix()
			if len(args) > 1 {
				if i, ok := args[1].(*object.Integer); ok {
					ts = i.Value
				}
			}

			// 轉換 Go 的 time format
			t := time.Unix(ts, 0)
			f := format
			f = strings.ReplaceAll(f, "%Y", "2006")
			f = strings.ReplaceAll(f, "%m", "01")
			f = strings.ReplaceAll(f, "%d", "02")
			f = strings.ReplaceAll(f, "%H", "15")
			f = strings.ReplaceAll(f, "%M", "04")
			f = strings.ReplaceAll(f, "%S", "05")

			return &object.String{Value: t.Format(f)}
		},
	})

	// 語法: string ctime(int time)
	// 說明: 將 Unix 時間戳轉為人類可讀的字串格式。
	// 範例: write(ctime(time()));
	obj.Vars.Set("ctime", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			ts := time.Now().Unix()
			if len(args) > 0 {
				if i, ok := args[0].(*object.Integer); ok {
					ts = i.Value
				}
			}
			return &object.String{Value: time.Unix(ts, 0).Format("Mon Jan _2 15:04:05 2006")}
		},
	})
}

func (d *Driver) registerResetEfuns(obj *object.LPCObject) {
	// 語法: varargs void set_reset(object ob, int time)
	// 說明: 設定物件下次 reset 的時間。若 time 未提供，則使用預設公式。
	// 範例: set_reset(this_object(), 3600);
	obj.Vars.Set("set_reset", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			target, ok := args[0].(*object.LPCObject)
			if !ok {
				return &object.Nil{}
			}

			// 簡單實作：若有時間參數，設定到物件屬性
			if len(args) > 1 {
				if t, ok := args[1].(*object.Integer); ok {
					target.Vars.Set("_reset_time", t)
				}
			}
			return &object.Nil{}
		},
	})
}
