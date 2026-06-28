// driver/efun_comm.go
package driver

import (
	"fmt"
	"strings"

	"mudscript/lpc-vm/object"
)

func (d *Driver) registerCommEfuns(obj *object.LPCObject) {
	// 語法: void shout(string msg)
	// 說明: 對全伺服器所有連線中的玩家廣播訊息 (會自動排除自己)。
	// 範例: shout("【謠言】" + query_name() + " 登入了遊戲！\n");
	obj.Vars.Set("shout", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) == 0 {
				return &object.Nil{}
			}
			msg := args[0].Inspect()
			if s, ok := args[0].(*object.String); ok {
				msg = s.Value
			}

			tp := d.GetCurrentPlayer()
			if tp == nil && obj.IsInteractive {
				tp = d.GetConnectionFromObject(obj)
			}

			d.interactiveObjects.Range(func(key, value interface{}) bool {
				if conn, ok := value.(*PlayerConnection); ok && conn.IsActive {
					if tp == nil || conn != tp { // 排除發送者自己
						safeMsg := strings.ReplaceAll(msg, "\r\n", "\n")
						conn.Send(strings.ReplaceAll(safeMsg, "\n", "\r\n"))
					}
				}
				return true
			})
			return &object.Nil{}
		},
	})

	// 語法: void write(string msg)
	// 說明: 發送訊息給觸發當前行為的玩家。
	// 範例: write("你看了看四周。\n");
	obj.Vars.Set("write", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			msg := ""
			if len(args) > 0 {
				if s, ok := args[0].(*object.String); ok {
					msg = s.Value
				} else {
					msg = args[0].Inspect()
				}
			}

			p := d.GetCurrentPlayer()
			// 👉 關鍵修正：若無全域玩家上下文 (如 NPC 心跳中)，但呼叫者是玩家物件，則自動導向該玩家
			if p == nil && obj.IsInteractive {
				p = d.GetConnectionFromObject(obj)
			}

			// 只要玩家物件存在且處於活動狀態，就呼叫 p.Send
			if p != nil && p.IsActive {
				safeMsg := strings.ReplaceAll(msg, "\r\n", "\n")
				safeMsg = strings.ReplaceAll(safeMsg, "\n", "\r\n")
				p.Send(safeMsg)
			} else {
				// 只有在找不到玩家上下文時（例如系統背景執行），才印到伺服器終端機
				// 在終端機模式下，我們嘗試轉換 {r} 標籤為 ANSI
				processed := d.ProcessAnsi(msg)
				fmt.Print(processed)
			}
			return &object.Nil{}
		},
	})

	// 語法: void write_raw(string msg)
	// 說明: 發送原始 HTML 訊息給玩家 (不進行 ANSI 解析與 Markdown 處理)。
	// 範例: write_raw("<div style='color:red'>Hello</div>");
	obj.Vars.Set("write_raw", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			msg := ""
			if len(args) > 0 {
				if s, ok := args[0].(*object.String); ok {
					msg = s.Value
				} else {
					msg = args[0].Inspect()
				}
			}

			p := d.GetCurrentPlayer()
			if p == nil && obj.IsInteractive {
				p = d.GetConnectionFromObject(obj)
			}

			if p != nil && p.IsActive {
				p.OutputRaw = true
				p.Send(msg)
				p.OutputRaw = false
			} else {
				fmt.Print(msg)
			}
			return &object.Nil{}
		},
	})

	// 語法: void say(string msg, [mixed exclude])
	// 說明: 將訊息廣播給與當前物件處於同一環境(房間)內的所有其他物件。
	// 範例: say("一陣微風吹過。\n");
	obj.Vars.Set("say", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) == 0 {
				return &object.Nil{}
			}
			msgStr := args[0].Inspect()
			if s, ok := args[0].(*object.String); ok {
				msgStr = s.Value
			}

			var exclude []*object.LPCObject

			// 1. 預設排除執行此程式碼的物件 (例如指令物件)
			exclude = append(exclude, obj)

			// 2. 獲取當前玩家連線
			tp := d.GetCurrentPlayer()
			if tp != nil && tp.Object != nil {
				exclude = append(exclude, tp.Object)
			}

			// 3. 處理手動指定的額外排除對象
			if len(args) > 1 {
				if o, ok := args[1].(*object.LPCObject); ok {
					exclude = append(exclude, o)
				} else if arr, ok := args[1].(*object.Array); ok {
					for _, el := range arr.Elements {
						if o, ok := el.(*object.LPCObject); ok {
							exclude = append(exclude, o)
						}
					}
				}
			}

			// 取得環境 (指令物件本身不在房間，所以要取 tp 的環境)
			env := obj.Location
			if env == nil && tp != nil && tp.Object != nil {
				env = tp.Object.Location
			}

			if env == nil {
				return &object.Nil{}
			}

			// 4. 🚀 核心邏輯：遍歷房間
			for _, ob := range env.Inventory {
				if ob == nil || ob.IsDestructed {
					continue
				}

				// 檢查排除清單
				isExcluded := false
				for _, ex := range exclude {
					if ob == ex || (ex != nil && ob.Filename == ex.Filename) {
						isExcluded = true
						break
					}
				}

				// 🚀 終極排除：如果這個物件對應的連線就是當前發話者的連線，排除之！
				if tp != nil {
					obConn := d.GetConnectionFromObject(ob)
					if obConn != nil && obConn == tp {
						isExcluded = true
					}
				}

				if isExcluded {
					continue
				}

				// A. 判定是否為玩家
				isUser := false
				if ob.IsInteractive || strings.HasPrefix(ob.Filename, "/std/user.c") || strings.HasPrefix(ob.Filename, "/data/user/") {
					isUser = true
				}

				// B. 判定是否為生物
				isLiving := false
				res := d.CallFunction(ob, "is_living", nil)
				if isLPCTrue(res) {
					isLiving = true
				}

				if isUser {
					// 發送訊息到玩家終端 (TellObject 會處理 Socket 發送)
					d.TellObject(ob, msgStr)
				} else if isLiving {
					// 主動對 NPC 呼叫 catch_tell
					if tp != nil {
						d.RunCommand(tp, ob, "catch_tell", []object.Object{&object.String{Value: msgStr}})
					} else {
						d.CallFunction(ob, "catch_tell", []object.Object{&object.String{Value: msgStr}})
					}
				}
			}

			return &object.Nil{}
		},
	})

	// 語法: void tell_object(object target, string msg)
	// 說明: 直接向指定物件 (通常是玩家) 發送訊息。
	// 範例: tell_object(user, "你感受到一股強大的力量。\n");
	obj.Vars.Set("tell_object", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return object.NewError("tell_object 需要 2 個參數")
			}
			targetObj, ok := args[0].(*object.LPCObject)
			if !ok {
				return object.NewError("tell_object 的第一個參數必須是 object")
			}

			msg := args[1].Inspect()
			if s, isStr := args[1].(*object.String); isStr {
				msg = s.Value
			}

			d.TellObject(targetObj, msg)
			return &object.Nil{}
		},
	})

	// 語法: void tell_room(object room, string msg, [mixed exclude])
	// 說明: 向指定房間廣播訊息。
	// 範例: tell_room(room, "天空中閃過一道雷電。\n");
	obj.Vars.Set("tell_room", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return object.NewError("tell_room 需要 2 個參數")
			}
			roomObj, ok := args[0].(*object.LPCObject)
			if !ok {
				return object.NewError("tell_room 的第一個參數必須是 object")
			}

			msg := args[1].Inspect()
			if s, isStr := args[1].(*object.String); isStr {
				msg = s.Value
			}

			var exclude []*object.LPCObject
			if len(args) > 2 {
				if o, ok := args[2].(*object.LPCObject); ok {
					exclude = append(exclude, o)
				} else if arr, ok := args[2].(*object.Array); ok {
					for _, el := range arr.Elements {
						if o, ok := el.(*object.LPCObject); ok {
							exclude = append(exclude, o)
						}
					}
				}
			}

			// 🚀 tell_room 也加入終極連線排除
			tp := d.GetCurrentPlayer()

			for _, item := range roomObj.Inventory {
				if item == nil || item.IsDestructed {
					continue
				}

				shouldExclude := false
				for _, ex := range exclude {
					if item == ex || (ex != nil && item.Filename == ex.Filename) {
						shouldExclude = true
						break
					}
				}

				if tp != nil {
					itemConn := d.GetConnectionFromObject(item)
					if itemConn != nil && itemConn == tp {
						shouldExclude = true
					}
				}

				if !shouldExclude {
					d.TellObject(item, msg)
				}
			}
			return &object.Nil{}
		},
	})

	// 語法: void message(string class, string msg, mixed target, [mixed exclude])
	// 說明: 傳送訊息給特定物件或房間。
	// 範例: message("say", "你對大家說：哈囉。\n", environment(me), me);
	obj.Vars.Set("message", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 3 {
				return object.NewError("message 至少需要 3 個參數")
			}
			target := args[2]
			msg := ""
			if s, ok := args[1].(*object.String); ok {
				msg = s.Value
			}

			var excludeList []*object.LPCObject
			if len(args) > 3 {
				if exObj, ok := args[3].(*object.LPCObject); ok {
					excludeList = append(excludeList, exObj)
				} else if exArr, ok := args[3].(*object.Array); ok {
					for _, t := range exArr.Elements {
						if ob, ok := t.(*object.LPCObject); ok {
							excludeList = append(excludeList, ob)
						}
					}
				}
			}

			if room, ok := target.(*object.LPCObject); ok {
				conn := d.GetConnectionFromObject(room)
				isChar := d.CallFunction(room, "is_character", nil)
				isLiving := d.CallFunction(room, "is_living", nil)

				if conn != nil || isLPCTrue(isChar) || isLPCTrue(isLiving) {
					// 檢查是否在排除名單
					isExcluded := false
					for _, ex := range excludeList {
						if ex == room {
							isExcluded = true
							break
						}
					}
					if !isExcluded {
						d.TellObject(room, msg)
					}
				} else {
					d.TellRoom(room, msg, excludeList)
				}
			} else if targetArr, ok := target.(*object.Array); ok {
				for _, t := range targetArr.Elements {
					if ob, ok := t.(*object.LPCObject); ok {
						isExcluded := false
						for _, ex := range excludeList {
							if ex == ob {
								isExcluded = true
								break
							}
						}
						if !isExcluded {
							d.TellObject(ob, msg)
						}
					}
				}
			}
			return &object.Nil{}
		},
	})

	// 語法: int receive(string msg)
	// 說明: 直接向當前物件發送訊息。
	// 範例: this_player()->receive("系統訊息：伺服器即將重啟。\n");
	obj.Vars.Set("receive", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Integer{Value: 0}
			}
			msg, ok := args[0].(*object.String)
			if !ok {
				return &object.Integer{Value: 0}
			}

			d.TellObject(obj, msg.Value)
			if obj.IsInteractive {
				return &object.Integer{Value: 1}
			}
			return &object.Integer{Value: 0}
		},
	})
	// 語法: int in_input(object ob)
	obj.Vars.Set("in_input", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := getTarget(args, obj)
			if conn := d.GetConnectionFromObject(target); conn != nil {
				if conn.NextInputFunc != "" {
					return &object.Integer{Value: 1}
				}
			}
			return &object.Integer{Value: 0}
		},
	})

	// 語法: int in_edit(object ob)
	obj.Vars.Set("in_edit", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := getTarget(args, obj)
			if conn := d.GetConnectionFromObject(target); conn != nil {
				if strings.Contains(conn.NextInputFunc, "edit_loop") {
					return &object.Integer{Value: 1}
				}
			}
			return &object.Integer{Value: 0}
		},
	})

	// 🚀 Socket Efuns Stub (暫不實作完整邏輯，僅提供介面避免 Error)
	obj.Vars.Set("socket_create", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.Integer{Value: -1}
		},
	})
	obj.Vars.Set("socket_connect", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.Integer{Value: -1}
		},
	})
	obj.Vars.Set("socket_write", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.Integer{Value: -1}
		},
	})
	obj.Vars.Set("socket_close", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.Integer{Value: -1}
		},
	})
	obj.Vars.Set("socket_release", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.Integer{Value: -1}
		},
	})
	obj.Vars.Set("socket_acquire", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.Integer{Value: -1}
		},
	})
	obj.Vars.Set("socket_address", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.String{Value: "0.0.0.0 0"}
		},
	})
}

