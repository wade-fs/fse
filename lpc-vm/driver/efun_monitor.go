// driver/efun_monitor.go
package driver

import (
	"os"
	"time"

	"mudscript/lpc-vm/evaluator"
	"mudscript/lpc-vm/object"
)

func (d *Driver) registerPerformanceEfuns(obj *object.LPCObject) {
	// 語法: string query_host_name()
	obj.Vars.Set("query_host_name", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			hostname, _ := os.Hostname()
			return &object.String{Value: hostname}
		},
	})

	// 語法: float query_load_average()
	obj.Vars.Set("query_load_average", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.Float{Value: 0.0} // Placeholder
		},
	})

	// 語法: int time_expression(closure f)
	obj.Vars.Set("time_expression", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Integer{Value: 0}
			}
			start := time.Now()
			
			// 執行閉包
			switch f := args[0].(type) {
			case *object.Closure:
				d.CallFunction(f.Target, f.FuncName, f.BoundArgs)
			case *object.Builtin:
				f.Fn()
			}

			elapsed := time.Since(start).Microseconds()
			return &object.Integer{Value: int64(elapsed)}
		},
	})

	// 語法: void set_eval_limit(int limit)
	obj.Vars.Set("set_eval_limit", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return evaluator.NilValue
		},
	})

	// 語法: int eval_cost()
	obj.Vars.Set("eval_cost", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.Integer{Value: 0}
		},
	})

	// 語法: int max_eval_cost()
	obj.Vars.Set("max_eval_cost", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.Integer{Value: 1000000} // Placeholder for max eval cost
		},
	})

	// 語法: void reset_eval_cost()
	obj.Vars.Set("reset_eval_cost", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return evaluator.NilValue
		},
	})

	// 語法: mapping author_stats()
	obj.Vars.Set("author_stats", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.Mapping{Pairs: make(map[object.HashKey]object.HashPair)}
		},
	})

	// 語法: mapping domain_stats()
	obj.Vars.Set("domain_stats", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.Mapping{Pairs: make(map[object.HashKey]object.HashPair)}
		},
	})

	// 語法: void set_author(string name)
	obj.Vars.Set("set_author", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return evaluator.NilValue
		},
	})

	// 語法: void opcprof([string file])
	obj.Vars.Set("opcprof", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			d.TellObject(obj, "Opcode profiling is not supported in this version.\n")
			return &object.Nil{}
		},
	})

	// 語法: void trace(int level)
	obj.Vars.Set("trace", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.Nil{}
		},
	})

	// 語法: void traceprefix(string prefix)
	obj.Vars.Set("traceprefix", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.Nil{}
		},
	})

	// 語法: void moncontrol(int flag)
	obj.Vars.Set("moncontrol", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.Nil{}
		},
	})
}
