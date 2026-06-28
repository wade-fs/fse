// driver/efun_bit.go
package driver

import (
	"hash/crc32"

	"mudscript/lpc-vm/object"
)

func (d *Driver) registerBitEfuns(obj *object.LPCObject) {
	// 語法: string set_bit(string str, int n)
	// 說明: 在位元字串 str 中設定第 n 個位元。
	obj.Vars.Set("set_bit", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return &object.Nil{}
			}
			str, ok1 := args[0].(*object.String)
			n, ok2 := args[1].(*object.Integer)
			if !ok1 || !ok2 {
				return &object.Nil{}
			}

			bit := int(n.Value)
			if bit < 0 {
				return str
			}

			charIdx := bit / 6
			bitIdx := bit % 6

			bytes := []byte(str.Value)
			for len(bytes) <= charIdx {
				bytes = append(bytes, ' ')
			}

			val := bytes[charIdx] - 32
			val |= (1 << uint(bitIdx))
			bytes[charIdx] = val + 32

			return &object.String{Value: string(bytes)}
		},
	})

	// 語法: string clear_bit(string str, int n)
	// 說明: 在位元字串 str 中清除第 n 個位元。
	obj.Vars.Set("clear_bit", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return &object.Nil{}
			}
			str, ok1 := args[0].(*object.String)
			n, ok2 := args[1].(*object.Integer)
			if !ok1 || !ok2 {
				return &object.Nil{}
			}

			bit := int(n.Value)
			if bit < 0 {
				return str
			}

			charIdx := bit / 6
			bitIdx := bit % 6

			bytes := []byte(str.Value)
			if charIdx >= len(bytes) {
				return str
			}

			val := bytes[charIdx] - 32
			val &= ^(1 << uint(bitIdx))
			bytes[charIdx] = val + 32

			return &object.String{Value: string(bytes)}
		},
	})

	// 語法: int test_bit(string str, int n)
	// 說明: 測試位元字串 str 中的第 n 個位元。
	obj.Vars.Set("test_bit", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return &object.Integer{Value: 0}
			}
			str, ok1 := args[0].(*object.String)
			n, ok2 := args[1].(*object.Integer)
			if !ok1 || !ok2 {
				return &object.Integer{Value: 0}
			}

			bit := int(n.Value)
			if bit < 0 {
				return &object.Integer{Value: 0}
			}

			charIdx := bit / 6
			bitIdx := bit % 6

			bytes := []byte(str.Value)
			if charIdx >= len(bytes) {
				return &object.Integer{Value: 0}
			}

			val := bytes[charIdx] - 32
			if (val & (1 << uint(bitIdx))) != 0 {
				return &object.Integer{Value: 1}
			}
			return &object.Integer{Value: 0}
		},
	})

	// 語法: int crc32(string | buffer b)
	// 說明: 計算 32 位元循環冗餘檢查值。
	obj.Vars.Set("crc32", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Integer{Value: 0}
			}
			var data []byte
			switch v := args[0].(type) {
			case *object.String:
				data = []byte(v.Value)
			case *object.Buffer:
				data = v.Value
			default:
				return &object.Integer{Value: 0}
			}

			return &object.Integer{Value: int64(crc32.ChecksumIEEE(data))}
		},
	})
}
