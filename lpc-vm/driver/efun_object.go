// driver/efun_object.go
package driver

import (
	"encoding/json"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"strings"

	"mudscript/lpc-vm/object"
)

func (d *Driver) registerEnvironmentEfuns(obj *object.LPCObject) {
	// 語法: object environment([object target])
	// 說明: 取得物件所在的環境 (房間或容器)。
	// 範例: object room = environment(this_player());
	obj.Vars.Set("environment", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := getTarget(args, obj)
			if target.Location != nil {
				return target.Location
			}
			return &object.Nil{}
		},
	})

	// 語法: int move_object(object dest) / int move_object(object item, object dest)
	// 說明: 將物件移動到目標物件(房間或容器)之內。回傳 1 成功。
	// 範例: move_object(load_object("/d/city/square"));
	obj.Vars.Set("move_object", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) == 1 && args[0].TokenType() == object.LPC_OBJECT_OBJ {
				d.MoveObject(obj, args[0].(*object.LPCObject))
				return &object.Integer{Value: 1}
			} else if len(args) == 2 && args[0].TokenType() == object.LPC_OBJECT_OBJ && args[1].TokenType() == object.LPC_OBJECT_OBJ {
				d.MoveObject(args[0].(*object.LPCObject), args[1].(*object.LPCObject))
				return &object.Integer{Value: 1}
			}
			return object.NewError("move_object 參數錯誤，需要 1 或 2 個 object 參數")
		},
	})

	// 語法: object clone_object(string file)
	// 說明: 根據腳本路徑，複製並產生一個新的物件實體 (Clone)。
	// 範例: object sword = clone_object("/obj/weapon/sword");
	obj.Vars.Set("clone_object", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) != 1 || args[0].TokenType() != object.StringType {
				return object.NewError("clone_object 需要 string 參數")
			}
			path := d.ResolvePath(obj.Filename, args[0].(*object.String).Value)
			clonedObj, err := d.CloneObject(path)
			if err != nil {
				log.Printf("⚠️ 物件載入失敗 (%s): %v", path, err)
				return &object.Integer{Value: 0}
			}
			return clonedObj
		},
	})

	// 語法: object *all_inventory([object target])
	// 說明: 取得目標物件內部包含的所有物件 (淺層搜尋)。回傳陣列。
	// 範例: object *items = all_inventory(this_player());
	obj.Vars.Set("all_inventory", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := getTarget(args, obj)
			elements := make([]object.Object, len(target.Inventory))
			for i, item := range target.Inventory {
				elements[i] = item
			}
			return &object.Array{Elements: elements}
		},
	})

	// 語法: object *deep_inventory([object target])
	// 說明: 取得目標物件內部包含的所有物件，包含子容器內的物品 (遞迴深層搜尋)。
	// 範例: object *inv = deep_inventory(this_object());
	obj.Vars.Set("deep_inventory", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := getTarget(args, obj)
			var result []object.Object
			var traverse func(*object.LPCObject)
			traverse = func(cur *object.LPCObject) {
				for _, item := range cur.Inventory {
					result = append(result, item)
					traverse(item)
				}
			}
			traverse(target)
			return &object.Array{Elements: result}
		},
	})

	// 語法: object present(string id_or_obj, [object env])
	// 說明: 在指定容器中尋找符合特定 ID 的物件。
	// 範例: object sword = present("sword", this_player());
	obj.Vars.Set("present", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return object.NewError("present 至少需要 1 個參數")
			}
			container := obj
			if len(args) > 1 {
				if c, ok := args[1].(*object.LPCObject); ok {
					container = c
				}
			}

			if targetObj, isObj := args[0].(*object.LPCObject); isObj {
				for _, item := range container.Inventory {
					if item == targetObj {
						return item
					}
				}
				return &object.Nil{}
			}

			if idStr, isStr := args[0].(*object.String); isStr {
				for _, item := range container.Inventory {
					res := d.CallFunction(item, "id", []object.Object{idStr})
					if isLPCTrue(res) {
						return item
					}
				}
			}
			return &object.Nil{}
		},
	})

	// 語法: object first_inventory([object ob])
	// 說明: 回傳指定物件庫存中的第一個物品。若無則回傳 0。
	// 範例: object item = first_inventory(this_object());
	obj.Vars.Set("first_inventory", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := getTarget(args, obj)
			if len(target.Inventory) > 0 {
				return target.Inventory[0]
			}
			return &object.Nil{}
		},
	})

	// 語法: object next_inventory(object ob)
	// 說明: 回傳指定物品在同容器中的下一個物品。若無則回傳 0。
	// 範例: object next_item = next_inventory(item);
	obj.Vars.Set("next_inventory", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			item, ok := args[0].(*object.LPCObject)
			if !ok {
				return &object.Nil{}
			}

			env := item.Location
			if env == nil {
				return &object.Nil{}
			}

			for i, invItem := range env.Inventory {
				if invItem == item && i+1 < len(env.Inventory) {
					return env.Inventory[i+1]
				}
			}
			return &object.Nil{}
		},
	})

	// 語法: int set_light(int adjustment)
	// 說明: 設定物件的光照度。若傳入 0 則僅查詢當前光照度。
	// 範例: set_light(1); // 增加 1 點光照
	obj.Vars.Set("set_light", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Integer{Value: int64(obj.Light)}
			}
			adj, ok := args[0].(*object.Integer)
			if !ok {
				return &object.Integer{Value: int64(obj.Light)}
			}
			obj.Light += int(adj.Value)
			return &object.Integer{Value: int64(obj.Light)}
		},
	})

	// 語法: string base_name(object ob)
	// 說明: 取得物件的原始檔案路徑 (去除 #clone_id)。
	// 範例: base_name(find_player("wade")) -> "/std/user.c"
	obj.Vars.Set("base_name", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			target, ok := args[0].(*object.LPCObject)
			if !ok {
				return &object.Nil{}
			}

			name := target.Filename
			if pos := strings.Index(name, "#"); pos != -1 {
				name = name[:pos]
			}
			return &object.String{Value: name}
		},
	})

	// 語法: string query_uuid([object target])
	// 說明: 取得物件的 UUID。
	// 範例: string uid = query_uuid(this_player());
	obj.Vars.Set("query_uuid", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := obj
			if len(args) > 0 {
				if t, ok := args[0].(*object.LPCObject); ok {
					target = t
				}
			}
			return &object.String{Value: target.UUID}
		},
	})

	// 語法: object find_by_uuid(string uuid)
	// 說明: 透過 UUID 尋找記憶體中的物件。
	// 範例: object target = find_by_uuid("...");
	obj.Vars.Set("find_by_uuid", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			uuidArg, ok := args[0].(*object.String)
			if !ok {
				return &object.Nil{}
			}

			d.mu.RLock()
			defer d.mu.RUnlock()
			if o, exists := d.UUIDTable[uuidArg.Value]; exists {
				return o
			}
			return &object.Nil{}
		},
	})
}
func (d *Driver) registerPersistenceEfuns(obj *object.LPCObject) {
	// 語法: int save_object(string file)
	// 說明: 將當前物件內的所有變數狀態，以 JSON 格式儲存至硬碟。
	// 範例: save_object("/data/user/" + id);
	obj.Vars.Set("save_object", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Integer{Value: 0}
			}
			fileArg, ok := args[0].(*object.String)
			if !ok {
				return object.NewError("save_object 需要字串參數")
			}

			fileName := fileArg.Value
			if !strings.HasSuffix(fileName, ".o") {
				fileName += ".o"
			}

			allowed, errMsg := d.checkWritePermission(obj, fileName, "save_object")
			if !allowed {
				if p := d.GetCurrentPlayer(); p != nil {
					p.Send(fmt.Sprintf("\r\n⚠️ 系統安全攔截：%s\r\n", errMsg))
				} else {
					fmt.Printf("🚫 存檔拒絕: %s\n", errMsg)
				}
				return &object.Integer{Value: 0}
			}

			fullPath := filepath.Join(d.Config.MudLibPath, fileName)

			saveData := make(map[string]interface{})
			for k, v := range obj.Vars.GetAll() {
				if strings.HasPrefix(k, "_") {
					continue
				}
				if v.TokenType() == object.FunctionType || v.TokenType() == object.BuiltinType || v.TokenType() == object.ClosureType {
					continue
				}
				saveData[k] = lpcToGoValue(v)
			}

			os.MkdirAll(filepath.Dir(fullPath), 0755)
			jsonData, err := json.MarshalIndent(saveData, "", "  ")
			if err != nil {
				return &object.Integer{Value: 0}
			}

			err = os.WriteFile(fullPath, jsonData, 0644)
			if err != nil {
				return &object.Integer{Value: 0}
			}
			return &object.Integer{Value: 1}
		},
	})

	// 語法: int restore_object(string file)
	// 說明: 從硬碟讀取儲存的 JSON 變數，恢復當前物件的狀態。成功回傳 1，失敗回傳 0。
	// 範例: if(restore_object("/data/user/" + id)) { write("讀檔成功"); }
	obj.Vars.Set("restore_object", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Integer{Value: 0}
			}
			fileArg, ok := args[0].(*object.String)
			if !ok {
				return object.NewError("restore_object 需要字串參數")
			}

			fileName := fileArg.Value
			if !strings.HasSuffix(fileName, ".o") {
				fileName += ".o"
			}
			fullPath := filepath.Join(d.Config.MudLibPath, fileName)

			jsonData, err := os.ReadFile(fullPath)
			if err != nil {
				return &object.Integer{Value: 0}
			}

			var loadedData map[string]interface{}
			err = json.Unmarshal(jsonData, &loadedData)
			if err != nil {
				return &object.Integer{Value: 0}
			}

			for k, v := range loadedData {
				obj.Vars.Set(k, goToLPCValue(v))
			}
			return &object.Integer{Value: 1}
		},
	})

	// 語法: int exec(object target, object src)
	// 說明: 將 TCP 連線狀態從來源物件(src)轉移到目標物件(target)上。常用於登入系統連線切換。
	// 範例: exec(user_ob, this_object());
	obj.Vars.Set("exec", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return object.NewError("exec 需要兩個 object 參數")
			}
			target, ok1 := args[0].(*object.LPCObject)
			src, ok2 := args[1].(*object.LPCObject)
			if !ok1 || !ok2 {
				return object.NewError("exec 參數必須是 object")
			}

			success := d.TransferConnection(target, src)
			if success {
				return &object.Integer{Value: 1}
			}
			return &object.Integer{Value: 0}
		},
	})

	obj.Vars.Set("save_variable", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.String{Value: ""}
			}
			data := lpcToGoValue(args[0])
			jsonData, err := json.Marshal(data)
			if err != nil {
				return &object.String{Value: ""}
			}
			return &object.String{Value: string(jsonData)}
		},
	})

	obj.Vars.Set("restore_variable", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			str, ok := args[0].(*object.String)
			if !ok {
				return &object.Nil{}
			}
			var data interface{}
			err := json.Unmarshal([]byte(str.Value), &data)
			if err != nil {
				return &object.Nil{}
			}
			return goToLPCValue(data)
		},
	})
}

