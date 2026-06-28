// driver/efun_helper.go
package driver

import (
	"fmt"
	"log"
	"path/filepath"
	"strings"

	"mudscript/lpc-vm/evaluator"
	"mudscript/lpc-vm/object"
)

// ExecuteCallback 支援傳入函式名稱 (string) 或 函式指標 (closure)
func (d *Driver) ExecuteCallback(obj *object.LPCObject, fnArg object.Object, args []object.Object) object.Object {
	switch v := fnArg.(type) {
	case *object.String:
		log.Printf("🎮 [Callback] %s->%s(%v)", obj.Filename, v.Value, args)
		return d.CallFunction(obj, v.Value, args)
	case *object.Closure:
		// 🚀 執行 Lambda
		if v.Lambda != nil || len(v.Expressions) > 0 || len(v.Parameters) > 0 {
			log.Printf("🎮 [Callback] %s-><lambda>(%v)", obj.Filename, args)
			// 建立一個閉包環境，繼承自定義時的環境
			lambdaEnv := object.NewEnclosedEnvironment(v.Env)

			// 1. 注入具名型別參數 (如果有的話)
			if len(v.Parameters) > 0 {
				for i, param := range v.Parameters {
					if i < len(args) {
						lambdaEnv.Set(param.Name.Value, args[i])
					} else {
						// 如果參數不足，給予預設值
						lambdaEnv.Set(param.Name.Value, evaluator.GetDefaultLPCValue(param.Token.Literal))
					}
				}
			}

			// 2. 注入 $1, $2, $3... (傳統 Lambda 參數)
			for i, arg := range args {
				lambdaEnv.Set(fmt.Sprintf("$%d", i+1), arg)
			}

			// 3. 執行 Lambda
			if v.Lambda != nil {
				return evaluator.Eval(v.Lambda, lambdaEnv)
			}

			var result object.Object
			for _, expr := range v.Expressions {
				result = evaluator.Eval(expr, lambdaEnv)
				if result != nil && result.TokenType() == object.ReturnValueType {
					return result.(*object.ReturnValue).Value
				}
			}
			return result
		}

		target := v.Target
		if target == nil {
			target = obj
		}
		
		argStrs := make([]string, len(args))
		for i, arg := range args {
			if arg != nil {
				argStrs[i] = arg.Inspect()
			} else {
				argStrs[i] = "nil"
			}
		}
		log.Printf("🎮 [Callback] %s->%s(%s) [via closure]", target.Filename, v.FuncName, strings.Join(argStrs, ", "))

		// 合併綁定參數與傳入參數
		callArgs := append([]object.Object{}, v.BoundArgs...)
		callArgs = append(callArgs, args...)
		return d.CallFunction(target, v.FuncName, callArgs)
	default:
		return object.NewError("callback 必須是字串或 closure")
	}
}

// CheckReadPermission 匯出供外部 (如 signaling) 使用
func (d *Driver) CheckReadPermission(caller *object.LPCObject, path string, efunName string) (bool, string) {
	return d.checkReadPermission(caller, path, efunName)
}

// checkReadPermission 呼叫 LPC 的 valid_read 來判定權限
func (d *Driver) checkReadPermission(caller *object.LPCObject, path string, efunName string) (bool, string) {
	// 🚀 遞迴防護：防止在 valid_read 內部呼叫 load_object 導致無限循環
	gid := getGID()
	if _, already := d.inPermissionCheck.Load(gid); already {
		return true, ""
	}
	d.inPermissionCheck.Store(gid, true)
	defer d.inPermissionCheck.Delete(gid)

	cleanPath := filepath.Clean(path)
	cleanPath = filepath.ToSlash(cleanPath)
	if !strings.HasPrefix(cleanPath, "/") {
		cleanPath = "/" + cleanPath
	}

	if d.MasterObject == nil {
		return true, "" // 若沒 master 則預設允許
	}

	player := d.GetCurrentPlayer()
	var userObj object.Object = &object.Nil{}
	if player != nil && player.Object != nil {
		userObj = player.Object
	} else if caller != nil {
		// 🚀 關鍵強化：偵測是否為具有 Role 的使用者物件 (即使非互動連線)
		if caller.Vars != nil {
			if r, ok := caller.Vars.Get("role"); ok && r.TokenType() != object.NilType {
				userObj = caller
			} else if caller.IsInteractive {
				userObj = caller
			}
		}
	}

	res := d.CallFunction(d.MasterObject, "valid_read", []object.Object{
		&object.String{Value: cleanPath},
		userObj,
		&object.String{Value: efunName},
	})

	switch v := res.(type) {
	case *object.Integer:
		if v.Value != 0 {
			return true, ""
		}
		return false, "權限拒絕：無法讀取該路徑。"
	case *object.String:
		return false, v.Value
	case *object.Nil:
		return true, ""
	default:
		return true, ""
	}
}

// CheckWritePermission 匯出供外部 (如 signaling) 使用
func (d *Driver) CheckWritePermission(caller *object.LPCObject, path string, efunName string) (bool, string) {
	return d.checkWritePermission(caller, path, efunName)
}

// checkWritePermission 呼叫 LPC 的 valid_write 來判定權限
func (d *Driver) checkWritePermission(caller *object.LPCObject, path string, efunName string) (bool, string) {
	// 🚀 遞迴防護：防止在 valid_write 內部再次觸發寫入檢查導致無限循環
	gid := getGID()
	if _, already := d.inPermissionCheck.Load(gid); already {
		return true, ""
	}
	d.inPermissionCheck.Store(gid, true)
	defer d.inPermissionCheck.Delete(gid)

	// 1. 【路徑正規化】：防禦 ../ 目錄穿越攻擊
	cleanPath := filepath.Clean(path)
	cleanPath = filepath.ToSlash(cleanPath) // 確保跨平台都使用 MUD 習慣的 "/"
	if !strings.HasPrefix(cleanPath, "/") {
		cleanPath = "/" + cleanPath
	}

	if d.MasterObject == nil {
		return true, "" // 若沒 master 則預設允許
	}

	// 🚀 關鍵修正：直接傳遞發起寫入的「物件」 (caller) 給 valid_write
	// 權限系統應根據此物件的身分（路徑或內部 Role）來判定
	res := d.CallFunction(d.MasterObject, "valid_write", []object.Object{
		&object.String{Value: cleanPath},
		caller,
		&object.String{Value: efunName},
	})

	// 4. 【解析動態錯誤訊息】：判斷回傳型別
	allowed := false
	errMsg := "權限不足：寫入拒絕。"

	switch v := res.(type) {
	case *object.Integer:
		if v.Value != 0 {
			allowed = true
			errMsg = ""
		}
	case *object.String:
		allowed = false
		errMsg = v.Value
	}

	// 🚀 背景日誌：方便追蹤所有的權限行為
	if !allowed {
		log.Printf("🛡️  [Permission] Denied: %s (by %s in %s)", cleanPath, efunName, caller.Filename)
	}


	return allowed, errMsg
}
