// driver/efun_parse.go
package driver

import (
	"strings"

	"mudscript/lpc-vm/object"
)

func (d *Driver) registerParseEfuns(obj *object.LPCObject) {
	// 語法: int parse_command(string cmd, object | object *source, string pattern, [mixed args...])
	// 說明: 解析自然語言指令。此為簡化版實作。
	obj.Vars.Set("parse_command", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			// 目前僅回傳 0，代表解析失敗。完整實作需要複雜的語意分析。
			return &object.Integer{Value: 0}
		},
	})

	// 語法: string match_path(mapping m, string path)
	// 說明: 在 mapping 中尋找與 path 最匹配的鍵。常用於權限系統。
	obj.Vars.Set("match_path", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return &object.Nil{}
			}
			m, ok1 := args[0].(*object.Mapping)
			path, ok2 := args[1].(*object.String)
			if !ok1 || !ok2 {
				return &object.Nil{}
			}

			inputPath := path.Value
			// 🚀 關鍵修正：不強制加上 "/"，由呼叫者決定。這修正了 dbase 查詢 "id" 失敗的問題。
			/*
			if !strings.HasPrefix(inputPath, "/") {
				inputPath = "/" + inputPath
			}
			*/

			var bestMatch string
			var bestVal object.Object

			for _, pair := range m.Pairs {
				key, ok := pair.Key.(*object.String)
				if !ok {
					continue
				}

				pattern := key.Value
				if strings.HasSuffix(pattern, "/") {
					if strings.HasPrefix(inputPath, pattern) || inputPath+"/" == pattern {
						if len(pattern) > len(bestMatch) {
							bestMatch = pattern
							bestVal = pair.Value
						}
					}
				} else {
					if inputPath == pattern {
						bestMatch = pattern
						bestVal = pair.Value
						break // 精確匹配
					}
				}
			}

			if bestVal != nil {
				return bestVal
			}
			return &object.Nil{}
		},
	})
}