// ==========================================
// 轉換工具 (LPC 與 Go 原生型別互轉，供 JSON 存檔用)
// ==========================================

func lpcToGoValue(o object.Object) interface{} {
	if o == nil {
		return nil
	}
	switch v := o.(type) {
	case *object.Integer:
		return v.Value
	case *object.Float:
		return v.Value
	case *object.String:
		return v.Value
	case *object.Boolean:
		return v.Value
	case *object.Array:
		arr := make([]interface{}, len(v.Elements))
		for i, el := range v.Elements {
			arr[i] = lpcToGoValue(el)
		}
		return arr
	case *object.Mapping:
		m := make(map[string]interface{})
		for _, pair := range v.Pairs {
			keyStr := pair.Key.Inspect()
			if s, ok := pair.Key.(*object.String); ok {
				keyStr = s.Value
			}
			m[keyStr] = lpcToGoValue(pair.Value)
		}
		return m
	default:
		return nil
	}
}

func goToLPCValue(v interface{}) object.Object {
	if v == nil {
		return &object.Nil{}
	}
	switch val := v.(type) {
	case float64:
		if val == float64(int64(val)) {
			return &object.Integer{Value: int64(val)}
		}
		return &object.Float{Value: val}
	case string:
		return &object.String{Value: val}
	case bool:
		return &object.Boolean{Value: val}
	case []interface{}:
		arr := make([]object.Object, len(val))
		for i, el := range val {
			arr[i] = goToLPCValue(el)
		}
		return &object.Array{Elements: arr}
	case map[string]interface{}:
		m := &object.Mapping{Pairs: make(map[object.HashKey]object.HashPair)}
		for k, el := range val {
			strKey := &object.String{Value: k}
			m.Pairs[strKey.HashKey()] = object.HashPair{Key: strKey, Value: goToLPCValue(el)}
		}
		return m
	default:
		return &object.Nil{}
	}
}

