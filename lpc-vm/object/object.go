package object

import (
	"bytes"
	"fmt"
	"hash/fnv"
	"strconv"
	"strings"
	"time"

	"mudscript/lpc-vm/ast"
)

// TokenType is a type of objects.
type TokenType string
const ClosureType TokenType = "CLOSURE_TYPE"

const (
	// IntegerType represents a type of integers.
	IntegerType TokenType = "Integer"
	// FloatType represents a type of floating point numbers.
	FloatType = "Float"
	// BooleanType represents a type of booleans.
	BooleanType = "Boolean"
	// NilType represents a type of nil.
	NilType = "Nil"
	// ReturnValueType represents a type of return values.
	ReturnValueType = "ReturnValue"
	// ErrorType represents a type of errors.
	ErrorType = "Error"
	// BufferType represents a type of buffers.
	BufferType TokenType = "Buffer"
	// FunctionType represents a type of functions.
	FunctionType = "Function"
	// StringType represents a type of strings.
	StringType = "String"
	// BuiltinType represents a type of builtin functions.
	BuiltinType = "Builtin"
	// ArrayType represents a type of arrays.
	ArrayType = "Array"

	////////////////////////////////////////////
	// mudscript
	LPC_OBJECT_OBJ = "LPC_OBJECT"
	MAPPING_OBJ    = "MAPPING"
	BREAK_VALUE_OBJ    = "BREAK_VALUE"
	CONTINUE_VALUE_OBJ = "CONTINUE_VALUE"
	AsyncPauseType     = "ASYNC_PAUSE"
	)

	// Object represents an object of Monkey language.
	type Object interface {
	TokenType() TokenType
	Inspect() string
	}

	// AsyncPause represents a non-blocking pause.
	type AsyncPause struct {
	Duration time.Duration
	}

	func (ap *AsyncPause) TokenType() TokenType { return AsyncPauseType }
	func (ap *AsyncPause) Inspect() string      { return fmt.Sprintf("AsyncPause(%v)", ap.Duration) }
// HashKey represents a key of a hash.
type HashKey struct {
	TokenType  TokenType
	Value uint64
}

// Hashable is the interface that is able to become a hash key.
type Hashable interface {
	HashKey() HashKey
}

// Integer represents an integer.
type Integer struct {
	Value int64
}

// TokenType returns the type of the Integer.
func (i *Integer) TokenType() TokenType {
	return IntegerType
}

// Inspect returns a string representation of the Integer.
func (i *Integer) Inspect() string {
	return strconv.FormatInt(i.Value, 10)
}

// HashKey returns a hash key object for i.
func (i *Integer) HashKey() HashKey {
	return HashKey{
		TokenType:  i.TokenType(),
		Value: uint64(i.Value),
	}
}

// Float represents an integer.
type Float struct {
	Value float64
}

// TokenType returns the type of f.
func (f *Float) TokenType() TokenType {
	return FloatType
}

// Inspect returns a string representation of f.
func (f *Float) Inspect() string {
	return strconv.FormatFloat(f.Value, 'f', -1, 64)
}

// HashKey returns a hash key object for f.
func (f *Float) HashKey() HashKey {
	s := strconv.FormatFloat(f.Value, 'f', -1, 64)
	h := fnv.New64a()
	h.Write([]byte(s))

	return HashKey{
		TokenType:  f.TokenType(),
		Value: h.Sum64(),
	}
}

// Boolean represents a boolean.
type Boolean struct {
	Value bool
}

// TokenType returns the type of the Boolean.
func (b *Boolean) TokenType() TokenType {
	return BooleanType
}

// Inspect returns a string representation of the Boolean.
func (b *Boolean) Inspect() string {
	return strconv.FormatBool(b.Value)
}

// HashKey returns a hash key object for b.
func (b *Boolean) HashKey() HashKey {
	key := HashKey{TokenType: b.TokenType()}
	if b.Value {
		key.Value = 1
	}
	return key
}

// Nil represents the absence of any value.
type Nil struct{}

// TokenType returns the type of the Nil.
func (n *Nil) TokenType() TokenType {
	return NilType
}

// Inspect returns a string representation of the Nil.
func (n *Nil) Inspect() string {
	return "nil"
}

// HashKey returns a hash key object for n.
func (n *Nil) HashKey() HashKey {
	return HashKey{
		TokenType: n.TokenType(),
		Value:     0,
	}
}

// ReturnValue represents a return value.
type ReturnValue struct {
	Value Object
}

// TokenType returns the type of the ReturnValue.
func (rv *ReturnValue) TokenType() TokenType {
	return ReturnValueType
}

