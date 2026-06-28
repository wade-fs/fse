// driver/efun_p2p.go
package driver

import (
	"mudscript/lpc-vm/evaluator"
	"mudscript/lpc-vm/object"
)

// ==========================================
// 🚀 P2P 擴充 (P2P Extensions)
// ==========================================
func (d *Driver) registerP2PEfuns(obj *object.LPCObject) {
	// 語法: void p2p_broadcast(string content, [string sender_id])
	// 說明: 將訊息發送到全球 P2P 網路（星際網路）。
	// 範例: p2p_broadcast("Hello World");
	obj.Vars.Set("p2p_broadcast", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return evaluator.NilValue
			}
			content, ok := args[0].(*object.String)
			if !ok {
				return evaluator.NilValue
			}

			if d.P2PSendChat != nil {
				sender := ""
				if len(args) > 1 {
					if customSender, ok := args[1].(*object.String); ok && customSender.Value != "" {
						sender = customSender.Value
					}
				}

				if sender == "" {
					if p := d.GetCurrentPlayer(); p != nil {
						sender = p.Username
					} else {
						// 🚀 關鍵修正：若無當前玩家，則代表是系統驅動的通訊 (如 fs_d)
						// 這裡應該優先取用 system_d 定義的 FS_MUDLIB_ID
						systemD, err := d.LoadObject("/secure/system_d")
						if err == nil && systemD != nil {
							if res := d.CallFunction(systemD, "query_mudlib_id", nil); res != nil {
								if s, ok := res.(*object.String); ok {
									sender = s.Value
								}
							}
						}
						// 備援：若連 system_d 都沒法回傳，才用 Filename
						if sender == "" {
							sender = obj.Filename
						}
					}
				}
				d.P2PSendChat(sender, content.Value)
			}
			return evaluator.NilValue
		},
	})

	// 語法: void p2p_send_direct(string target_sid, string content)
	obj.Vars.Set("p2p_send_direct", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return evaluator.NilValue
			}
			// 此處僅為接口佔位，完整實作需 WebRTC/DataChannel 支持
			return evaluator.NilValue
		},
	})
}