func (d *Driver) registerFunctionExistsEfun(obj *object.LPCObject) {
	obj.Vars.Set("function_exists", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return &object.Nil{}
			}
			funcName, ok1 := args[0].(*object.String)
			target, ok2 := args[1].(*object.LPCObject)
			if !ok1 || !ok2 {
				return &object.Nil{}
			}

			// 檢查物件本身的函式與 Efun
			if _, exists := target.Functions[funcName.Value]; exists {
				return &object.String{Value: target.Filename}
			}
			if _, exists := target.Vars.Get(funcName.Value); exists {
				return &object.String{Value: "efun"}
			}
			if _, exists := target.Vars.Get("efun::" + funcName.Value); exists {
				return &object.String{Value: "efun"}
			}
			return &object.Nil{}
		},
	})
}

func (d *Driver) registerLifecycleEfuns(obj *object.LPCObject) {
	// 語法: object new(string file)
	// 說明: 根據腳本路徑，複製並產生一個新的物件實體。
	// 範例: object sword = new("/obj/weapon/sword");
	obj.Vars.Set("new", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) != 1 || args[0].TokenType() != object.StringType {
				return object.NewError("new 需要 string 參數")
			}
			path := d.ResolvePath(obj.Filename, args[0].(*object.String).Value)
			clonedObj, err := d.CloneObject(path)
			if err != nil {
				log.Printf("⚠️ 物件載入失敗 (%s): %v", path, err)
				return &object.Integer{Value: 0}
			}
			return clonedObj
		},
	})

	// 語法: string file_name([object ob])
	// 說明: 取得物件的完整檔案名稱。
	// 範例: write(file_name(this_object()));
	obj.Vars.Set("file_name", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := obj
			if len(args) > 0 {
				if t, ok := args[0].(*object.LPCObject); ok {
					target = t
				}
			}
			return &object.String{Value: target.Filename}
		},
	})
}