func (d *Driver) registerInteractiveEfuns(obj *object.LPCObject) {
	// 語法: object this_interactive()
	// 說明: 回傳當前互動的玩家物件。如果不是互動連線，則回傳 0。
	// 範例: if (this_interactive() == me) ...
	obj.Vars.Set("this_interactive", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			p := d.GetCurrentPlayer()
			if p != nil && p.Object != nil && p.IsActive {
				return p.Object
			}
			return &object.Nil{}
		},
	})

	// 語法: int get_char(string func, [int flag])
	// 說明: 獲取玩家下一個輸入字元並傳給指定的函式。若 flag 為 1，則隱藏輸入內容。
	// 範例: get_char("do_choice", 1);
	obj.Vars.Set("get_char", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Integer{Value: 0}
			}
			funcName, ok := args[0].(*object.String)
			if !ok {
				return &object.Integer{Value: 0}
			}

			p := d.GetCurrentPlayer()
			if p == nil && obj.IsInteractive {
				p = d.GetConnectionFromObject(obj)
			}
			if p == nil {
				return &object.Integer{Value: 0}
			}

			p.NextInputFunc = funcName.Value
			p.InputHidden = false
			if len(args) > 1 {
				if flag, ok := args[1].(*object.Integer); ok && flag.Value != 0 {
					p.InputHidden = true
					p.Send("__INPUT_HIDDEN__")
				}
			}
			return &object.Integer{Value: 1}
		},
	})
}
