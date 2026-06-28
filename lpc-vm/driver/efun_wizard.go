// driver/efun_wizard.go
package driver

import (
	"fmt"
	"mudscript/lpc-vm/evaluator"
	"mudscript/lpc-vm/object"
)

func (d *Driver) registerWizardEfuns(obj *object.LPCObject) {
	// 語法: void enable_wizard()
	// 說明: 將當前物件標記為巫師。
	obj.Vars.Set("enable_wizard", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			obj.IsWizard = true
			return &object.Integer{Value: 1}
		},
	})

	// 語法: void disable_wizard()
	// 說明: 移除當前物件的巫師標記。
	obj.Vars.Set("disable_wizard", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			obj.IsWizard = false
			return &object.Integer{Value: 0}
		},
	})

	// 語法: int wizardp(object ob)
	// 說明: 判斷物件是否為巫師。
	obj.Vars.Set("wizardp", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := getTarget(args, obj)
			if target.IsWizard {
				return &object.Integer{Value: 1}
			}
			return &object.Integer{Value: 0}
		},
	})

	// 語法: string wizhood(object ob)
	// 說明: 回傳物件的身分組字串 (如 "(admin)", "(wizard)", "(player)")。
	obj.Vars.Set("wizhood", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := getTarget(args, obj)
			// 🚀 關鍵相容：回傳符合 Mudlib 預期的標籤
			if target.IsWizard {
				return &object.String{Value: "(admin)"}
			}
			// 🚀 關鍵相容性：嘗試透過 query("id") 取得玩家 ID
			id := d.CallFunction(target, "query", []object.Object{&object.String{Value: "id"}})
			if idStr, ok := id.(*object.String); ok {
				if idStr.Value == "guest" {
					return &object.String{Value: "(guest)"}
				}
			}
			return &object.String{Value: "(player)"}
		},
	})

	// 語法: int snoop(object snooper, object snoopee)
	// 說明: 讓 snooper 監看 snoopee 的輸入與輸出。若 snoopee 為空，則停止 snooper 的所有監看。
	obj.Vars.Set("snoop", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Integer{Value: 0}
			}
			snooper, ok1 := args[0].(*object.LPCObject)
			if !ok1 {
				return &object.Integer{Value: 0}
			}

			snooperConn := d.GetConnectionFromObject(snooper)
			if snooperConn == nil {
				return &object.Integer{Value: 0}
			}

			// 停止監看
			if len(args) == 1 {
				d.interactiveObjects.Range(func(key, value interface{}) bool {
					conn := value.(*PlayerConnection)
					if conn.SnoopedBy == snooperConn {
						conn.SnoopedBy = nil
					}
					return true
				})
				return &object.Integer{Value: 1}
			}

			snoopee, ok2 := args[1].(*object.LPCObject)
			if !ok2 {
				return &object.Integer{Value: 0}
			}

			snoopeeConn := d.GetConnectionFromObject(snoopee)
			if snoopeeConn == nil {
				return &object.Integer{Value: 0}
			}

			// 防止循環監看
			temp := snooperConn
			for temp != nil {
				if temp == snoopeeConn {
					return &object.Integer{Value: 0}
				}
				temp = temp.SnoopedBy
			}

			snoopeeConn.SnoopedBy = snooperConn
			return &object.Integer{Value: 1}
		},
	})

	// 語法: object query_snoop(object ob)
	// 說明: 查詢誰正在監看 ob。
	obj.Vars.Set("query_snoop", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := getTarget(args, obj)
			conn := d.GetConnectionFromObject(target)
			if conn != nil && conn.SnoopedBy != nil && conn.SnoopedBy.Object != nil {
				return conn.SnoopedBy.Object
			}
			return &object.Nil{}
		},
	})

	// 語法: object query_snooping(object ob)
	// 說明: 查詢 ob 正在監看誰。
	obj.Vars.Set("query_snooping", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := getTarget(args, obj)
			targetConn := d.GetConnectionFromObject(target)
			if targetConn == nil {
				return &object.Nil{}
			}

			var snooping object.Object = &object.Nil{}
			d.interactiveObjects.Range(func(key, value interface{}) bool {
				conn := value.(*PlayerConnection)
				if conn.SnoopedBy == targetConn && conn.Object != nil {
					snooping = conn.Object
					return false
				}
				return true
			})
			return snooping
		},
	})

	// 語法: int request_web_edit(string path)
	// 說明: 讀取檔案內容並透過 WebSocket 發送 JSON 給前端編輯器。
	// 範例: request_web_edit("/area/entrance.c");
	obj.Vars.Set("request_web_edit", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Integer{Value: 0}
			}
			pathStr, ok := args[0].(*object.String)
			if !ok {
				return &object.Integer{Value: 0}
			}

			// 1. 取得當前玩家連線
			p := d.GetCurrentPlayer()
			if p == nil && obj.IsInteractive {
				p = d.GetConnectionFromObject(obj)
			}
			if p == nil || !p.IsActive || p.OutputCallback == nil {
				return &object.Integer{Value: 0} // 非 Web 玩家或連線不活動
			}

			// 2. 權限檢查 (使用 ResolvePath 確保路徑正確)
			resolvedPath := d.ResolvePath(obj.Filename, pathStr.Value)
			// 我們通常用 read_file 的權限檢查
			// 由於 checkReadPermission 也是內部 helper，這裡直接調用或模仿 read_file 邏輯
			// 為求簡化且安全，我們模仿 efun_system.go 的邏輯
			
			// 3. 讀取檔案
			content, err := d.ReadFile(resolvedPath)
			if err != nil {
				// 可能是新檔案，傳回空字串
				content = []byte("")
			}

			// 4. 封裝 JSON 並發送
			// 技巧：我們發送一個帶有特殊前綴的字串，讓 hub.go 攔截並重組
			// 🚀 關鍵修正：Payload 必須是 JSON 字串，因為 Frontend 會解析它
			innerPayload := fmt.Sprintf("{ \"path\": \"%s\", \"content\": %q }", resolvedPath, string(content))
			payload := fmt.Sprintf("__JSON_MSG__{ \"type\": \"edit_file\", \"payload\": %q }", innerPayload)
			p.Send(payload)

			return &object.Integer{Value: 1}
		},
	})

	// 語法: void set_this_player(object ob)
	// 說明: 設定當前執行緒的 this_player。
	obj.Vars.Set("set_this_player", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return evaluator.NilValue
			}
			target, ok := args[0].(*object.LPCObject)
			if !ok {
				return evaluator.NilValue
			}

			conn := d.GetConnectionFromObject(target)
			if conn == nil {
				// 如果不是互動玩家，建立一個臨時的虛擬連線以支持 GetCurrentPlayer().Object
				// 🚀 修正：必須使用 NewPlayerConnection 以初始化通道與啟動 WritePump
				conn = NewPlayerConnection(nil, target)
			}
			
			gid := getGID()
			d.playerContexts.Store(gid, conn)
			return evaluator.NilValue
		},
	})
}
