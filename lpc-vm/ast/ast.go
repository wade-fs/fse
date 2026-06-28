package ast

import (
	"bytes"
	"strings"

	"mudscript/lpc-vm/token"
)

// Node represents an AST node.
type Node interface {
	TokenLiteral() string
	String() string
}

// Statement represents a statement.
type Statement interface {
	Node
	statementNode()
}

// Expression represents an expression.
type Expression interface {
	Node
	expressionNode()
}

// Program is a top-level AST node of a program.
type Program struct {
	Statements []Statement
}

// TokenLiteral returns the first token literal of a program.
func (p *Program) TokenLiteral() string {
	if len(p.Statements) == 0 {
		return ""
	}
	return p.Statements[0].TokenLiteral()
}

func (p *Program) String() string {
	var out bytes.Buffer

	for _, s := range p.Statements {
		out.WriteString(s.String())
	}

	return out.String()
}

// Ident represents an identifier.
type Ident struct {
	Token token.Token // the token.IDENT token
	Value string
}

func (i *Ident) expressionNode() {}

// TokenLiteral returns a token literal of an identifier.
func (i *Ident) TokenLiteral() string {
	return i.Token.Literal
}

func (i *Ident) String() string {
	return i.Value
}

// ReturnStatement represents a return statement.
type ReturnStatement struct {
	Token       token.Token // the token.RETURN token
	ReturnValue Expression
}

func (rs *ReturnStatement) statementNode()  {}
func (rs *ReturnStatement) expressionNode() {}

// TokenLiteral returns a token literal of return statement.
func (rs *ReturnStatement) TokenLiteral() string {
	return rs.Token.Literal
}

func (rs *ReturnStatement) String() string {
	var out bytes.Buffer

	out.WriteString(rs.TokenLiteral() + " ")

	if rs.ReturnValue != nil {
		out.WriteString(rs.ReturnValue.String())
	}

	out.WriteString(";")

	return out.String()
}

// ExpressionStatement represents an expression statement.
type ExpressionStatement struct {
	Token      token.Token // the first token of the expression
	Expression Expression
}

func (es *ExpressionStatement) statementNode() {}

// TokenLiteral returns a token literal of expression statement.
func (es *ExpressionStatement) TokenLiteral() string {
	return es.Token.Literal
}

func (es *ExpressionStatement) String() string {
	if es.Expression != nil {
		return es.Expression.String()
	}

	return ""
}

// IntegerLiteral represents an integer literal.
type IntegerLiteral struct {
	Token token.Token
	Value int64
}

func (il *IntegerLiteral) expressionNode() {}

// TokenLiteral returns a token literal of integer.
func (il *IntegerLiteral) TokenLiteral() string {
	return il.Token.Literal
}

func (il *IntegerLiteral) String() string {
	return il.Token.Literal
}

// FloatLiteral represents a floating point number literal.
type FloatLiteral struct {
	Token token.Token
	Value float64
}

func (fl *FloatLiteral) expressionNode() {}

// TokenLiteral returns a token literal of floating point number.
func (fl *FloatLiteral) TokenLiteral() string {
	return fl.Token.Literal
}

func (fl *FloatLiteral) String() string {
	return fl.Token.Literal
}

// PrefixExpression represents a prefix expression.
type PrefixExpression struct {
	Token    token.Token // The prefix token, e.g. !
	Operator string
	Right    Expression
}

func (pe *PrefixExpression) expressionNode() {}

// TokenLiteral returns a token literal.
func (pe *PrefixExpression) TokenLiteral() string {
	return pe.Token.Literal
}

func (pe *PrefixExpression) String() string {
	var out bytes.Buffer

	out.WriteString("(")
	out.WriteString(pe.Operator)
	out.WriteString(pe.Right.String())
	out.WriteString(")")

	return out.String()
}

// InfixExpression represents an infix expression.
type InfixExpression struct {
	Token    token.Token // The operator token, e.g. +
	Left     Expression
	Operator string
	Right    Expression
}

func (ie *InfixExpression) expressionNode() {}

