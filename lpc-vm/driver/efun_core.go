// driver/efun_core.go
package driver

import (
	"strings"

	"mudscript/lpc-vm/evaluator"
	"mudscript/lpc-vm/object"
)

func (d *Driver) registerCoreEfuns(obj *object.LPCObject) {
	// 語法: int userp(object ob)
	// 說明: 判斷該物件是否為玩家物件。
	// 範例: if (userp(this_player())) {}
	obj.Vars.Set("userp", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) == 0 {
				if obj.IsInteractive {
					return &object.Integer{Value: 1}
				}
				return &object.Integer{Value: 0}
			}
			if o, ok := args[0].(*object.LPCObject); ok {
				if o.IsInteractive {
					return &object.Integer{Value: 1}
				}
				if strings.HasPrefix(o.Filename, "/std/user.c") || strings.HasPrefix(o.Filename, "/data/user/") {
					return &object.Integer{Value: 1}
				}
			}
			return &object.Integer{Value: 0}
		},
	})

	// 語法: int is_interactive(object ob)
	// 說明: 判斷該物件是否為連線中的玩家。
	// 範例: if (is_interactive(this_player())) {}
	obj.Vars.Set("is_interactive", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := getTarget(args, obj)
			if target.IsInteractive {
				return &object.Integer{Value: 1}
			}
			return &object.Integer{Value: 0}
		},
	})

	// 語法: string query_verb()
	// 說明: 回傳當前觸發指令的動詞。
	// 範例: 若輸入 "go north"，query_verb() 回傳 "go"。
	obj.Vars.Set("query_verb", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			conn := d.GetCurrentPlayer()
			if conn != nil {
				return &object.String{Value: conn.CurrentVerb}
			}
			return &object.Nil{}
		},
	})

	// 語法: mixed call_other(mixed ob, string func, [mixed args...])
	// 說明: 動態呼叫物件上的函式。當函式名稱是變數時非常有用。
	// 範例: call_other(this_player(), "set_" + prop_name, value);
	// 範例: call_other("/adm/daemons/logind", "logon", this_object());
	obj.Vars.Set("call_other", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return &object.Nil{}
			}

			var target *object.LPCObject
			if targetFile, ok := args[0].(*object.String); ok {
				var err error
				target, err = d.LoadObject(targetFile.Value)
				if err != nil || target == nil {
					return &object.Nil{}
				}
			} else if targetObj, ok := args[0].(*object.LPCObject); ok {
				target = targetObj
			} else {
				return &object.Nil{}
			}

			funcName, ok2 := args[1].(*object.String)
			if !ok2 {
				return &object.Nil{}
			}
			return d.CallFunction(target, funcName.Value, args[2:])
		},
	})

	// 語法: int living(object ob)
	// 說明: 判斷物件是否為活著的生物。
	// 實作: 直接檢查物件的 IsLiving 標記 (通常由 enable_commands() 設定)。
	// 範例: if (living(this_player())) {}
	obj.Vars.Set("living", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := getTarget(args, obj)
			if target != nil && target.IsLiving {
				return &object.Integer{Value: 1}
			}
			return &object.Integer{Value: 0}
		},
	})

	// 語法: object find_player(string id)
	// 說明: 找到 player id 的物件
	// 範例: object user = find_player(arg);
	obj.Vars.Set("find_player", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) == 0 {
				return &object.Nil{}
			}
			idStr, ok := args[0].(*object.String)
			if !ok {
				return &object.Nil{}
			}

			var target object.Object = &object.Nil{}
			d.interactiveObjects.Range(func(key, value interface{}) bool {
				conn, ok := value.(*PlayerConnection)
				if ok && conn.IsActive && conn.Object != nil {
					res := d.CallFunction(conn.Object, "get_id", nil)
					if s, isStr := res.(*object.String); isStr && s.Value == idStr.Value {
						target = conn.Object
						return false // 找到即中斷
					}
				}
				return true
			})
			return target
		},
	})

	// 語法: int interactive(object ob)
	// 說明: 判斷該物件是否為正在連線中的玩家 (有網路 Socket 綁定)。
	// 範例: if (interactive(target)) { write("玩家在線上。\n"); }
	obj.Vars.Set("interactive", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := obj
			if len(args) > 0 {
				if o, ok := args[0].(*object.LPCObject); ok {
					target = o
				} else {
					return &object.Integer{Value: 0}
				}
			}
			if d.GetConnectionFromObject(target) != nil {
				return &object.Integer{Value: 1}
			}
			return &object.Integer{Value: 0}
		},
	})

	// 語法: string query_ip_number(object ob)
	// 說明: 取得玩家連線的 IP 位址。
	// 範例: write(query_ip_number(this_player()));
	obj.Vars.Set("query_ip_number", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := obj
			if len(args) > 0 {
				if o, ok := args[0].(*object.LPCObject); ok {
					target = o
				} else {
					return &object.String{Value: ""}
				}
			}
			if conn := d.GetConnectionFromObject(target); conn != nil && conn.Conn != nil {
				addr := conn.Conn.RemoteAddr().String()
				// Remove port if present
				if colonIdx := strings.LastIndex(addr, ":"); colonIdx != -1 {
					return &object.String{Value: addr[:colonIdx]}
				}
				return &object.String{Value: addr}
			}
			return &object.String{Value: "127.0.0.1"}
		},
	})

	// 語法: string query_ip_name(object ob)
	// 說明: 取得玩家連線的 Hostname (暫以 IP 代替)。
	// 範例: write(query_ip_name(this_player()));
	obj.Vars.Set("query_ip_name", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := obj
			if len(args) > 0 {
				if o, ok := args[0].(*object.LPCObject); ok {
					target = o
				} else {
					return &object.String{Value: ""}
				}
			}
			if conn := d.GetConnectionFromObject(target); conn != nil && conn.Conn != nil {
				addr := conn.Conn.RemoteAddr().String()
				if colonIdx := strings.LastIndex(addr, ":"); colonIdx != -1 {
					return &object.String{Value: addr[:colonIdx]}
				}
				return &object.String{Value: addr}
			}
			return &object.String{Value: "localhost"}
		},
	})

	// 語法: int is_web_client(object ob)
	// 說明: 判斷玩家是否使用 Web 介面連線。
	// 範例: if (is_web_client(this_player())) {}
	obj.Vars.Set("is_web_client", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := obj
			if len(args) > 0 {
				if o, ok := args[0].(*object.LPCObject); ok {
					target = o
				} else {
					return &object.Integer{Value: 0}
				}
			}
			if conn := d.GetConnectionFromObject(target); conn != nil {
				if conn.OutputCallback != nil {
					return &object.Integer{Value: 1}
				}
			}
			return &object.Integer{Value: 0}
		},
	})

	// 語法: int set_heart_beat(int flag)
	// 說明: 開啟(1)或關閉(0)物件的心跳機制 (每秒觸發一次 heart_beat 函式)。
	// 範例: if (living(inv[i])) inv[i]->set_heart_beat(1);
	obj.Vars.Set("set_heart_beat", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return object.NewError("set_heart_beat 需要 1 個整數參數")
			}
			flag, ok := args[0].(*object.Integer)
			if !ok {
				return object.NewError("set_heart_beat 參數必須是整數")
			}
			enable := flag.Value > 0

			// 🚀 修正：直接使用傳入的 obj，確保是設定在 NPC 自己身上
			// 而不是誤設在當前發話的玩家 (GetThisObject) 身上。
			d.SetHeartBeat(obj, enable)
			return &object.Integer{Value: flag.Value}
		},
	})

	// 語法: int query_heart_beat([object ob])
	// 說明: 查詢物件的心跳狀態，回傳該物件設定的 heart_beat 頻率 (或 0 代表關閉)。
	// 範例: if (query_heart_beat(me)) write("心跳運作中\n");
	obj.Vars.Set("query_heart_beat", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := getTarget(args, obj)

			d.mu.RLock()
			_, exists := d.Heartbeats[target]
			d.mu.RUnlock()

			if exists {
				return &object.Integer{Value: 1}
			}
			return &object.Integer{Value: 0}
		},
	})

	// 語法: void destruct(object ob)
	// 說明: 從記憶體中徹底銷毀指定的物件。若未指定參數，則銷毀自己。
	// 範例: destruct(me);
	obj.Vars.Set("destruct", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := getTarget(args, obj)
			d.DestructObject(target)
			return &object.Nil{}
		},
	})

	// 語法: void enable_commands()
	// 說明: 將當前物件標記為生物 (Living)，使其可以接收與執行 add_action 註冊的指令。
	// 範例: enable_commands();
	obj.Vars.Set("enable_commands", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			obj.IsLiving = true
			if obj.Actions == nil {
				obj.Actions = make(map[string]*object.Action)
			}
			return &object.Integer{Value: 1}
		},
	})

	// 語法: void set_living_name(string name)
	// 說明: 設定生物的對外名稱，並同步至連線資訊中。
	// 範例: set_living_name("wade");
	obj.Vars.Set("set_living_name", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return evaluator.NilValue
			}
			name, ok := args[0].(*object.String)
			if !ok {
				return evaluator.NilValue
			}

			d.UpdatePlayerUsername(obj, name.Value)
			return evaluator.NilValue
		},
	})

	// 語法: void add_action(string func_name, string verb, [int flag])
	// 說明: 為玩家註冊一個指令，當玩家輸入 verb 時，會呼叫 func_name。
	// 若 flag 為 1，則 verb 會被視為前綴。若 verb 為空字串且 flag 為 1，則攔截所有輸入。
	// 範例: add_action("do_look", "look");
	obj.Vars.Set("add_action", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return object.NewError("add_action 需至少 2 個參數")
			}
			funcName, ok1 := args[0].(*object.String)
			verb, ok2 := args[1].(*object.String)
			if !ok1 || !ok2 {
				return object.NewError("add_action 參數型別錯誤")
			}

			flag := 0
			if len(args) > 2 {
				if f, ok := args[2].(*object.Integer); ok {
					flag = int(f.Value)
				}
			}

			playerObj := obj
			if p := d.GetCurrentPlayer(); p != nil && p.Object != nil {
				playerObj = p.Object
			}

			if !playerObj.IsLiving {
				playerObj.IsLiving = true
			}
			if playerObj.Actions == nil {
				playerObj.Actions = make(map[string]*object.Action)
			}
			
			// 🚀 關鍵相容性修正：如果 verb 是陣列，則註冊多個指令
			verbs := []string{verb.Value}
			
			for _, v := range verbs {
				playerObj.Actions[v] = &object.Action{
					Verb:     v,
					FuncName: funcName.Value,
					Provider: obj,
					Flags:    flag,
				}
			}
			return &object.Integer{Value: 1}
		},
	})

	// 語法: int command(string cmd)
	// 說明: 以當前物件的身分執行一條指令。回傳 1 表示執行成功，0 表示失敗。
	// 範例: command("say hello");
	obj.Vars.Set("command", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) != 1 || args[0].TokenType() != object.StringType {
				return object.NewError("command 需 1 個字串參數")
			}
			input := strings.TrimSpace(args[0].(*object.String).Value)
			if input == "" {
				return &object.Integer{Value: 0}
			}

			pConn := d.getPlayerConnection(obj)
			if pConn == nil {
				pConn = &PlayerConnection{Object: obj, IsActive: true}
			}
			
			// 🚀 核心修正：將所有 command() 的呼叫導向標準的指令處理流程
			success := d.ProcessCommand(pConn, input)
			if success {
				return &object.Integer{Value: 1}
			}
			return &object.Integer{Value: 0}
		},
	})

	// 語法: void throw(mixed msg)
	// 說明: 主動拋出一個執行期錯誤，可被 catch() 攔截。
	// 範例: if (!arg) throw("參數不能為空！\n");
	obj.Vars.Set("throw", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			msg := "LPC Error"
			if len(args) > 0 {
				msg = args[0].Inspect()
				if s, ok := args[0].(*object.String); ok {
					msg = s.Value
				}
			}
			return object.NewError("%s", msg)
		},
	})

	// 語法: object this_player()
	// 說明: 取得觸發當前執行緒的玩家物件。若無則回傳 0。
	// 範例: if (me == this_player()) continue; // skip self
	obj.Vars.Set("this_player", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			p := d.GetCurrentPlayer()
			if p != nil && p.Object != nil {
				return p.Object
			}
			return &object.Nil{}
		},
	})

	// 語法: object this_object()
	// 說明: 取得當前正在執行程式碼的物件。
	// 範例: mixed here_inv = all_inventory(this_object());
	obj.Vars.Set("this_object", &object.Builtin{
		Fn: func(args ...object.Object) object.Object { return obj },
	})

	// 語法: object previous_object()
	// 說明: 取得呼叫當前函式的物件。
	// 範例: if (previous_object() == find_object(MASTER_OB)) {}
	obj.Vars.Set("previous_object", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			prev := d.GetPreviousObject()
			if prev != nil {
				return prev
			}
			return &object.Nil{}
		},
	})
}

