// driver/comm.go
package driver

import (
	"strings"

	"mudscript/lpc-vm/object"
)

// ProcessAnsi 將自定義的 {r} 與 $TAG$ 標籤轉換為 ANSI 色碼 (用於終端機)
func (d *Driver) ProcessAnsi(text string) string {
	colorMap := map[string]string{
		"r":  "\x1b[31m",
		"g":  "\x1b[32m",
		"y":  "\x1b[33m",
		"b":  "\x1b[34m",
		"m":  "\x1b[35m",
		"c":  "\x1b[36m",
		"w":  "\x1b[37m",
		"gr": "\x1b[90m",
		"n":  "\x1b[0m",
	}

	res := text
	for tag, code := range colorMap {
		res = strings.ReplaceAll(res, "{"+tag+"}", code)
	}

	// 支援 MUDLib 標準定義的 $TAG$ 顏色代碼
	mudColorMap := map[string]string{
		"$NOR$": "\x1b[0m",
		"$BLK$": "\x1b[30m",
		"$RED$": "\x1b[31m",
		"$GRN$": "\x1b[32m",
		"$YEL$": "\x1b[33m",
		"$BLU$": "\x1b[34m",
		"$MAG$": "\x1b[35m",
		"$CYN$": "\x1b[36m",
		"$WHT$": "\x1b[37m",
		"$GRA$": "\x1b[90m",
		
		"$HIK$": "\x1b[1;30m",
		"$HIR$": "\x1b[1;31m",
		"$HIG$": "\x1b[1;32m",
		"$HIY$": "\x1b[1;33m",
		"$HIB$": "\x1b[1;34m",
		"$HIM$": "\x1b[1;35m",
		"$HIC$": "\x1b[1;36m",
		"$HIW$": "\x1b[1;37m",
	}
	for tag, code := range mudColorMap {
		res = strings.ReplaceAll(res, tag, code)
	}

	return res
}

// TellObject 向指定物件發送訊息 (封裝通訊邏輯)
func (d *Driver) TellObject(target *object.LPCObject, msg string) {
	if target == nil || target.IsDestructed {
		return
	}

	// 1. 若為連線中的玩家，直接透過 Socket 發送
	conn := d.GetConnectionFromObject(target)
	if conn != nil {
		safeMsg := strings.ReplaceAll(msg, "\r\n", "\n")
		safeMsg = strings.ReplaceAll(safeMsg, "\n", "\r\n")
		conn.Send(safeMsg)
	}

	// 2. 不論是否為玩家，都觸發 catch_tell 函式 (供 NPC 或腳本攔截訊息)
	initiator := d.GetCurrentPlayer()
	if initiator != nil {
		d.RunCommand(initiator, target, "catch_tell", []object.Object{&object.String{Value: msg}})
	} else {
		d.CallFunction(target, "catch_tell", []object.Object{&object.String{Value: msg}})
	}
}

// TellRoom 向指定房間內的所有物件廣播訊息 (支援排除清單)
func (d *Driver) TellRoom(room *object.LPCObject, msg string, exclude []*object.LPCObject) {
	if room == nil || room.IsDestructed {
		return
	}

	for _, item := range room.Inventory {
		if item == nil || item.IsDestructed {
			continue
		}

		shouldExclude := false
		for _, ex := range exclude {
			if item == ex {
				shouldExclude = true
				break
			}
		}

		if !shouldExclude {
			d.TellObject(item, msg)
		}
	}
}