// TokenLiteral returns a token literal.
func (ie *InfixExpression) TokenLiteral() string {
	return ie.Token.Literal
}

func (ie *InfixExpression) String() string {
	var out bytes.Buffer

	out.WriteString("(")
	out.WriteString(ie.Left.String())
	out.WriteString(" " + ie.Operator + " ")
	out.WriteString(ie.Right.String())
	out.WriteString(")")

	return out.String()
}

// Boolean represents a boolean value.
type Boolean struct {
	Token token.Token
	Value bool
}

func (b *Boolean) expressionNode() {}

// TokenLiteral returns a token literal of boolean value.
func (b *Boolean) TokenLiteral() string {
	return b.Token.Literal
}

func (b *Boolean) String() string {
	return b.Token.Literal
}

// IfExpression represents an if expression.
type IfExpression struct {
	Token       token.Token // The 'if' token
	Condition   Expression
	Consequence *BlockStatement
	Alternative *BlockStatement
}

func (ie *IfExpression) expressionNode() {}

// TokenLiteral returns a token literal of if expression.
func (ie *IfExpression) TokenLiteral() string {
	return ie.Token.Literal
}

func (ie *IfExpression) String() string {
	var out bytes.Buffer

	out.WriteString("if")
	out.WriteString(ie.Condition.String())
	out.WriteString(" ")
	out.WriteString(ie.Consequence.String())

	if ie.Alternative != nil {
		out.WriteString("else ")
		out.WriteString(ie.Alternative.String())
	}

	return out.String()
}

// BlockStatement represents a block statement.
type BlockStatement struct {
	Token      token.Token // the '{' token
	Statements []Statement
}

func (bs *BlockStatement) statementNode()  {}
func (bs *BlockStatement) expressionNode() {}

// TokenLiteral returns a token literal of block statement.
func (bs *BlockStatement) TokenLiteral() string {
	return bs.Token.Literal
}

func (bs *BlockStatement) String() string {
	var out bytes.Buffer

	for _, s := range bs.Statements {
		out.WriteString(s.String())
	}

	return out.String()
}

// CallExpression represents a function call expression.
type CallExpression struct {
	Token     token.Token // the '(' token
	Function  Expression  // Ident or ClosureLiteral
	Arguments []Expression
}

func (ce *CallExpression) expressionNode() {}

// TokenLiteral returns a token literal of function call expression.
func (ce *CallExpression) TokenLiteral() string {
	return ce.Token.Literal
}

func (ce *CallExpression) String() string {
	var out bytes.Buffer

	args := make([]string, 0, len(ce.Arguments))
	for _, arg := range ce.Arguments {
		args = append(args, arg.String())
	}

	out.WriteString(ce.Function.String())
	out.WriteString("(")
	out.WriteString(strings.Join(args, ", "))
	out.WriteString(")")

	return out.String()
}

// StringLiteral represents a string literal.
type StringLiteral struct {
	Token token.Token
	Value string
}

func (sl *StringLiteral) expressionNode() {}

// TokenLiteral returns a token literal of string.
func (sl *StringLiteral) TokenLiteral() string {
	if sl == nil {
		return ""
	}
	return sl.Token.Literal
}

func (sl *StringLiteral) String() string {
	return sl.TokenLiteral()
}

// ArrayLiteral represents an array literal.
type ArrayLiteral struct {
	Token    token.Token // the '({ ' token
	Elements []Expression
}

func (*ArrayLiteral) expressionNode() {}

// TokenLiteral returns a token literal of array.
func (al *ArrayLiteral) TokenLiteral() string {
	if al == nil {
		return ""
	}
	return al.Token.Literal
}

func (al *ArrayLiteral) String() string {
	if al == nil {
		return ""
	}

	elements := make([]string, 0, len(al.Elements))
	for _, el := range al.Elements {
		elements = append(elements, el.String())
	}

	var out bytes.Buffer

	out.WriteString("({ ")
	out.WriteString(strings.Join(elements, ", "))
	out.WriteString(" })")

	return out.String()
}

// IndexExpression represents an expression in array index operator.
type IndexExpression struct {
	Token token.Token // the '[' token
	Left  Expression
	Index Expression
}

