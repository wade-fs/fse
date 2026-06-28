// driver/efun_type.go
package driver

import (
	"strconv"

	"mudscript/lpc-vm/object"
)

// ==========================================
// 1. 型別判斷 (Predicates)
// ==========================================
func (d *Driver) registerTypePredicates(obj *object.LPCObject) {
	// 語法: int <type>p(mixed arg)
	// 說明: 判斷傳入的變數是否為指定型別。是回傳 1，否回傳 0。
	// 範例: intp(123) -> 1; stringp(123) -> 0; nullp(0) -> 1
	register := func(name string, expectedType object.TokenType) {
		obj.Vars.Set(name, &object.Builtin{
			Fn: func(args ...object.Object) object.Object {
				if len(args) < 1 {
					if expectedType == object.NilType {
						return &object.Integer{Value: 1}
					}
					return &object.Integer{Value: 0}
				}

				arg := args[0]
				if arg == nil {
					if expectedType == object.NilType {
						return &object.Integer{Value: 1}
					}
					return &object.Integer{Value: 0}
				}

				if expectedType == object.NilType {
					if arg.TokenType() == object.NilType {
						return &object.Integer{Value: 1}
					}
					if i, ok := arg.(*object.Integer); ok && i.Value == 0 {
						return &object.Integer{Value: 1}
					}
					return &object.Integer{Value: 0}
				}

				if arg.TokenType() == expectedType {
					return &object.Integer{Value: 1}
				}
				return &object.Integer{Value: 0}
			},
		})
	}

	register("intp", object.IntegerType)
	register("stringp", object.StringType)
	register("floatp", object.FloatType)
	register("objectp", object.LPC_OBJECT_OBJ)
	register("mappingp", object.MAPPING_OBJ)
	register("mapp", object.MAPPING_OBJ) // 🚀 別名
	register("arrayp", object.ArrayType)
	register("pointerp", object.ArrayType) // 🚀 別名
	register("nullp", object.NilType)
	register("undefinedp", object.NilType)
	register("bufferp", object.BufferType)
	register("errorp", object.ErrorType)

	// 語法: int functionp(mixed arg)
	// 說明: 判斷是否為函式指標或閉包。
	// 範例: functionp((: write :)) -> 1
	obj.Vars.Set("functionp", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Integer{Value: 0}
			}
			tt := args[0].TokenType()
			if tt == object.FunctionType || tt == object.ClosureType || tt == object.BuiltinType {
				return &object.Integer{Value: 1}
			}
			return &object.Integer{Value: 0}
		},
	})

	// 語法: string typeof(mixed arg)
	// 說明: 回傳變數型別的字串名稱。
	// 範例: typeof(1) -> "int"
	obj.Vars.Set("typeof", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.String{Value: "void"}
			}
			switch args[0].TokenType() {
			case object.IntegerType:
				return &object.String{Value: "int"}
			case object.StringType:
				return &object.String{Value: "string"}
			case object.FloatType:
				return &object.String{Value: "float"}
			case object.LPC_OBJECT_OBJ:
				return &object.String{Value: "object"}
			case object.MAPPING_OBJ:
				return &object.String{Value: "mapping"}
			case object.ArrayType:
				return &object.String{Value: "array"}
			case object.BufferType:
				return &object.String{Value: "buffer"}
			case object.ErrorType:
				return &object.String{Value: "error"}
			default:
				return &object.String{Value: "unknown"}
			}
		},
	})
}

// ==========================================
// 2. 型別轉換 (Casting)
// ==========================================
func (d *Driver) registerTypeCasting(obj *object.LPCObject) {
	// 語法: int to_int(mixed arg)
	// 說明: 將浮點數或字串轉換為整數。
	// 範例: to_int("123") -> 123; to_int(3.14) -> 3
	obj.Vars.Set("to_int", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Integer{Value: 0}
			}
			switch v := args[0].(type) {
			case *object.Integer:
				return v
			case *object.Float:
				return &object.Integer{Value: int64(v.Value)}
			case *object.String:
				i, err := strconv.ParseInt(v.Value, 10, 64)
				if err != nil {
					return &object.Integer{Value: 0}
				}
				return &object.Integer{Value: i}
			default:
				return &object.Integer{Value: 0}
			}
		},
	})

	// 語法: string to_string(mixed arg)
	// 說明: 將任意變數轉換為字串表示。
	// 範例: to_string(123) -> "123"; to_string(({1, 2})) -> "[1, 2]"
	obj.Vars.Set("to_string", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 || args[0] == nil {
				return &object.String{Value: "0"}
			}
			if s, ok := args[0].(*object.String); ok {
				return s
			}
			return &object.String{Value: args[0].Inspect()}
		},
	})

	// 語法: float to_float(mixed arg)
	// 說明: 將整數或字串轉換為浮點數。
	// 範例: to_float(123) -> 123.0; to_float("3.14") -> 3.14
	obj.Vars.Set("to_float", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Float{Value: 0.0}
			}
			switch v := args[0].(type) {
			case *object.Float:
				return v
			case *object.Integer:
				return &object.Float{Value: float64(v.Value)}
			case *object.String:
				f, err := strconv.ParseFloat(v.Value, 64)
				if err != nil {
					return &object.Float{Value: 0.0}
				}
				return &object.Float{Value: f}
			default:
				return &object.Float{Value: 0.0}
			}
		},
	})
}
