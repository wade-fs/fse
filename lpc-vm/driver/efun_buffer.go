// driver/efun_buffer.go
package driver

import (
	"fmt"
	"os"
	"path/filepath"

	"mudscript/lpc-vm/object"
)

func (d *Driver) registerBufferEfuns(obj *object.LPCObject) {
	// 語法: buffer allocate_buffer(int size)
	// 說明: 配置指定大小的 buffer。
	obj.Vars.Set("allocate_buffer", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			size, ok := args[0].(*object.Integer)
			if !ok || size.Value < 0 {
				return &object.Nil{}
			}
			return &object.Buffer{Value: make([]byte, size.Value)}
		},
	})

	// 語法: mixed read_buffer(buffer b, int start, int len)
	// 說明: 從 buffer 中讀取內容。如果 len 為 1，傳回該位元組的整數值；否則傳回字串。
	obj.Vars.Set("read_buffer", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			buf, ok := args[0].(*object.Buffer)
			if !ok {
				return &object.Nil{}
			}

			start := 0
			if len(args) > 1 {
				if s, ok := args[1].(*object.Integer); ok {
					start = int(s.Value)
				}
			}

			length := len(buf.Value) - start
			if len(args) > 2 {
				if l, ok := args[2].(*object.Integer); ok {
					length = int(l.Value)
				}
			}

			if start < 0 || start >= len(buf.Value) || length <= 0 {
				return &object.Nil{}
			}

			end := start + length
			if end > len(buf.Value) {
				end = len(buf.Value)
			}

			data := buf.Value[start:end]
			if length == 1 {
				return &object.Integer{Value: int64(data[0])}
			}
			return &object.String{Value: string(data)}
		},
	})

	// 語法: int write_buffer(buffer dest, int start, mixed source)
	// 說明: 將 source (字串、整數或另一個 buffer) 寫入 dest buffer。
	obj.Vars.Set("write_buffer", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 3 {
				return &object.Integer{Value: 0}
			}
			dest, ok := args[0].(*object.Buffer)
			if !ok {
				return &object.Integer{Value: 0}
			}

			start, ok := args[1].(*object.Integer)
			if !ok || start.Value < 0 || start.Value >= int64(len(dest.Value)) {
				return &object.Integer{Value: 0}
			}

			pos := int(start.Value)
			var sourceData []byte

			switch s := args[2].(type) {
			case *object.String:
				sourceData = []byte(s.Value)
			case *object.Buffer:
				sourceData = s.Value
			case *object.Integer:
				sourceData = []byte{byte(s.Value)}
			default:
				return &object.Integer{Value: 0}
			}

			n := copy(dest.Value[pos:], sourceData)
			return &object.Integer{Value: int64(n)}
		},
	})

	// 語法: buffer read_bytes(string file, int start, int length)
	// 說明: 從檔案中讀取指定範圍的位元組。
	obj.Vars.Set("read_bytes", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			file, ok := args[0].(*object.String)
			if !ok {
				return &object.Nil{}
			}

			start := int64(0)
			if len(args) > 1 {
				if s, ok := args[1].(*object.Integer); ok {
					start = s.Value
				}
			}

			length := int64(0)
			if len(args) > 2 {
				if l, ok := args[2].(*object.Integer); ok {
					length = l.Value
				}
			}

			resolvedPath := d.ResolvePath(obj.Filename, file.Value)
			allowed, errMsg := d.checkReadPermission(obj, resolvedPath, "read_bytes")
			if !allowed {
				if p := d.GetCurrentPlayer(); p != nil {
					p.Send(fmt.Sprintf("\r\n⚠️ 系統安全攔截：%s\r\n", errMsg))
				}
				return &object.Nil{}
			}

			fullPath := filepath.Join(d.Config.MudLibPath, resolvedPath)
			f, err := os.Open(fullPath)
			if err != nil {
				return &object.Nil{}
			}
			defer f.Close()

			info, _ := f.Stat()
			if start < 0 {
				start = info.Size() + start
			}
			if start < 0 || start >= info.Size() {
				return &object.Nil{}
			}

			if length <= 0 {
				length = info.Size() - start
			}

			data := make([]byte, length)
			n, err := f.ReadAt(data, start)
			if err != nil && n == 0 {
				return &object.Nil{}
			}
			return &object.Buffer{Value: data[:n]}
		},
	})

	// 語法: int write_bytes(string file, int start, mixed series)
	// 說明: 將 series (字串或 buffer) 寫入檔案指定位置。
	obj.Vars.Set("write_bytes", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 3 {
				return &object.Integer{Value: 0}
			}
			file, ok := args[0].(*object.String)
			if !ok {
				return &object.Integer{Value: 0}
			}

			start, ok := args[1].(*object.Integer)
			if !ok {
				return &object.Integer{Value: 0}
			}

			var sourceData []byte
			switch s := args[2].(type) {
			case *object.String:
				sourceData = []byte(s.Value)
			case *object.Buffer:
				sourceData = s.Value
			default:
				return &object.Integer{Value: 0}
			}

			resolvedPath := d.ResolvePath(obj.Filename, file.Value)
			allowed, errMsg := d.checkWritePermission(obj, resolvedPath, "write_bytes")
			if !allowed {
				if p := d.GetCurrentPlayer(); p != nil {
					p.Send(fmt.Sprintf("\r\n⚠️ 系統安全攔截：%s\r\n", errMsg))
				}
				return &object.Integer{Value: 0}
			}

			fullPath := filepath.Join(d.Config.MudLibPath, resolvedPath)
			f, err := os.OpenFile(fullPath, os.O_RDWR|os.O_CREATE, 0644)
			if err != nil {
				return &object.Integer{Value: 0}
			}
			defer f.Close()

			info, _ := f.Stat()
			pos := start.Value
			if pos < 0 {
				pos = info.Size() + pos
			}
			if pos < 0 {
				pos = 0
			}

			n, err := f.WriteAt(sourceData, pos)
			if err != nil {
				return &object.Integer{Value: 0}
			}
			return &object.Integer{Value: int64(n)}
		},
	})
}