func (*IndexExpression) expressionNode() {}

// TokenLiteral returns a token literal of array.
func (ie *IndexExpression) TokenLiteral() string {
	if ie == nil {
		return ""
	}
	return ie.Token.Literal
}

func (ie *IndexExpression) String() string {
	if ie == nil {
		return ""
	}

	var out bytes.Buffer

	out.WriteString("(")
	out.WriteString(ie.Left.String())
	out.WriteString("[")
	out.WriteString(ie.Index.String())
	out.WriteString("])")

	return out.String()
}

// SliceExpression represents a slice operator: Left[StartIndex..EndIndex]
type SliceExpression struct {
	Token      token.Token // The '[' token
	Left       Expression
	StartIndex Expression // Can be nil
	EndIndex   Expression // Can be nil
}

func (*SliceExpression) expressionNode() {}
func (se *SliceExpression) TokenLiteral() string { return se.Token.Literal }
func (se *SliceExpression) String() string {
	var out bytes.Buffer
	out.WriteString("(")
	out.WriteString(se.Left.String())
	out.WriteString("[")
	if se.StartIndex != nil {
		out.WriteString(se.StartIndex.String())
	}
	out.WriteString("..")
	if se.EndIndex != nil {
		out.WriteString(se.EndIndex.String())
	}
	out.WriteString("])")
	return out.String()
}

// RangeExpression represents a range, e.g. 1..10
type RangeExpression struct {
	Token token.Token // The '..' token
	Start Expression
	End   Expression
}

func (re *RangeExpression) expressionNode()      {}
func (re *RangeExpression) TokenLiteral() string { return re.Token.Literal }
func (re *RangeExpression) String() string {
	return re.Start.String() + ".." + re.End.String()
}

////////////////////////////////////////////////
// mudscript
// 變數宣告 (例如: int x = 1; 或 int x;)
type TypedVarDecl struct {
	Token   token.Token // 記錄型別的 Token (例如 INT_TYPE)
	IsArray bool        // 是否為陣列 (例如 string *paths)
	Name    *Ident      // 變數名稱 (使用 *Ident)
	Value   Expression  // 初始值 (如果有的話)
}

func (tvd *TypedVarDecl) statementNode()       {}
func (tvd *TypedVarDecl) expressionNode()      {}
func (tvd *TypedVarDecl) TokenLiteral() string { return tvd.Token.Literal }
func (tvd *TypedVarDecl) String() string {
	var out bytes.Buffer

	out.WriteString(tvd.TokenLiteral() + " ")
	if tvd.IsArray {
		out.WriteString("*")
	}
	out.WriteString(tvd.Name.String())

	if tvd.Value != nil {
		out.WriteString(" = ")
		out.WriteString(tvd.Value.String())
	}
	out.WriteString(";")
	return out.String()
}

// 函式定義 (例如: int main(string arg) { ... })
type FunctionDef struct {
	Token      token.Token    // 記錄回傳型別的 Token
	IsArray    bool           // 回傳值是否為陣列
	IsVarargs  bool           // 是否宣告為 varargs
	Name       *Ident         // 函式名稱 (使用 *Ident)
	Params     []*TypedParam  // 帶有型別的參數清單
	Body       *BlockStatement
}

func (fd *FunctionDef) statementNode()       {}
func (fd *FunctionDef) TokenLiteral() string { return fd.Token.Literal }
func (fd *FunctionDef) String() string {
	var out bytes.Buffer

	out.WriteString(fd.TokenLiteral() + " ")
	if fd.IsArray {
		out.WriteString("*")
	}
	out.WriteString(fd.Name.String())
	out.WriteString("(")

	var params []string
	for _, p := range fd.Params {
		params = append(params, p.String())
	}
	out.WriteString(strings.Join(params, ", "))
	
	out.WriteString(") { ") // 加上左大括號

	if fd.Body != nil {
		out.WriteString(fd.Body.String())
	}

	out.WriteString(" } ") // 加上右大括號
	
	return out.String()
}