// Inspect returns a string representation of the ReturnValue.
func (rv *ReturnValue) Inspect() string {
	return rv.Value.Inspect()
}

// Error represents an error.
type Error struct {
	Message string
}

func NewError(format string, a ...interface{}) *Error {
	return &Error{Message: fmt.Sprintf(format, a...)}
}

// TokenType returns the type of the Error.
func (e *Error) TokenType() TokenType {
	return ErrorType
}

// Inspect returns a string representation of the Error.
func (e *Error) Inspect() string {
	return "Error: " + e.Message
}

// Buffer represents a buffer.
type Buffer struct {
	Value []byte
}

// ReverseIndex represents a reverse index (e.g. <1)
type ReverseIndex struct {
	Value int64
}

// TokenType returns the type of the ReverseIndex.
func (ri *ReverseIndex) TokenType() TokenType {
	return "REVERSE_INDEX"
}

// Inspect returns a string representation of the ReverseIndex.
func (ri *ReverseIndex) Inspect() string {
	return fmt.Sprintf("<%d", ri.Value)
}

// TokenType returns the type of the Buffer.
func (b *Buffer) TokenType() TokenType {
	return BufferType
}

// Inspect returns a string representation of the Buffer.
func (b *Buffer) Inspect() string {
	return fmt.Sprintf("<buffer:%d>", len(b.Value))
}

// HashKey returns a hash key object for b.
func (b *Buffer) HashKey() HashKey {
	h := fnv.New64a()
	h.Write(b.Value)

	return HashKey{
		TokenType: b.TokenType(),
		Value:     h.Sum64(),
	}
}

// Function represents a function.
type Function struct {
	IsVarargs  bool // 🚀 新增：是否支援不定長度參數
	Parameters []*ast.Ident
	Body       *ast.BlockStatement
	Env        Environment
	OriginFile string // 🚀 新增：記錄此函式定義於哪個檔案
}

// TokenType returns the type of the Function.
func (f *Function) TokenType() TokenType {
	return FunctionType
}

// Inspect returns a string representation of the Function.
func (f *Function) Inspect() string {
	return fmt.Sprintf("<function defined in %s>", f.OriginFile)
}

func (f *Function) HashKey() HashKey {
	h := fnv.New64a()
	// 利用 fmt.Sprintf("%p") 取得這個函式在 Go 記憶體中的唯一指標位址
	h.Write([]byte(fmt.Sprintf("%p", f))) 
	return HashKey{
		TokenType: f.TokenType(),
		Value:     h.Sum64(),
	}
}

// String represents a string.
type String struct {
	Value string
}

// TokenType returns the type of the String.
func (s *String) TokenType() TokenType {
	return StringType
}

// Inspect returns a string representation of the String.
func (s *String) Inspect() string {
	return `"` + s.Value + `"`
}

// HashKey returns a hash key object for s.
func (s *String) HashKey() HashKey {
	h := fnv.New64a()
	h.Write([]byte(s.Value))

	return HashKey{
		TokenType:  s.TokenType(),
		Value: h.Sum64(),
	}
}

// BuiltinFunction represents a function signature of builtin functions.
type BuiltinFunction func(args ...Object) Object

// Builtin represents a builtin function.
type Builtin struct {
	Fn         BuiltinFunction
	IsConstant bool // 🚀 新增：標記為常數，會在 evalIdent 中自動展開
}

// TokenType returns the type of the Builtin.
func (b *Builtin) TokenType() TokenType {
	return BuiltinType
}

// Inspect returns a string representation of the Builtin.
func (b *Builtin) Inspect() string {
	return "builtin function"
}

func (b *Builtin) HashKey() HashKey {
	h := fnv.New64a()
	h.Write([]byte(fmt.Sprintf("%p", b)))
	return HashKey{
		TokenType: b.TokenType(),
		Value:     h.Sum64(),
	}
}

// Array represents an array.
type Array struct {
	Elements []Object
}

// TokenType returns the type of the Array.
func (*Array) TokenType() TokenType {
	return ArrayType
}

// Inspect returns a string representation of the Array.
func (a *Array) Inspect() string {
	if a == nil {
		return ""
	}

	elements := make([]string, 0, len(a.Elements))
	for _, e := range a.Elements {
		elements = append(elements, e.Inspect())
	}

	var out bytes.Buffer
	out.WriteString("({ ")
	out.WriteString(strings.Join(elements, ", "))
	out.WriteString(" })")
	return out.String()
}

// HashPair represents a key-value pair in a mapping.
type HashPair struct {
	Key   Object
	Value Object
}

////////////////////////////////////////////////
// mudscript

