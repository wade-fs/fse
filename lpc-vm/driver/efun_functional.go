// driver/efun_functional.go
package driver

import (
	"mudscript/lpc-vm/evaluator"
	"mudscript/lpc-vm/object"
)

func (d *Driver) registerFunctionalEfuns(obj *object.LPCObject) {
	// 語法: mixed evaluate(mixed cl, [mixed args...])
	// 說明: 執行閉包 (closure) 或呼叫函式名稱。
	// 範例: evaluate((: write, "Hello" :));
	obj.Vars.Set("evaluate", &object.Builtin{
	        Fn: func(args ...object.Object) object.Object {
	                if len(args) < 1 {
	                        return evaluator.NilValue
	                }
	// 🚀 關鍵相容：若不是 closure，則直接回傳該值
	if args[0].TokenType() != object.ClosureType {
	// 實際上 MudOS 中 evaluate(3) 回傳 3。
	// evaluate("string") 回傳 "string"。
	return args[0]
	}
	                return d.ExecuteCallback(obj, args[0], args[1:])
	        },
	})

	// 語法: mixed apply(mixed cl, [mixed args...])
	// 說明: 同 evaluate，執行閉包或呼叫函式名稱。
	// 範例: apply("write", "Hello");
	obj.Vars.Set("apply", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return evaluator.NilValue
			}
			return d.ExecuteCallback(obj, args[0], args[1:])
		},
	})
}

func (d *Driver) registerBindingEfuns(obj *object.LPCObject) {
	// 語法: function bind(function f, object ob)
	// 說明: 將一個閉包綁定到指定的物件上。
	// 範例: f = bind((: f :), ob);
	obj.Vars.Set("bind", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return object.NewError("bind 需要 2 個參數")
			}
			f, ok1 := args[0].(*object.Closure)
			ob, ok2 := args[1].(*object.LPCObject)
			if !ok1 || !ok2 {
				return object.NewError("bind 參數型別錯誤")
			}

			// 複製閉包並綁定目標物件
			newClosure := &object.Closure{
				Lambda:    f.Lambda,
				Target:    ob,
				FuncName:  f.FuncName,
				BoundArgs: f.BoundArgs,
			}
			return newClosure
		},
	})

	// 語法: int origin()
	// 說明: 回傳當前函式呼叫的來源類型 (0: efun, 1: simul_efun, 2: lfun)。
	// 範例: if (origin() == 2) write("local call");
	obj.Vars.Set("origin", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			// 這個需要 Driver 的 CallStack 支持，目前簡化處理
			return &object.Integer{Value: 2} // 假設是 local call
		},
	})
}