// 帶型別的參數 (例如: string arg)
type TypedParam struct {
	TypeToken token.Token
	IsArray   bool
	Name      *Ident      // 參數名稱 (使用 *Ident)
}

func (tp *TypedParam) TokenLiteral() string { return tp.TypeToken.Literal }
func (tp *TypedParam) String() string {
	res := tp.TypeToken.Literal + " "
	if tp.IsArray {
		res += "*"
	}
	res += tp.Name.String()
	return res
}

// InheritStatement 不變
type InheritStatement struct {
	Token token.Token 
	Path  string      
}

func (is *InheritStatement) statementNode()       {}
func (is *InheritStatement) TokenLiteral() string { return is.Token.Literal }
func (is *InheritStatement) String() string {
	var out bytes.Buffer
	out.WriteString(is.TokenLiteral() + " ")
	out.WriteString(`"` + is.Path + `"`)
	out.WriteString(";")
	return out.String()
}

// ==========================================
// 表達式 (Expressions): 賦值與自增減
// ==========================================

// AssignExpression (處理 x = 1, x += 2 等)
type AssignExpression struct {
	Token    token.Token // =, +=, -= 等
	Left     Expression  // 通常是 Ident 或 IndexExpression
	Operator string
	Value    Expression
}
func (ae *AssignExpression) expressionNode()      {}
func (ae *AssignExpression) TokenLiteral() string { return ae.Token.Literal }
func (ae *AssignExpression) String() string {
	return "(" + ae.Left.String() + " " + ae.Operator + " " + ae.Value.String() + ")"
}

// PostfixExpression (處理 x++, x--)
type PostfixExpression struct {
	Token    token.Token // ++, --
	Left     Expression
	Operator string
}
func (pe *PostfixExpression) expressionNode()      {}
func (pe *PostfixExpression) TokenLiteral() string { return pe.Token.Literal }
func (pe *PostfixExpression) String() string {
	return "(" + pe.Left.String() + pe.Operator + ")"
}

// ==========================================
// 陳述式 (Statements): 迴圈與分支
// ==========================================

// ForStatement
type ForStatement struct {
	Token     token.Token // 'for' token
	Init      Statement
	Condition Expression
	Post      Expression
	Body      *BlockStatement
}
func (fs *ForStatement) statementNode()       {}
func (fs *ForStatement) TokenLiteral() string { return fs.Token.Literal }
func (fs *ForStatement) String() string {
	var out bytes.Buffer
	out.WriteString("for (")
	if fs.Init != nil { out.WriteString(fs.Init.String()) }
	out.WriteString(" ")
	if fs.Condition != nil { out.WriteString(fs.Condition.String()) }
	out.WriteString("; ")
	if fs.Post != nil { out.WriteString(fs.Post.String()) }
	out.WriteString(") ")
	out.WriteString(fs.Body.String())
	return out.String()
}

// WhileStatement
type WhileStatement struct {
	Token     token.Token // 'while'
	Condition Expression
	Body      *BlockStatement
}
func (ws *WhileStatement) statementNode()       {}
func (ws *WhileStatement) TokenLiteral() string { return ws.Token.Literal }
func (ws *WhileStatement) String() string {
	return "while (" + ws.Condition.String() + ") " + ws.Body.String()
}

// DoWhileStatement
type DoWhileStatement struct {
	Token     token.Token // 'do'
	Body      *BlockStatement
	Condition Expression
}
func (dws *DoWhileStatement) statementNode()       {}
func (dws *DoWhileStatement) TokenLiteral() string { return dws.Token.Literal }
func (dws *DoWhileStatement) String() string {
	return "do " + dws.Body.String() + " while (" + dws.Condition.String() + ");"
}

// BreakStatement
type BreakStatement struct { Token token.Token }
func (bs *BreakStatement) statementNode()       {}
func (bs *BreakStatement) TokenLiteral() string { return bs.Token.Literal }
func (bs *BreakStatement) String() string       { return "break;" }

// ContinueStatement
type ContinueStatement struct { Token token.Token }
func (cs *ContinueStatement) statementNode()       {}
func (cs *ContinueStatement) TokenLiteral() string { return cs.Token.Literal }
func (cs *ContinueStatement) String() string       { return "continue;" }