// 玩家/生物身上的指令結構
type Action struct {
	Verb     string      // 指令動詞 (例如 "get", "look")
	FuncName string      // 對應的 LPC 函式名稱 (例如 "do_get")
	Provider *LPCObject  // 提供這個指令的物件 (例如「蘋果」或「房間」)
	Flags    int         // 🚀 新增：支援 LPC add_action 的 flags (如 1 代表前綴匹配)
}

// LPCObject: 代表一個載入記憶體中的 LPC 物件實體 (.c 檔)
type LPCObject struct {
UUID        string // 🚀 新增：全域唯一識別碼
Filename    string
Vars        Environment
Efuns       Environment // 🚀 新增：原始 Efun 表 (用於 efun:: 呼叫)

	Functions   map[string]*Function
	Inherits    []*LPCObject
	
	// --- 空間與生命週期狀態 ---
	Location    *LPCObject   // 這個物件目前在哪裡？ (例如在某個房間，或某個玩家身上)
	Inventory   []*LPCObject // 這個物件裡面裝了什麼？
	IsDestructed bool        // 標記是否已經被摧毀

	// 生物與互動標籤
	IsLiving      bool               // 是否為活物 (可以接收指令)
	IsInteractive bool               // 是否為線上玩家 (背後有 TCP 連線)
	IsWizard      bool               // 🚀 新增：是否為巫師 (管理者)
	Actions       map[string]*Action // 該生物目前可用的指令表
	Light         int                // 🚀 新增：光照度
	
	// 🚀 新增：生命週期管理
	LastActivity int64 // 最後活動時間 (Unix Timestamp)

	ShadowedObject *LPCObject // 🚀 Shadowed 物件
	}

// TokenType implements Object interface
func (o *LPCObject) TokenType() TokenType {
	return LPC_OBJECT_OBJ
}

func (o *LPCObject) Inspect() string {
	return "<object: " + o.Filename + ">"
}

func (o *LPCObject) HashKey() HashKey {
	h := fnv.New64a()
	// LPC 物件因為檔名加上 CloneID 是獨一無二的，所以我們 Hash 它的檔名
	h.Write([]byte(o.Filename))
	return HashKey{
		TokenType: o.TokenType(),
		Value:     h.Sum64(),
	}
}

// Mapping: LPC 專用的 mapping 結構
type Mapping struct {
	Pairs    map[HashKey]HashPair
	Iterator []HashKey // 🚀 新增：支援 each() 的迭代器狀態
	Idx      int       // 🚀 新增：目前迭代到的位置
}

// TokenType implements Object interface
func (m *Mapping) TokenType() TokenType {
	return MAPPING_OBJ
}

func (m *Mapping) Inspect() string {
	var out bytes.Buffer
	var pairs []string
	for _, pair := range m.Pairs {
		pairs = append(pairs, pair.Key.Inspect()+": "+pair.Value.Inspect())
	}
	out.WriteString("([ ")
	out.WriteString(strings.Join(pairs, ", "))
	out.WriteString(" ])")
	return out.String()
}

// BreakValue: 代表遇到了 break;
type BreakValue struct{}
func (b *BreakValue) TokenType() TokenType { return BREAK_VALUE_OBJ }
func (b *BreakValue) Inspect() string { return "break" }

// ContinueValue: 代表遇到了 continue;
type ContinueValue struct{}
func (c *ContinueValue) TokenType() TokenType { return CONTINUE_VALUE_OBJ }
func (c *ContinueValue) Inspect() string { return "continue" }

// Closure 表示 LPC 的函數指標或 Lambda
type Closure struct {
	Target    *LPCObject // 目標物件 (如果是 nil，且非 Lambda，代表呼叫時的 this_object)
	FuncName  string     // 函式名稱 (用於一般閉包)
	BoundArgs []Object   // 預先綁定的參數 (Currying)
	
	// 🚀 新增：支援 Lambda
	Lambda      ast.Expression   // 如果不為 nil，則這是一個單一表達式的 Lambda
	Expressions []ast.Expression // 支援多個表達式
	Parameters  []*ast.TypedVarDecl // 支援具名型別參數
	Env         Environment      // Lambda 執行時的環境上下文
}
func (c *Closure) TokenType() TokenType { return ClosureType }
func (c *Closure) Inspect() string {
	if c.Lambda != nil {
		return fmt.Sprintf("(: %s :)", c.Lambda.String())
	}
	target := "this_object"
	if c.Target != nil { target = c.Target.Filename }
	return fmt.Sprintf("(: %s, \"%s\", ...%d args :)", target, c.FuncName, len(c.BoundArgs))
}