func (d *Driver) registerErrorEfun(obj *object.LPCObject) {
	obj.Vars.Set("error", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			msg := "未知錯誤"
			if len(args) > 0 {
				if s, ok := args[0].(*object.String); ok {
					msg = s.Value
				}
			}
			return object.NewError("%s", msg)
		},
	})
}

func (d *Driver) registerSetQueryEfuns(obj *object.LPCObject) {
	// 語法: void set(string key, mixed val)
	// 說明: 設定物件的屬性值。
	// 範例: set("hp", 100);
	obj.Vars.Set("set", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return &object.Nil{}
			}
			key, ok1 := args[0].(*object.String)
			if !ok1 {
				return &object.Nil{}
			}
			obj.Vars.Set(key.Value, args[1])
			return &object.Nil{}
		},
	})

	// 語法: mixed query(string key)
	// 說明: 查詢物件的屬性值。
	// 範例: int hp = query("hp");
	obj.Vars.Set("query", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			key, ok := args[0].(*object.String)
			if !ok {
				return &object.Nil{}
			}
			if val, exists := obj.Vars.Get(key.Value); exists {
				return val
			}
			return &object.Nil{}
		},
	})

	// 語法: int remove_action(string verb)
	// 說明: 移除先前透過 add_action 註冊的指令。
	// 範例: remove_action("look");
	obj.Vars.Set("remove_action", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Integer{Value: 0}
			}
			verb, ok := args[0].(*object.String)
			if !ok {
				return &object.Integer{Value: 0}
			}

			removed := false
			// 1. 從當前物件移除
			if obj.Actions != nil {
				if _, exists := obj.Actions[verb.Value]; exists {
					delete(obj.Actions, verb.Value)
					removed = true
				}
			}

			// 2. 從當前玩家移除 (與 add_action 對應)
			if p := d.GetCurrentPlayer(); p != nil && p.Object != nil && p.Object != obj {
				if p.Object.Actions != nil {
					if _, exists := p.Object.Actions[verb.Value]; exists {
						delete(p.Object.Actions, verb.Value)
						removed = true
					}
				}
			}

			if removed {
				return &object.Integer{Value: 1}
			}
			return &object.Integer{Value: 0}
		},
	})

	// 語法: mapping commands()
	// 說明: 回傳當前物件所有註冊的指令。
	// 範例: mapping cmds = commands();
	obj.Vars.Set("commands", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			m := &object.Mapping{Pairs: make(map[object.HashKey]object.HashPair)}
			if obj.Actions != nil {
				for verb, act := range obj.Actions {
					k := &object.String{Value: verb}
					v := &object.String{Value: act.FuncName}
					m.Pairs[k.HashKey()] = object.HashPair{Key: k, Value: v}
				}
			}
			return m
		},
	})

	// 語法: void notify_fail(string msg)
	// 說明: 設定指令執行失敗時回傳的訊息。
	// 範例: notify_fail("你不知道該怎麼做。\n");
	obj.Vars.Set("notify_fail", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			msg, ok := args[0].(*object.String)
			if !ok {
				return &object.Nil{}
			}

			pConn := d.GetCurrentPlayer()
			if pConn != nil {
				pConn.NotifyFail = msg.Value
			}
			return &object.Nil{}
		},
	})

	// 語法: void disable_commands()
	// 說明: 將當前物件標記為不可互動。
	// 範例: disable_commands();
	obj.Vars.Set("disable_commands", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			obj.IsLiving = false
			return &object.Nil{}
		},
	})
}