// CaseStatement
type CaseStatement struct {
	Token token.Token // 'case' or 'default'
	Value Expression  // nil if default
	Body  []Statement
}
func (cs *CaseStatement) statementNode()       {}
func (cs *CaseStatement) TokenLiteral() string { return cs.Token.Literal }
func (cs *CaseStatement) String() string {
	var out bytes.Buffer
	if cs.Value == nil {
		out.WriteString("default:\n")
	} else {
		out.WriteString("case " + cs.Value.String() + ":\n")
	}
	for _, s := range cs.Body {
		out.WriteString(s.String() + "\n")
	}
	return out.String()
}

// SwitchStatement
type SwitchStatement struct {
	Token token.Token // 'switch'
	Value Expression
	Cases []*CaseStatement
}
func (ss *SwitchStatement) statementNode()       {}
func (ss *SwitchStatement) TokenLiteral() string { return ss.Token.Literal }
func (ss *SwitchStatement) String() string {
	var out bytes.Buffer
	out.WriteString("switch (" + ss.Value.String() + ") {\n")
	for _, c := range ss.Cases {
		out.WriteString(c.String())
	}
	out.WriteString("}")
	return out.String()
}

// MappingLiteral 處理 LPC 的 mapping: ([ "key": val, 1: 2 ])
type MappingLiteral struct {
	Token token.Token // 儲存 token.LBRACKET_MAP ('([')
	Pairs map[Expression]Expression
}

func (ml *MappingLiteral) expressionNode()      {}
func (ml *MappingLiteral) TokenLiteral() string { return ml.Token.Literal }
func (ml *MappingLiteral) String() string {
	var out bytes.Buffer

	var pairs []string
	for key, value := range ml.Pairs {
		pairs = append(pairs, key.String()+": "+value.String())
	}

	out.WriteString("([")
	out.WriteString(strings.Join(pairs, ", "))
	out.WriteString("])")

	return out.String()
}

// CallOtherExpression 處理 ob->func(args)
type CallOtherExpression struct {
	Token     token.Token // 儲存 '->' (token.ARROW)
	Object    Expression  // 左側的目標物件 (例如 ob)
	Method    *Ident      // 右側的函式名稱 (例如 func)
	Arguments []Expression // 傳入的參數
}

func (ce *CallOtherExpression) expressionNode()      {}
func (ce *CallOtherExpression) TokenLiteral() string { return ce.Token.Literal }
func (ce *CallOtherExpression) String() string {
	var out bytes.Buffer

	args := []string{}
	for _, a := range ce.Arguments {
		args = append(args, a.String())
	}

	out.WriteString("(")
	out.WriteString(ce.Object.String())
	out.WriteString("->")
	out.WriteString(ce.Method.String())
	out.WriteString("(")
	out.WriteString(strings.Join(args, ", "))
	out.WriteString("))")

	return out.String()
}

type ClosureLiteral struct {
	Token    token.Token // The '(: ' token
	Elements []Expression
}
func (cl *ClosureLiteral) expressionNode()      {}
func (cl *ClosureLiteral) TokenLiteral() string { return cl.Token.Literal }
func (cl *ClosureLiteral) String() string { return "(: closure :)" }

type ForEachStatement struct {
	Token      token.Token // 'foreach' 標記
	Key        *Ident      // 對於 mapping 來說是 key；對於 array 來說可以不寫
	Value      *Ident      // 對於 mapping 來說是 value；對於 array 來說是 item
	Collection Expression  // 被巡覽的陣列或 Mapping 表達式
	Body       *BlockStatement
}

func (fes *ForEachStatement) statementNode()       {}
func (fes *ForEachStatement) TokenLiteral() string { return fes.Token.Literal }
func (fes *ForEachStatement) String() string {
	var out bytes.Buffer
	out.WriteString("foreach (")
	if fes.Key != nil {
		out.WriteString(fes.Key.String() + ", ")
	}
	out.WriteString(fes.Value.String() + " in " + fes.Collection.String() + ") ")
	out.WriteString(fes.Body.String())
	return out.String()
}
