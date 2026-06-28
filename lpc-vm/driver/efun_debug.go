// driver/efun_debug.go
package driver

import (
	"fmt"
	"runtime"
	"strings"

	"mudscript/lpc-vm/object"
)

func (d *Driver) registerDebugEfuns(obj *object.LPCObject) {
	// 語法: mixed debug_info(int flag, mixed arg)
	// 說明: 取得物件或驅動程式的偵錯資訊。
	obj.Vars.Set("debug_info", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			flag, ok := args[0].(*object.Integer)
			if !ok {
				return &object.Nil{}
			}

			switch flag.Value {
			case 0: // 物件詳細資訊
				target := obj
				if len(args) > 1 {
					if t, ok := args[1].(*object.LPCObject); ok {
						target = t
					}
				}
				res := fmt.Sprintf("Object: %s\nUUID: %s\nHeartbeat: %v\nLiving: %v\nWizard: %v\n",
					target.Filename, target.UUID, d.Heartbeats[target], target.IsLiving, target.IsWizard)
				return &object.String{Value: res}
			case 1: // 記憶體狀態
				var stats runtime.MemStats
				runtime.ReadMemStats(&stats)
				res := fmt.Sprintf("Alloc: %d\nTotalAlloc: %d\nSys: %d\nNumGC: %d\nHeapObjects: %d\n",
					stats.Alloc, stats.TotalAlloc, stats.Sys, stats.NumGC, stats.HeapObjects)
				return &object.String{Value: res}
			case 2: // 所有物件清單摘要
				d.mu.RLock()
				defer d.mu.RUnlock()
				var lines []string
				for path := range d.ObjectTable {
					lines = append(lines, path)
				}
				return &object.String{Value: strings.Join(lines, "\n")}
			}
			return &object.Nil{}
		},
	})

	// 語法: void malloc_status()
	obj.Vars.Set("malloc_status", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			var stats runtime.MemStats
			runtime.ReadMemStats(&stats)
			msg := fmt.Sprintf("Memory Status:\n  Allocated: %v bytes\n  Total Allocated: %v bytes\n  System: %v bytes\n  Num GC: %v\n",
				stats.Alloc, stats.TotalAlloc, stats.Sys, stats.NumGC)
			d.TellObject(obj, msg)
			return &object.Nil{}
		},
	})

	// 語法: int memory_info([object ob])
	obj.Vars.Set("memory_info", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			// Go 難以精確計算單一物件大小，此處回傳預估值或全域值
			if len(args) == 0 {
				var stats runtime.MemStats
				runtime.ReadMemStats(&stats)
				return &object.Integer{Value: int64(stats.Alloc)}
			}
			return &object.Integer{Value: 1024} // Placeholder for object size
		},
	})

	// 語法: void function_profile(object ob)
	obj.Vars.Set("function_profile", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			// 佔位符
			d.TellObject(obj, "Function profiling is not supported yet.\n")
			return &object.Nil{}
		},
	})

	// 語法: int refs(mixed arg)
	obj.Vars.Set("refs", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			// Go 自動垃圾回收，此處僅傳回 1 代表有引用
			return &object.Integer{Value: 1}
		},
	})

	// 語法: void dumpallobj()
	obj.Vars.Set("dumpallobj", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			d.mu.RLock()
			defer d.mu.RUnlock()
			for path, ob := range d.ObjectTable {
				d.TellObject(obj, fmt.Sprintf("%s: %s\n", path, ob.UUID))
			}
			return &object.Nil{}
		},
	})

	// 語法: void dump_prog(object ob, [string file])
	obj.Vars.Set("dump_prog", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			target := getTarget(args, obj)
			d.TellObject(obj, fmt.Sprintf("Program dump for %s:\n  Functions: %d\n", target.Filename, len(target.Functions)))
			return &object.Nil{}
		},
	})
}