// 語法: object find_living(string str)
// 說明: 尋找第一個標記為 living 且 id 符合 str 的生物。
// 範例: object npc = find_living("guard");
func (d *Driver) registerFindLivingEfun(obj *object.LPCObject) {
	obj.Vars.Set("find_living", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			idStr, ok := args[0].(*object.String)
			if !ok {
				return &object.Nil{}
			}

			d.mu.RLock()
			defer d.mu.RUnlock()
			for _, ob := range d.ObjectTable {
				if ob != nil && ob.IsLiving && !ob.IsDestructed {
					res := d.CallFunction(ob, "id", []object.Object{idStr})
					if isLPCTrue(res) {
						return ob
					}
				}
			}
			return &object.Nil{}
		},
	})
}

func (d *Driver) registerSecurityEfuns(obj *object.LPCObject) {
	// 語法: string getuid(object ob)
	obj.Vars.Set("getuid", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			// 如果有傳入物件，應該回傳該物件的 uid (簡化處理)
			if len(args) > 0 {
				if t, ok := args[0].(*object.LPCObject); ok {
					if t.Filename == d.MasterObject.Filename || strings.HasPrefix(t.Filename, "/adm/") {
						return &object.String{Value: d.RootUID}
					}
					return &object.String{Value: "User"}
				}
			}
			return &object.String{Value: d.RootUID}
		},
	})

	// 語法: string geteuid(object ob)
	obj.Vars.Set("geteuid", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) > 0 {
				if t, ok := args[0].(*object.LPCObject); ok {
					if t.Filename == d.MasterObject.Filename || strings.HasPrefix(t.Filename, "/adm/") {
						return &object.String{Value: d.RootUID}
					}
					return &object.String{Value: "User"}
				}
			}
			return &object.String{Value: d.RootUID}
		},
	})

	// 語法: int seteuid(string euid)
	obj.Vars.Set("seteuid", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			// 權限檢查邏輯通常由 Master 物件控制，這裡簡化處理
			return &object.Integer{Value: 1}
		},
	})

	// 語法: int export_uid(object ob)
	obj.Vars.Set("export_uid", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.Integer{Value: 1}
		},
	})

	// 語法: object master()
	obj.Vars.Set("master", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return d.MasterObject
		},
	})
}