func (d *Driver) registerMemoryEfuns(obj *object.LPCObject) {
	// 語法: void swap(object ob)
	// 說明: 強制將物件交換至虛擬記憶體或清除記憶體佔用。
	// 範例: swap(this_object());
	obj.Vars.Set("swap", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			// 目前實作為 No-op
			return &object.Nil{}
		},
	})
}

func (d *Driver) registerInheritanceEfuns(obj *object.LPCObject) {
	// 語法: string *inherit_list(object ob)
	// 說明: 取得物件直接繼承的檔案列表。
	// 範例: string *parents = inherit_list(this_object());
	obj.Vars.Set("inherit_list", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := obj
			if len(args) > 0 {
				if t, ok := args[0].(*object.LPCObject); ok {
					target = t
				}
			}
			var result []object.Object
			for _, inh := range target.Inherits {
				result = append(result, &object.String{Value: inh.Filename})
			}
			return &object.Array{Elements: result}
		},
	})

	// 語法: string *deep_inherit_list(object ob)
	// 說明: 遞迴取得物件繼承的所有檔案列表 (包含繼承鏈)。
	// 範例: string *parents = deep_inherit_list(this_object());
	obj.Vars.Set("deep_inherit_list", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := obj
			if len(args) > 0 {
				if t, ok := args[0].(*object.LPCObject); ok {
					target = t
				}
			}
			var result []object.Object
			var visited = make(map[string]bool)
			var traverse func(*object.LPCObject)
			traverse = func(cur *object.LPCObject) {
				for _, inh := range cur.Inherits {
					if !visited[inh.Filename] {
						visited[inh.Filename] = true
						result = append(result, &object.String{Value: inh.Filename})
						traverse(inh)
					}
				}
			}
			traverse(target)
			return &object.Array{Elements: result}
		},
	})

	// 語法: int inherits(string file, object ob)
	// 說明: 判斷物件是否繼承了指定的檔案 (包含間接繼承)。
	// 範例: if (inherits("/std/char", me)) ...
	obj.Vars.Set("inherits", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return &object.Integer{Value: 0}
			}
			file, ok1 := args[0].(*object.String)
			target, ok2 := args[1].(*object.LPCObject)
			if !ok1 || !ok2 {
				return &object.Integer{Value: 0}
			}

			path := file.Value
			if !strings.HasSuffix(path, ".c") {
				path += ".c"
			}

			var checkInherits func(ob *object.LPCObject) bool
			checkInherits = func(ob *object.LPCObject) bool {
				for _, inh := range ob.Inherits {
					if inh.Filename == path || strings.HasSuffix(inh.Filename, "/"+path) {
						return true
					}
					if checkInherits(inh) {
						return true
					}
				}
				return false
			}

			if checkInherits(target) {
				return &object.Integer{Value: 1}
			}
			return &object.Integer{Value: 0}
		},
	})
	// 語法: int virtualp(object ob)
	obj.Vars.Set("virtualp", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			// 目前尚無虛擬物件系統，先傳回 0
			return &object.Integer{Value: 0}
		},
	})

	// 語法: void set_hide(int flag)
	obj.Vars.Set("set_hide", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.Nil{}
		},
	})
}

// 語法: void reload_object(object ob)
// 說明: 重新載入物件的定義。
// 範例: reload_object(this_object());
func (d *Driver) registerReloadObjectEfun(obj *object.LPCObject) {
	obj.Vars.Set("reload_object", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			target, ok := args[0].(*object.LPCObject)
			if !ok {
				return &object.Nil{}
			}
			d.LoadObject(target.Filename)
			return &object.Nil{}
		},
	})

	// 語法: object shadow(object ob, int flag)
	// 說明: 讓當前物件 shadow (代理) 指定物件。
	obj.Vars.Set("shadow", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			target, ok := args[0].(*object.LPCObject)
			if !ok {
				return &object.Nil{}
			}
			obj.ShadowedObject = target
			return target
		},
	})

	// 語法: object query_shadowing(object ob)
	// 說明: 查詢物件是否正在被 shadow。
	obj.Vars.Set("query_shadowing", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			target, ok := args[0].(*object.LPCObject)
			if !ok {
				return &object.Nil{}
			}
			if target.ShadowedObject != nil {
				return target.ShadowedObject
			}
			return &object.Nil{}
		},
	})
}