// ProcessCommand 處理玩家輸入的指令。回傳 true 代表指令被成功處理，false 代表指令不存在或執行失敗。
func (d *Driver) ProcessCommand(pConn *PlayerConnection, input string) bool {
	if pConn == nil || pConn.Object == nil {
		return false
	}
	obj := pConn.Object

	// ==========================================
	// 0. 優先處理 input_to 攔截
	// ==========================================
	if pConn.NextInputFunc != "" || pConn.NextInputClosure != nil {
		funcName := pConn.NextInputFunc
		closure := pConn.NextInputClosure
		targetObj := pConn.NextInputObj
		args := pConn.NextInputArgs

		pConn.NextInputFunc = ""
		pConn.NextInputClosure = nil
		pConn.NextInputObj = nil
		pConn.NextInputArgs = nil
		pConn.InputHidden = false

		finalArgs := append([]object.Object{&object.String{Value: input}}, args...)

		if closure != nil {
			restore := d.setPlayerContext(pConn)
			defer restore()
			d.ExecuteCallback(obj, closure, finalArgs)
		} else {
			if targetObj == nil {
				targetObj = obj
			}
			d.RunCommand(pConn, targetObj, funcName, finalArgs)
		}
		return true 
	}

	input = strings.TrimSpace(input)
	if input == "" {
		return true 
	}

	// 處理動詞歷史與 ! 展開
	input = pConn.ExpandHistory(input)
	if input == "" {
		return true
	}

	// ==========================================
	// 1. 呼叫 process_input (別名擴展)
	// ==========================================
	resInput := d.RunCommand(pConn, obj, "process_input", []object.Object{&object.String{Value: input}})
	if resInput != nil && resInput.TokenType() != object.NilType {
		if s, ok := resInput.(*object.String); ok && s.Value != "" {
			input = s.Value // 指令被改寫
		} else if d.IsLPCTrue(resInput) {
			return true // 已被完全處理
		}
	}

	// 解析動詞與參數
	input = strings.TrimSpace(input)
	if input == "" {
		return true
	}

	verb := ""
	arg := ""
	parts := strings.SplitN(input, " ", 2)
	verb = parts[0]
	if len(parts) > 1 {
		arg = parts[1]
	}

	pConn.CurrentVerb = verb
	pConn.NotifyFail = "" 

	// ==========================================
	// 2. 檢查 add_action 註冊的指令
	// ==========================================
	if obj.Actions != nil {
		// A. 精確匹配
		if action, exists := obj.Actions[verb]; exists {
			oldVerb := pConn.CurrentVerb
			pConn.CurrentVerb = verb
			res := d.RunCommand(pConn, action.Provider, action.FuncName, []object.Object{&object.String{Value: arg}})
			pConn.CurrentVerb = oldVerb
			if d.IsLPCTrue(res) {
				return true
			}
		}

		// B. 前綴匹配與萬用攔截 ("")
		for v, action := range obj.Actions {
			if action.Flags == 1 {
				// 略過空動詞匹配 (保留給最後處理)
				if v == "" { continue }

				if strings.HasPrefix(verb, v) {
					oldVerb := pConn.CurrentVerb
					pConn.CurrentVerb = v

					callArg := arg
					if len(verb) > len(v) {
						callArg = verb[len(v):] + " " + arg
						callArg = strings.TrimSpace(callArg)
					}

					res := d.RunCommand(pConn, action.Provider, action.FuncName, []object.Object{&object.String{Value: callArg}})
					pConn.CurrentVerb = oldVerb

					if d.IsLPCTrue(res) {
						d.postCommandCleanup(pConn)
						return true
					}
				}
			}
		}
		
		// C. 專門處理萬用攔截器 (Legacy FS 專用)
		if action, exists := obj.Actions[""]; exists && action.Flags == 1 {
			oldVerb := pConn.CurrentVerb
			pConn.CurrentVerb = verb
			res := d.RunCommand(pConn, action.Provider, action.FuncName, []object.Object{&object.String{Value: arg}})
			pConn.CurrentVerb = oldVerb
			if d.IsLPCTrue(res) {
				d.postCommandCleanup(pConn)
				return true
			}
		}
	}

	// ==========================================
	// 3. 智慧出口後備機制 (Smart Exit Fallback)
	// ==========================================
	// 🚀 重要修正：只有在非 "go" 開頭時才執行，避免無限遞迴
	if verb != "go" && obj.Location != nil && !obj.Location.IsDestructed {
		if exitsVal, exists := obj.Location.Vars.Get("exits"); exists {
			if exitsMap, ok := exitsVal.(*object.Mapping); ok {
				verbKey := (&object.String{Value: verb}).HashKey()
				if _, hasExit := exitsMap.Pairs[verbKey]; hasExit {
					// 轉換為 go 指令並直接處理，若成功則不再繼續
					if d.ProcessCommand(pConn, "go "+input) {
						return true
					}
				}
			}
		}
	}

	// ==========================================
	// 4. 處理失敗訊息 (notify_fail)
	// ==========================================
	if pConn.NotifyFail != "" {
		pConn.Send(pConn.NotifyFail)
		pConn.NotifyFail = "" 
		return true
	}

	// 若完全找不到指令，報錯
	pConn.Send("什麼？\n")
	d.CallFunction(obj, "write_prompt", nil)

	return false
}

func (d *Driver) postCommandCleanup(pConn *PlayerConnection) {
	if pConn != nil && pConn.Object != nil {
		d.CallFunction(pConn.Object, "write_prompt", nil)
	}
}