func (d *Driver) registerPrivilegeEfuns(obj *object.LPCObject) {
	// 語法: void set_privs(object ob, string privs)
	// 說明: 設定物件的特權字串。
	// 範例: set_privs(this_object(), "wizard");
	obj.Vars.Set("set_privs", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return &object.Nil{}
			}
			target, ok1 := args[0].(*object.LPCObject)
			privs, ok2 := args[1].(*object.String)
			if !ok1 || !ok2 {
				return &object.Nil{}
			}
			target.Vars.Set("_privs", privs)
			return &object.Nil{}
		},
	})

	// 語法: string query_privs(object ob)
	// 說明: 查詢物件的特權字串。
	// 範例: string privs = query_privs(this_object());
	obj.Vars.Set("query_privs", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := obj
			if len(args) > 0 {
				if t, ok := args[0].(*object.LPCObject); ok {
					target = t
				}
			}
			if val, exists := target.Vars.Get("_privs"); exists {
				if s, ok := val.(*object.String); ok {
					return s
				}
			}
			return &object.String{Value: ""}
		},
	})

	// 語法: object *all_previous_objects()
	// 說明: 回傳呼叫鏈中所有前置物件的陣列。
	// 範例: object *objs = all_previous_objects();
	obj.Vars.Set("all_previous_objects", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			// 此處需要 Driver 的 CallStack 支援，目前簡化處理
			return &object.Array{Elements: []object.Object{}}
		},
	})
}
