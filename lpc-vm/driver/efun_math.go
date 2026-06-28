// driver/efun_math.go
package driver

import (
	"math"
	"math/rand"
	"time"

	"mudscript/lpc-vm/object"
)

// ==========================================
// 3. 數學運算 (Math)
// ==========================================
func (d *Driver) registerMathEfuns(obj *object.LPCObject) {
	// 語法: int abs(int num)
	// 說明: 取得整數的絕對值。
	// 範例: abs(-5) -> 5
	obj.Vars.Set("abs", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Integer{Value: 0}
			}
			if i, ok := args[0].(*object.Integer); ok {
				if i.Value < 0 {
					return &object.Integer{Value: -i.Value}
				}
				return i
			}
			return &object.Integer{Value: 0}
		},
	})
	// 語法: int max(int a, int b) / int min(int a, int b)
	// 說明: 回傳兩數之中的最大/最小值。
	// 範例: max(10, 20) -> 20; min(10, 20) -> 10
	for _, name := range []string{"max", "min"} {
		op := name
		obj.Vars.Set(op, &object.Builtin{
			Fn: func(args ...object.Object) object.Object {
				if len(args) < 2 {
					return &object.Integer{Value: 0}
				}
				v1, ok1 := args[0].(*object.Integer)
				v2, ok2 := args[1].(*object.Integer)
				if !ok1 || !ok2 {
					return &object.Integer{Value: 0}
				}
				if (op == "max" && v1.Value > v2.Value) || (op == "min" && v1.Value < v2.Value) {
					return v1
				}
				return v2
			},
		})
	}
	// 語法: int random(int max)
	// 說明: 產生 0 到 max-1 的隨機整數。
	// 範例: random(10) -> 可能回傳 0 ~ 9 之間的任意數字
	obj.Vars.Set("random", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) != 1 || args[0].TokenType() != object.IntegerType {
				return object.NewError("argument to `random` must be INTEGER")
			}
			max := args[0].(*object.Integer).Value
			if max <= 0 {
				return &object.Integer{Value: 0}
			}
			return &object.Integer{Value: rand.Int63n(max)}
		},
	})
	obj.Vars.Set("sqrt", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Float{Value: 0.0}
			}
			v, ok := args[0].(*object.Float)
			if !ok {
				return &object.Float{Value: 0.0}
			}
			return &object.Float{Value: math.Sqrt(v.Value)}
		},
	})
	obj.Vars.Set("pow", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return &object.Float{Value: 0.0}
			}
			v1, ok1 := args[0].(*object.Float)
			v2, ok2 := args[1].(*object.Float)
			if !ok1 || !ok2 {
				return &object.Float{Value: 0.0}
			}
			return &object.Float{Value: math.Pow(v1.Value, v2.Value)}
		},
	})
	obj.Vars.Set("log", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Float{Value: 0.0}
			}
			v, ok := args[0].(*object.Float)
			if !ok {
				return &object.Float{Value: 0.0}
			}
			return &object.Float{Value: math.Log(v.Value)}
		},
	})
	obj.Vars.Set("exp", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Float{Value: 0.0}
			}
			v, ok := args[0].(*object.Float)
			if !ok {
				return &object.Float{Value: 0.0}
			}
			return &object.Float{Value: math.Exp(v.Value)}
		},
	})
	for _, name := range []string{"sin", "cos", "tan", "asin", "acos", "atan", "floor", "ceil"} {
		funcName := name
		obj.Vars.Set(funcName, &object.Builtin{
			Fn: func(args ...object.Object) object.Object {
				if len(args) < 1 {
					return &object.Float{Value: 0.0}
				}
				v, ok := args[0].(*object.Float)
				if !ok {
					return &object.Float{Value: 0.0}
				}
				var res float64
				switch funcName {
				case "sin":
					res = math.Sin(v.Value)
				case "cos":
					res = math.Cos(v.Value)
				case "tan":
					res = math.Tan(v.Value)
				case "asin":
					res = math.Asin(v.Value)
				case "acos":
					res = math.Acos(v.Value)
				case "atan":
					res = math.Atan(v.Value)
				case "floor":
					res = math.Floor(v.Value)
				case "ceil":
					res = math.Ceil(v.Value)
				}
				return &object.Float{Value: res}
			},
		})
	}

	// System Monitor
	obj.Vars.Set("uptime", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.Integer{Value: int64(time.Since(d.StartTime).Seconds())}
		},
	})
}
