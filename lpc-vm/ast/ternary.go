
package ast

import (
	"bytes"
	"mudscript/lpc-vm/token"
)

// TernaryExpression represents a ternary operator (condition ? trueVal : falseVal)
type TernaryExpression struct {
	Token       token.Token // the '?' token
	Condition   Expression
	TrueResult  Expression
	FalseResult Expression
}

func (te *TernaryExpression) expressionNode() {}
func (te *TernaryExpression) TokenLiteral() string { return te.Token.Literal }
func (te *TernaryExpression) String() string {
	var out bytes.Buffer
	out.WriteString("(")
	out.WriteString(te.Condition.String())
	out.WriteString(" ? ")
	out.WriteString(te.TrueResult.String())
	out.WriteString(" : ")
	out.WriteString(te.FalseResult.String())
	out.WriteString(")")
	return out.String()
}
