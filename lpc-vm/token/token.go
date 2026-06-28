package token

// TokenType is a token type.
type TokenType string

type Token struct {
	TokenType TokenType
	Literal   string
	Line      int
}

const (
	// ILLEGAL is a token type for illegal tokens.
	ILLEGAL TokenType = "ILLEGAL"
	// EOF is a token type that represents end of file.
	EOF = "EOF"

	// IDENT is a token type for identifiers.
	IDENT = "IDENT" // add, foobar, x, y, ...
	// INT is a token type for integers.
	INT = "INT"
	// FLOAT is a token type for floating point numbers.
	FLOAT = "FLOAT"
	// STRING is a token type for strings.
	STRING = "STRING"

	// BANG is a token type for NOT operator.
	BANG = "!"
	// ASSIGN is a token type for assignment operators.
	ASSIGN = "="
	// PLUS is a token type for addition.
	PLUS = "+"
	// MINUS is a token type for subtraction.
	MINUS = "-"
	// ASTARISK is a token type for multiplication.
	ASTARISK = "*"
	// SLASH is a token type for division.
	SLASH = "/"
	MOD = "%"
	// LT is a token ype for 'less than' operator.
	LT = "<"
	// GT is a token ype for 'greater than' operator.
	GT = ">"
	// EQ is a token type for equality operator.
	EQ = "=="
	// NEQ is a token type for not equality operator.
	NEQ = "!="
	GTE = ">="
	LTE = "<="

	// COMMA is a token type for commas.
	COMMA = ","
	// QUESTION is a token type for question marks.
	QUESTION = "?"
	// SEMICOLON is a token type for semicolons.
	SEMICOLON = ";"
	// COLON is a token type for colons.
	COLON = ":"

	// LPAREN is a token type for left parentheses.
	LPAREN = "("
	// RPAREN is a token type for right parentheses.
	RPAREN = ")"

	// 閉包 closure
	LPAREN_COLON = "(: "
	LARRAY = "({" 
	RARRAY = "RARRAY" // 保留名稱但不再由 Lexer 直接產生，或直接移除

	// LBRACE is a token type for left braces.
	LBRACE = "{"
	// RBRACE is a token type for right braces.
	RBRACE = "}"
	// LBRACKET is a token type for left brackets.
	LBRACKET     = "["
	RBRACKET     = "]"

	// Slice operator
	DOTDOT       = ".."

	// TRUE is a token type for true.
	TRUE = "TRUE"
	// FALSE is a token type for false.
	FALSE = "FALSE"
	// IF is a token type for if.
	IF = "IF"
	// ELSE is a token type for else.
	ELSE = "ELSE"
	// RETURN is a token type for return.
	RETURN = "RETURN"

	/////////////////////////////////////////////////
	// Mudscript
	INT_TYPE     = "INT_TYPE"     // int
	STRING_TYPE  = "STRING_TYPE"  // string
	FLOAT_TYPE   = "FLOAT_TYPE"   // float
	OBJECT_TYPE  = "OBJECT_TYPE"  // object
	MAPPING_TYPE = "MAPPING_TYPE" // mapping
	MIXED_TYPE   = "MIXED_TYPE"   // mixed
	VOID_TYPE    = "VOID_TYPE"    // void
	CLOSURE_TYPE = "CLOSURE_TYPE" // closure
	FUNCTION_TYPE = "FUNCTION_TYPE" // function
	BUFFER_TYPE  = "BUFFER_TYPE"  // buffer

	// LPC 新增的關鍵字
	INHERIT      = "INHERIT"
	NEW          = "NEW"
	CATCH        = "CATCH"

	// LPC 專屬符號
	ARROW        = "->" // call_other
	SCOPE        = "::" // 繼承呼叫
	LBRACKET_MAP = "([" // mapping 起始
	PLUS_EQUALS     = "+="
	MINUS_EQUALS    = "-="
	ASTERISK_EQUALS = "*="
	SLASH_EQUALS    = "/="
	MOD_EQUALS      = "%="
	AND_EQUALS      = "&="
	OR_EQUALS       = "|="
	XOR_EQUALS      = "^="
	LSHIFT_EQUALS   = "<<="
	RSHIFT_EQUALS   = ">>="
	INC             = "++"
	DEC             = "--"

	// 邏輯運算子
	AND             = "&&"
	OR              = "||"

	// 位元運算子
	BIT_AND         = "&"
	BIT_OR          = "|"
	BIT_XOR         = "^"
	BIT_NOT         = "~"
	LSHIFT          = "<<"
	RSHIFT          = ">>"

	CHAR         = "CHAR"         // 'c'
	PREPROCESSOR = "PREPROCESSOR" // #include, #define

	FOR      = "FOR"
	WHILE    = "WHILE"
	DO       = "DO"
	SWITCH   = "SWITCH"
	CASE     = "CASE"
	DEFAULT  = "DEFAULT"
	BREAK    = "BREAK"
	CONTINUE = "CONTINUE"

	FOREACH = "FOREACH"
    IN      = "IN"

	PRIVATE   TokenType = "PRIVATE"
 	STATIC    TokenType = "STATIC"
 	PROTECTED TokenType = "PROTECTED"
 	VARARGS   TokenType = "VARARGS"
 	NOSAVE    TokenType = "NOSAVE"
 	NOMASK    TokenType = "NOMASK"
)

// Language keywords
var keywords = map[string]TokenType{
	"true":   TRUE,
	"false":  FALSE,
	"if":     IF,
	"else":   ELSE,
	"return": RETURN,

	/////////////////////////////////////////////////
	// Mudscript 加入 LPC 的關鍵字
	"int":     INT_TYPE,
	"string":  STRING_TYPE,
	"float":   FLOAT_TYPE,
	"object":  OBJECT_TYPE,
	"mapping": MAPPING_TYPE,
	"mixed":   MIXED_TYPE,
	"void":    VOID_TYPE,
	"closure": CLOSURE_TYPE,
	"function": FUNCTION_TYPE,
	"buffer":  BUFFER_TYPE,
	"inherit": INHERIT,
	"new":     NEW,
	"catch":   CATCH,

	"for":      FOR,
	"while":    WHILE,
	"do":       DO,
	"switch":   SWITCH,
	"case":     CASE,
	"default":  DEFAULT,
	"break":    BREAK,
	"continue": CONTINUE,

	"foreach": FOREACH,
    "in":      IN,
 	// Modifiers
 	"private":   PRIVATE,
 	"static":    STATIC,
 	"protected": PROTECTED,
 	"varargs":   VARARGS,
 	"nosave":    NOSAVE,
 	"nomask":    NOMASK,
}

// LookupIdent checks the language keywords to see whether the given identifier is a keyword.
// If it is, it returns the keyword's TokenType constant. If it isn't, it just gets back IDENT.
func LookupIdent(ident string) TokenType {
	if tok, ok := keywords[ident]; ok {
		return tok
	}
	return IDENT
}
