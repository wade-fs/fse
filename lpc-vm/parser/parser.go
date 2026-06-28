package parser

import (
	"fmt"
	"strconv"
	"strings" // [新增] 用來處理字串切分

	"mudscript/lpc-vm/ast"
	"mudscript/lpc-vm/lexer"
	"mudscript/lpc-vm/token"
)

const (
	_ int = iota
	LOWEST
	COMMA       // ,
	ASSIGN      // = += -= *= /=
	TERNARY     // ? :
	LOGICAL_OR	// ||
	LOGICAL_AND	// &&
	BIT_OR      // |
	BIT_XOR     // ^
	BIT_AND     // &
	EQUALS      // == !=
	LESSGREATER // > < >= <=
	RANGE       // ..
	SHIFT       // << >>
	SUM         // + -
	PRODUCT     // * / %
	PREFIX      // -X or !X or ~X or ++X or --X
	CALL        // myFunction(X)
	INDEX       // array[index]
	SCOPE_PREC  // 作用域解析的優先權極高
	POSTFIX     // X++ X--
)

var precedences = map[token.TokenType]int{
	token.OR:       LOGICAL_OR,
	token.AND:      LOGICAL_AND,
	token.BIT_OR:   BIT_OR,
	token.BIT_XOR:  BIT_XOR,
	token.BIT_AND:  BIT_AND,
	token.EQ:       EQUALS,
	token.NEQ:      EQUALS,
	token.LT:       LESSGREATER,
	token.GT:       LESSGREATER,
	token.GTE:      LESSGREATER,
	token.LTE:      LESSGREATER,
	token.DOTDOT:   RANGE,
	token.LSHIFT:   SHIFT,
	token.RSHIFT:   SHIFT,
	token.PLUS:     SUM,
	token.MINUS:    SUM,
	token.SLASH:    PRODUCT,
	token.ASTARISK: PRODUCT,
	token.MOD:      PRODUCT,
	token.LPAREN:   CALL,
	token.LBRACKET: INDEX,
	token.ARROW:    CALL,
	token.ASSIGN:          ASSIGN,
	token.PLUS_EQUALS:     ASSIGN,
	token.MINUS_EQUALS:    ASSIGN,
	token.ASTERISK_EQUALS: ASSIGN,
	token.SLASH_EQUALS:    ASSIGN,
	token.MOD_EQUALS:      ASSIGN,
	token.AND_EQUALS:      ASSIGN,
	token.OR_EQUALS:       ASSIGN,
	token.XOR_EQUALS:      ASSIGN,
	token.LSHIFT_EQUALS:   ASSIGN,
	token.RSHIFT_EQUALS:   ASSIGN,
	token.QUESTION:        TERNARY,
	token.COLON:           LOWEST, // 🚀 新增：冒號優先權最低
	token.INC:             POSTFIX,
	token.DEC:             POSTFIX,
	token.SCOPE:           SCOPE_PREC,
	token.COMMA:           COMMA,
}
type (
	prefixParseFn func() ast.Expression
	infixParseFn  func(ast.Expression) ast.Expression
)

// Parser is a parser of Monkey programming language.
type Parser struct {
	l      lexer.Lexer
	errors []string

	curToken      token.Token
	peekToken     token.Token
	peekPeekToken token.Token

	prefixParseFns map[token.TokenType]prefixParseFn
	infixParseFns  map[token.TokenType]infixParseFn
}

// New returns a new Parser.
func New(l lexer.Lexer) *Parser {
	p := &Parser{
		l:      l,
		errors: []string{},
	}

	p.prefixParseFns = map[token.TokenType]prefixParseFn{
		token.IDENT:    p.parseIdent,
		token.INT:      p.parseIntegerLiteral,
		token.FLOAT:    p.parseFloatLiteral,
		token.BANG:     p.parsePrefixExpression,
		token.MINUS:    p.parsePrefixExpression,
		token.BIT_NOT:  p.parsePrefixExpression,
		token.INC:      p.parsePrefixExpression, // [新增] ++X
		token.DEC:      p.parsePrefixExpression, // [新增] --X
		token.TRUE:     p.parseBoolean,
		token.FALSE:    p.parseBoolean,
		token.LPAREN:   p.parseGroupedExpression,
		token.IF:       p.parseIfExpression,
		token.STRING:   p.parseStringLiteral,
		token.CHAR:     p.parseCharLiteral, // [修正] 將 CHAR 正確解析為整數
		token.LARRAY:   p.parseLPCArrayLiteral,
		token.LT:       p.parsePrefixExpression, // [新增] 支援 <n 語法 (從末尾索引)
		token.NEW:      p.parsePrefixExpression, // [新增] 支援 new(path)
		token.CATCH:    p.parsePrefixExpression, // [新增] 支援 catch(expr)

		token.LBRACKET_MAP: p.parseMappingLiteral,
		token.SCOPE:        p.parsePrefixScope,
		token.LPAREN_COLON: p.parseClosureLiteral,

		// 支援型別開頭的表達式 (用於 Closure 內的變數宣告或型別標記)
		token.INT_TYPE:     p.parseTypePrefix,
		token.STRING_TYPE:  p.parseTypePrefix,
		token.FLOAT_TYPE:   p.parseTypePrefix,
		token.OBJECT_TYPE:  p.parseTypePrefix,
		token.MAPPING_TYPE: p.parseTypePrefix,
		token.MIXED_TYPE:   p.parseTypePrefix,
		token.CLOSURE_TYPE: p.parseTypePrefix,
		token.BUFFER_TYPE:  p.parseTypePrefix,
	}

	p.infixParseFns = map[token.TokenType]infixParseFn{
		token.PLUS:     p.parseInfixExpression,
		token.MINUS:    p.parseInfixExpression,
		token.ASTARISK: p.parseInfixExpression,
		token.SLASH:    p.parseInfixExpression,
		token.MOD:		p.parseInfixExpression,
		token.EQ:       p.parseInfixExpression,
		token.NEQ:      p.parseInfixExpression,
		token.LT:       p.parseInfixExpression,
		token.GT:       p.parseInfixExpression,
		token.LTE:      p.parseInfixExpression,
		token.GTE:      p.parseInfixExpression,
		token.DOTDOT:   p.parseRangeExpression, // [新增] X..Y
		token.LPAREN:   p.parseCallExpression,
		token.LBRACKET: p.parseIndexExpression,
		token.ASSIGN:   p.parseAssignExpression,
		token.PLUS_EQUALS:     p.parseAssignExpression,
		token.MINUS_EQUALS:    p.parseAssignExpression,
		token.ASTERISK_EQUALS: p.parseAssignExpression,
		token.SLASH_EQUALS:    p.parseAssignExpression,
		token.MOD_EQUALS:      p.parseAssignExpression,
		token.AND_EQUALS:      p.parseAssignExpression,
		token.OR_EQUALS:       p.parseAssignExpression,
		token.XOR_EQUALS:      p.parseAssignExpression,
		token.LSHIFT_EQUALS:   p.parseAssignExpression,
		token.RSHIFT_EQUALS:   p.parseAssignExpression,
		token.QUESTION:        p.parseTernaryExpression,
		token.INC:             p.parsePostfixExpression,
		token.DEC:             p.parsePostfixExpression,

		token.SCOPE:    p.parseInfixScope,
		token.ARROW:    p.parseCallOtherExpression,
		token.AND: p.parseInfixExpression,
 		token.OR:  p.parseInfixExpression,
		token.BIT_AND: p.parseInfixExpression,
		token.BIT_OR:  p.parseInfixExpression,
		token.BIT_XOR: p.parseInfixExpression,
		token.LSHIFT:  p.parseInfixExpression,
		token.RSHIFT:  p.parseInfixExpression,
		token.COMMA:   p.parseInfixExpression,
	}

	p.nextToken()
	p.nextToken()
	p.nextToken()

	return p
}

func (p *Parser) nextToken() {
	p.curToken = p.peekToken
	p.peekToken = p.peekPeekToken
	p.peekPeekToken = p.l.NextToken()
}

func (p *Parser) Errors() []string {
	return p.errors
}

// ==========================================
// [新增] 強大的錯誤產生器
// ==========================================
func (p *Parser) addError(tok token.Token, format string, args ...interface{}) {
	baseMsg := fmt.Sprintf(format, args...)
	
	// 從 lexer 取得完整的原始程式碼，切分成行
	lines := strings.Split(p.l.GetInput(), "\n")
	lineContent := ""
	if tok.Line > 0 && tok.Line <= len(lines) {
		lineContent = lines[tok.Line-1] // 陣列索引是從 0 開始
	}

	// 去除多餘空白，讓終端機顯示更好看
	lineContent = strings.TrimSpace(lineContent)

	// 組裝帶有行號與程式碼內容的錯誤訊息
	finalMsg := fmt.Sprintf("[Line %d] %s\n    > %s", tok.Line, baseMsg, lineContent)
	p.errors = append(p.errors, finalMsg)
}

func (p *Parser) peekError(typ token.TokenType) {
	p.addError(p.peekToken, "預期下一個符號為 '%s'，但得到了 '%s' (%s)", typ, p.peekToken.TokenType, p.peekToken.Literal)
}

func (p *Parser) curTokenIs(typ token.TokenType) bool {
	return p.curToken.TokenType == typ
}

func (p *Parser) peekTokenIs(typ token.TokenType) bool {
	return p.peekToken.TokenType == typ
}

func (p *Parser) expectPeek(typ token.TokenType) bool {
	if p.peekTokenIs(typ) {
		p.nextToken()
		return true
	}

	p.peekError(typ)
	return false
}

func (p *Parser) ParseProgram() *ast.Program {
	program := &ast.Program{
		Statements: []ast.Statement{},
	}

	for !p.curTokenIs(token.EOF) {
		stmt := p.parseStatement(true)
		if stmt != nil {
			program.Statements = append(program.Statements, stmt)
		}
		p.nextToken()
	}

	return program
}

func (p *Parser) parseReturnStatement() *ast.ReturnStatement {
	stmt := &ast.ReturnStatement{
		Token: p.curToken,
	}

	p.nextToken()
	if p.curTokenIs(token.SEMICOLON) {
		return stmt
	}

	stmt.ReturnValue = p.ParseExpression(LOWEST)

	for p.peekTokenIs(token.SEMICOLON) {
		p.nextToken()
	}

	return stmt
}

func (p *Parser) ParseExpressionStatement() *ast.ExpressionStatement {
	stmt := &ast.ExpressionStatement{
		Token:      p.curToken,
		Expression: p.ParseExpression(LOWEST),
	}

	for p.peekTokenIs(token.SEMICOLON) {
		p.nextToken()
	}
	return stmt
}

func (p *Parser) ParseExpression(precedence int) ast.Expression {
	prefix := p.prefixParseFns[p.curToken.TokenType]
	if prefix == nil {
		p.addError(p.curToken, "無法解析開頭為 '%s' 的語法 (找不到對應的前綴處理器)", p.curToken.TokenType)
		return nil
	}

	leftExp := prefix()

	for !p.curTokenIs(token.SEMICOLON) && precedence < p.peekPrecedence() {
		infix := p.infixParseFns[p.peekToken.TokenType]
		if infix == nil {
			return leftExp
		}

		p.nextToken()

		leftExp = infix(leftExp)
	}

	return leftExp
}

func (p *Parser) parseIdent() ast.Expression {
	return &ast.Ident{
		Token: p.curToken,
		Value: p.curToken.Literal,
	}
}

func (p *Parser) parseIntegerLiteral() ast.Expression {
	lit := &ast.IntegerLiteral{Token: p.curToken}

	val, err := strconv.ParseInt(p.curToken.Literal, 0, 64)
	if err != nil {
		p.addError(p.curToken, "無法將 '%s' 解析為整數", p.curToken.Literal)
		return nil
	}

	lit.Value = val
	return lit
}

func (p *Parser) parseFloatLiteral() ast.Expression {
	val, err := strconv.ParseFloat(p.curToken.Literal, 64)
	if err != nil {
		p.addError(p.curToken, "無法將 '%s' 解析為浮點數", p.curToken.Literal)
		return nil
	}

	return &ast.FloatLiteral{
		Token: p.curToken,
		Value: val,
	}
}

func (p *Parser) parsePrefixExpression() ast.Expression {
	expr := &ast.PrefixExpression{
		Token:    p.curToken,
		Operator: p.curToken.Literal,
	}

	p.nextToken()

	expr.Right = p.ParseExpression(PREFIX)
	return expr
}

func (p *Parser) peekPrecedence() int {
	if p, ok := precedences[p.peekToken.TokenType]; ok {
		return p
	}
	return LOWEST
}

func (p *Parser) curPrecedence() int {
	if p, ok := precedences[p.curToken.TokenType]; ok {
		return p
	}
	return LOWEST
}

func (p *Parser) parseInfixExpression(left ast.Expression) ast.Expression {
	expr := &ast.InfixExpression{
		Token:    p.curToken,
		Operator: p.curToken.Literal,
		Left:     left,
	}

	prec := p.curPrecedence()

	p.nextToken()

	expr.Right = p.ParseExpression(prec)
	return expr
}

func (p *Parser) parseBoolean() ast.Expression {
	return &ast.Boolean{
		Token: p.curToken,
		Value: p.curTokenIs(token.TRUE),
	}
}

func (p *Parser) parseGroupedExpression() ast.Expression {
	p.nextToken()

	// 🚀 新增：支援 (type)expr 強制轉型語法 (例如 (string)ob->query("name"))
	if p.isTypeToken(p.curToken.TokenType) {
		// 檢查下一個是否為 ')'
		if p.peekTokenIs(token.RPAREN) {
			p.nextToken() // Skip type
			p.nextToken() // Skip ')'
			// 這裡目前將轉型視為 No-op，直接解析後續表達式
			return p.ParseExpression(PREFIX)
		}
	}

	expr := p.ParseExpression(LOWEST)

	if !p.expectPeek(token.RPAREN) {
		return nil
	}

	return expr
}

func (p *Parser) parseIfExpression() ast.Expression {
	expression := &ast.IfExpression{Token: p.curToken}

	if !p.expectPeek(token.LPAREN) { return nil }
	p.nextToken()
	expression.Condition = p.ParseExpression(LOWEST)
	if !p.expectPeek(token.RPAREN) { return nil }

	// ▼ [關鍵修正]：Consequence (If 成立的分支)
	if p.peekTokenIs(token.LBRACE) {
		p.nextToken()
		expression.Consequence = p.parseBlockStatement()
	} else {
		// 處理單行 if (例如: if(x) return 1;)
		p.nextToken()
		stmt := p.parseStatement(false)
		expression.Consequence = &ast.BlockStatement{
			Token:      p.curToken,
			Statements: []ast.Statement{stmt},
		}
	}

	if p.peekTokenIs(token.ELSE) {
		p.nextToken() 

		if p.peekTokenIs(token.IF) {
			p.nextToken() 
			elseIfNode := p.parseIfExpression()
			expression.Alternative = &ast.BlockStatement{
				Token: p.curToken,
				Statements: []ast.Statement{
					&ast.ExpressionStatement{
						Token:      p.curToken,
						Expression: elseIfNode,
					},
				},
			}
		} else if p.peekTokenIs(token.LBRACE) {
			p.nextToken()
			expression.Alternative = p.parseBlockStatement()
		} else {
			// ▼ [關鍵修正]：處理單行 else (例如: else return 0;)
			p.nextToken()
			stmt := p.parseStatement(false)
			expression.Alternative = &ast.BlockStatement{
				Token:      p.curToken,
				Statements: []ast.Statement{stmt},
			}
		}
	}

	return expression
}

func (p *Parser) parseBlockStatement() *ast.BlockStatement {
	block := &ast.BlockStatement{Token: p.curToken}
	block.Statements = []ast.Statement{}

	p.nextToken()

	for !p.curTokenIs(token.RBRACE) && !p.curTokenIs(token.EOF) {
		stmt := p.parseStatement(false)
		if stmt != nil {
			block.Statements = append(block.Statements, stmt)
		}
		p.nextToken()
	}

	return block
}

func (p *Parser) parseFunctionParameters() []*ast.Ident {
	idents := []*ast.Ident{}

	if p.peekTokenIs(token.RPAREN) {
		p.nextToken()
		return idents
	}

	p.nextToken()

	ident := &ast.Ident{
		Token: p.curToken,
		Value: p.curToken.Literal,
	}
	idents = append(idents, ident)

	for p.peekTokenIs(token.COMMA) {
		p.nextToken()
		p.nextToken()
		ident := &ast.Ident{
			Token: p.curToken,
			Value: p.curToken.Literal,
		}
		idents = append(idents, ident)
	}

	if !p.expectPeek(token.RPAREN) {
		return nil
	}

	return idents
}

func (p *Parser) ParseExpressionList(end token.TokenType) []ast.Expression {
	list := make([]ast.Expression, 0)

	if p.peekTokenIs(end) {
		p.nextToken()
		return list
	}

	p.nextToken()
	list = append(list, p.ParseExpression(COMMA))

	for p.peekTokenIs(token.COMMA) {
		p.nextToken()
		if p.peekTokenIs(end) {
			p.nextToken() // Skip the trailing comma and move to end token
			return list
		}
		p.nextToken()
		list = append(list, p.ParseExpression(COMMA))
	}
	if !p.expectPeek(end) {
		return nil
	}

	return list
}

func (p *Parser) parseCallExpression(function ast.Expression) ast.Expression {
	return &ast.CallExpression{
		Token:     p.curToken,
		Function:  function,
		Arguments: p.ParseExpressionList(token.RPAREN),
	}
}

func (p *Parser) parseStringLiteral() ast.Expression {
	lit := &ast.StringLiteral{Token: p.curToken, Value: p.curToken.Literal}

	// 🚀 關鍵相容：支援相鄰字串自動串接 (ANSI C 風格)
	// 在 C/LPC 中，"a" "b" 會自動合併為 "ab"
	for p.peekTokenIs(token.STRING) || p.peekTokenIs(token.CHAR) {
		p.nextToken()
		lit.Value += p.curToken.Literal
	}

	return lit
}

func (p *Parser) parseCharLiteral() ast.Expression {
	// 如果後面緊接著字串或字元，則轉向字串解析 (相鄰串接)
	if p.peekTokenIs(token.STRING) || p.peekTokenIs(token.CHAR) {
		return p.parseStringLiteral()
	}

	lit := &ast.IntegerLiteral{Token: p.curToken}
	if len(p.curToken.Literal) > 0 {
		lit.Value = int64(p.curToken.Literal[0])
	}
	return lit
}

func (p *Parser) parseIndexExpression(left ast.Expression) ast.Expression {
	tok := p.curToken // '['

	p.nextToken() // Skip '['

	// 1. [..end] or [..]
	if p.curTokenIs(token.DOTDOT) {
		slice := &ast.SliceExpression{Token: tok, Left: left}
		p.nextToken() // Skip '..'
		if !p.curTokenIs(token.RBRACKET) {
			slice.EndIndex = p.ParseExpression(LOWEST)
			if !p.expectPeek(token.RBRACKET) { return nil }
		}
		return slice
	}

	// 2. [start..] or [start..end] or [index]
	idx := p.ParseExpression(RANGE) // 🚀 使用 RANGE 優先權，遇到 .. 會停止而不吞掉它

	if p.peekTokenIs(token.DOTDOT) {
		slice := &ast.SliceExpression{Token: tok, Left: left, StartIndex: idx}
		p.nextToken() // Move to '..'
		p.nextToken() // Skip '..'

		if !p.curTokenIs(token.RBRACKET) {
			slice.EndIndex = p.ParseExpression(LOWEST)
			if !p.expectPeek(token.RBRACKET) { return nil }
		}
		return slice
	}
	// 一般索引 [index]
	expr := &ast.IndexExpression{
		Token: tok,
		Left:  left,
		Index: idx,
	}

	if !p.expectPeek(token.RBRACKET) {
		return nil
	}

	return expr
}

func (p *Parser) isTypeToken(t token.TokenType) bool {
	switch t {
	case token.INT_TYPE, token.STRING_TYPE, token.OBJECT_TYPE, 
	     token.MAPPING_TYPE, token.FLOAT_TYPE, token.MIXED_TYPE, 
		 token.VOID_TYPE, token.CLOSURE_TYPE, token.BUFFER_TYPE,
		 token.FUNCTION_TYPE:
		return true
	default:
		return false
	}
}

func (p *Parser) parseStatement(topLevel bool) ast.Statement {
	isVarargs := false
	for p.curTokenIs(token.PRIVATE) || p.curTokenIs(token.STATIC) ||
		p.curTokenIs(token.PROTECTED) || p.curTokenIs(token.VARARGS) ||
		p.curTokenIs(token.NOSAVE) || p.curTokenIs(token.NOMASK) {
		if p.curTokenIs(token.VARARGS) {
			isVarargs = true
		}
		p.nextToken()
	}

	if p.isTypeToken(p.curToken.TokenType) {
		return p.parseTypedDeclarationStatement(isVarargs)
	}

	// 🚀 [關鍵修正] 支援省略回傳型別的函式定義 (預設為 mixed)
	// 語法: [modifiers] name(args) { ... }
	// 注意：這只應該發生在 top-level
	if topLevel && p.curTokenIs(token.IDENT) && p.peekTokenIs(token.LPAREN) {
	        // fmt.Printf("DEBUG: Found function definition candidate: %s\n", p.curToken.Literal)
	        typeToken := token.Token{TokenType: token.MIXED_TYPE, Literal: "mixed", Line: p.curToken.Line}

	        name := &ast.Ident{Token: p.curToken, Value: p.curToken.Literal}
	        return p.parseFunctionDefinition(typeToken, false, name, isVarargs)
	}
	switch p.curToken.TokenType {
	case token.IF:
		return p.parseIfStatement()
	case token.RETURN:
		return p.parseReturnStatement()
	case token.INHERIT:
		return p.parseInheritStatement()
	case token.FOR:
		return p.parseForStatement()
	case token.WHILE:
		return p.parseWhileStatement()
	case token.DO:
		return p.parseDoWhileStatement()
	case token.SWITCH:
		return p.parseSwitchStatement()
	case token.BREAK:
		return p.parseBreakStatement()
	case token.CONTINUE:
		return p.parseContinueStatement()
	case token.FOREACH:
		return p.parseForEachStatement()
	case token.LBRACE:
		return p.parseBlockStatement()
	case token.SEMICOLON:
		// 🚀 新增：支援空語句 (Lone Semicolon)
		return &ast.ExpressionStatement{Token: p.curToken}
	default:
		return p.ParseExpressionStatement()
	}
	return p.ParseExpressionStatement()
}

func (p *Parser) parseTypedDeclarationStatement(isVarargs bool) ast.Statement {
	typeToken := p.curToken 

	isArray := false
	if p.peekTokenIs(token.ASTARISK) {
		p.nextToken()
		isArray = true
	}

	if !p.expectPeek(token.IDENT) { 
		return nil
	}

	name := &ast.Ident{Token: p.curToken, Value: p.curToken.Literal}

	if p.peekTokenIs(token.LPAREN) {
	        // fmt.Printf("DEBUG: parseTypedDeclarationStatement calling parseFunctionDefinition for %s\n", name.Value)
	        return p.parseFunctionDefinition(typeToken, isArray, name, isVarargs)
	}

	return p.parseTypedVariableDeclaration(typeToken, isArray, name)
}

func (p *Parser) parseTypedVariableDeclaration(typeToken token.Token, isArray bool, name *ast.Ident) ast.Statement {
	var statements []ast.Statement

	// 處理第一個變數
	stmt := &ast.TypedVarDecl{
		Token:   typeToken,
		IsArray: isArray,
		Name:    name,
	}

	if p.peekTokenIs(token.ASSIGN) {
		p.nextToken()
		p.nextToken()
		stmt.Value = p.ParseExpression(COMMA)
	}
	statements = append(statements, stmt)

	// 處理逗號分隔的後續變數 (例如 int a, b, c;)
	for p.peekTokenIs(token.COMMA) {
		p.nextToken() // 跳過逗號

		// 支援 int a, *b; 這種 LPC 語法
		nextIsArray := isArray // 預設跟隨前面的型別，但可以被 * 覆蓋/強制
		if p.peekTokenIs(token.ASTARISK) {
			p.nextToken()
			nextIsArray = true
		}

		if !p.expectPeek(token.IDENT) {
			return nil
		}

		nextName := &ast.Ident{Token: p.curToken, Value: p.curToken.Literal}
		nextStmt := &ast.TypedVarDecl{
			Token:   typeToken,
			IsArray: nextIsArray,
			Name:    nextName,
		}

		if p.peekTokenIs(token.ASSIGN) {
			p.nextToken()
			p.nextToken()
			nextStmt.Value = p.ParseExpression(LOWEST)
		}
		statements = append(statements, nextStmt)
	}

	for p.peekTokenIs(token.SEMICOLON) {
		p.nextToken()
	}

	if len(statements) == 1 {
		return statements[0]
	}

	return &ast.BlockStatement{
		Token:      typeToken,
		Statements: statements,
	}
}

func (p *Parser) parseFunctionDefinition(typeToken token.Token, isArray bool, name *ast.Ident, isVarargs bool) ast.Statement {
	stmt := &ast.FunctionDef{
		Token:     typeToken,
		IsArray:   isArray,
		IsVarargs: isVarargs,
		Name:      name,
	}

	if !p.expectPeek(token.LPAREN) {
		return nil
	}

	stmt.Params = p.parseTypedParameters()

	// 🚀 支援函數原型 (Prototype): int foo();
	if p.peekTokenIs(token.SEMICOLON) {
		p.nextToken()
		return nil // 目前跳過原型宣告，不加入 AST
	}

	if !p.expectPeek(token.LBRACE) {
		return nil
	}

	stmt.Body = p.parseBlockStatement()

	return stmt
}

func (p *Parser) parseTypedParameters() []*ast.TypedParam {
	var params []*ast.TypedParam

	if p.peekTokenIs(token.RPAREN) {
		p.nextToken()
		return params
	}

	p.nextToken()

	for {
		var paramType token.Token
		paramIsArray := false
		var paramName *ast.Ident

		if p.isTypeToken(p.curToken.TokenType) {
			paramType = p.curToken
			if p.peekTokenIs(token.ASTARISK) {
				p.nextToken()
				paramIsArray = true
			}
			if p.peekTokenIs(token.IDENT) {
				p.nextToken()
				paramName = &ast.Ident{Token: p.curToken, Value: p.curToken.Literal}
			} else {
				// 🚀 支援原型宣告中的無名參數 (例如: func(string, int))
				paramName = &ast.Ident{Token: p.curToken, Value: fmt.Sprintf("__arg%d", len(params))}
			}
		} else if p.curTokenIs(token.IDENT) {
			// 🚀 支援省略型別的參數 (預設為 mixed)
			paramType = token.Token{TokenType: token.MIXED_TYPE, Literal: "mixed", Line: p.curToken.Line}
			paramName = &ast.Ident{Token: p.curToken, Value: p.curToken.Literal}
		} else {
			return nil
		}

		params = append(params, &ast.TypedParam{
			TypeToken: paramType,
			IsArray:   paramIsArray,
			Name:      paramName,
		})

		if p.peekTokenIs(token.COMMA) {
			p.nextToken()
			p.nextToken()
		} else {
			break
		}
	}
	if !p.expectPeek(token.RPAREN) {
		return nil
	}

	return params
}

func (p *Parser) parseInheritStatement() ast.Statement {
	stmt := &ast.InheritStatement{Token: p.curToken}

	if !p.expectPeek(token.STRING) {
		return nil
	}

	stmt.Path = p.curToken.Literal

	if p.peekTokenIs(token.SEMICOLON) {
		p.nextToken()
	}

	return stmt
}

func (p *Parser) parseAssignExpression(left ast.Expression) ast.Expression {
	expr := &ast.AssignExpression{
		Token:    p.curToken,
		Operator: p.curToken.Literal,
		Left:     left,
	}
	prec := p.curPrecedence()
	p.nextToken()
	expr.Value = p.ParseExpression(prec - 1) 
	return expr
}

func (p *Parser) parsePostfixExpression(left ast.Expression) ast.Expression {
	return &ast.PostfixExpression{
		Token:    p.curToken,
		Operator: p.curToken.Literal,
		Left:     left,
	}
}

func (p *Parser) parseBreakStatement() ast.Statement {
	stmt := &ast.BreakStatement{Token: p.curToken}
	if p.peekTokenIs(token.SEMICOLON) { p.nextToken() }
	return stmt
}
func (p *Parser) parseContinueStatement() ast.Statement {
	stmt := &ast.ContinueStatement{Token: p.curToken}
	if p.peekTokenIs(token.SEMICOLON) { p.nextToken() }
	return stmt
}

func (p *Parser) parseWhileStatement() ast.Statement {
	stmt := &ast.WhileStatement{Token: p.curToken}
	if !p.expectPeek(token.LPAREN) { return nil }
	p.nextToken()
	stmt.Condition = p.ParseExpression(LOWEST)
	if !p.expectPeek(token.RPAREN) { return nil }

	if p.peekTokenIs(token.LBRACE) {
		p.nextToken()
		stmt.Body = p.parseBlockStatement()
	} else {
		// 支援單行 while (例如: while(x) i++;)
		p.nextToken()
		bodyStmt := p.parseStatement(false)
		stmt.Body = &ast.BlockStatement{
			Token:      p.curToken,
			Statements: []ast.Statement{bodyStmt},
		}
	}
	return stmt
}

// --- 實作 For ---
func (p *Parser) parseIfStatement() ast.Statement {
	stmt := &ast.ExpressionStatement{Token: p.curToken}
	stmt.Expression = p.parseIfExpression()

	// if 敘述之後通常不加分號，但如果有也要一併處理
	if p.peekTokenIs(token.SEMICOLON) {
		p.nextToken()
	}

	return stmt
}

func (p *Parser) parseForStatement() ast.Statement {
	stmt := &ast.ForStatement{Token: p.curToken}
	if !p.expectPeek(token.LPAREN) {
		return nil
	}
	p.nextToken() // 從 '(' 移到下一個 token (Init 的開頭)

	// 1. 解析 Init (可能是 int i = 0; 或是空 ;)
	if !p.curTokenIs(token.SEMICOLON) {
		stmt.Init = p.parseStatement(false)
	}
	
	// 處理分號：由於 parseStatement 有時會吃掉結尾的分號，我們必須彈性判斷
	if p.curTokenIs(token.SEMICOLON) {
		p.nextToken() // 吃掉分號，移動到 Condition 的開頭
	} else if p.peekTokenIs(token.SEMICOLON) {
		p.nextToken()
		p.nextToken()
	} else {
		p.addError(p.curToken, "for 迴圈初始化區塊缺少分號 ';'")
		return nil
	}

	// 2. 解析 Condition
	if !p.curTokenIs(token.SEMICOLON) {
		stmt.Condition = p.ParseExpression(LOWEST)
		p.nextToken() // 表達式解析完後，手動往下推一格
	}
	
	// 處理分號
	if p.curTokenIs(token.SEMICOLON) {
		p.nextToken() // 吃掉分號，移動到 Post 的開頭
	} else if p.peekTokenIs(token.SEMICOLON) {
		p.nextToken()
		p.nextToken()
	} else {
		p.addError(p.curToken, "for 迴圈條件區塊缺少分號 ';'")
		return nil
	}

	// 3. 解析 Post (例如 i++)
	if !p.curTokenIs(token.RPAREN) {
		stmt.Post = p.ParseExpression(LOWEST)
		p.nextToken() // 手動往下推一格
	}
	
	// 確保收尾是 ')'
	if !p.curTokenIs(token.RPAREN) {
		if p.peekTokenIs(token.RPAREN) {
			p.nextToken()
		} else {
			p.addError(p.curToken, "for 迴圈缺少結尾的 ')'")
			return nil
		}
	}

	// 4. 解析 Body
	if p.peekTokenIs(token.LBRACE) {
		p.nextToken()
		stmt.Body = p.parseBlockStatement()
	} else {
		// 支援單行 for (例如: for(i=0;i<10;i++) write(i);)
		p.nextToken()
		bodyStmt := p.parseStatement(false)
		stmt.Body = &ast.BlockStatement{
			Token:      p.curToken,
			Statements: []ast.Statement{bodyStmt},
		}
	}

	return stmt
}

func (p *Parser) parseDoWhileStatement() ast.Statement {
	stmt := &ast.DoWhileStatement{Token: p.curToken}
	
	if p.peekTokenIs(token.LBRACE) {
		p.nextToken()
		stmt.Body = p.parseBlockStatement()
	} else {
		// 支援單行 do (例如: do i++; while(i<10);)
		p.nextToken()
		bodyStmt := p.parseStatement(false)
		stmt.Body = &ast.BlockStatement{
			Token:      p.curToken,
			Statements: []ast.Statement{bodyStmt},
		}
	}

	if !p.expectPeek(token.WHILE) { return nil }
	if !p.expectPeek(token.LPAREN) { return nil }
	p.nextToken()
	stmt.Condition = p.ParseExpression(LOWEST)
	if !p.expectPeek(token.RPAREN) { return nil }
	if p.peekTokenIs(token.SEMICOLON) { p.nextToken() }
	return stmt
}

func (p *Parser) parseSwitchStatement() ast.Statement {
	stmt := &ast.SwitchStatement{Token: p.curToken}
	if !p.expectPeek(token.LPAREN) { return nil }
	p.nextToken()
	stmt.Value = p.ParseExpression(LOWEST)
	if !p.expectPeek(token.RPAREN) { return nil }
	if !p.expectPeek(token.LBRACE) { return nil }

	p.nextToken()
	for !p.curTokenIs(token.RBRACE) && !p.curTokenIs(token.EOF) {
		if p.curTokenIs(token.CASE) || p.curTokenIs(token.DEFAULT) {
			caseStmt := &ast.CaseStatement{Token: p.curToken}
			if p.curTokenIs(token.CASE) {
				p.nextToken()
				caseStmt.Value = p.ParseExpression(LOWEST)
			}
			if !p.expectPeek(token.COLON) { return nil }
			p.nextToken()
			
			for !p.curTokenIs(token.CASE) && !p.curTokenIs(token.DEFAULT) && !p.curTokenIs(token.RBRACE) && !p.curTokenIs(token.EOF) {
				s := p.parseStatement(false)
				if s != nil { caseStmt.Body = append(caseStmt.Body, s) }
				p.nextToken()
			}
			stmt.Cases = append(stmt.Cases, caseStmt)
		} else {
			p.nextToken() 
		}
	}
	return stmt
}

func (p *Parser) parseTypePrefix() ast.Expression {
	typeToken := p.curToken
	isArray := false
	if p.peekTokenIs(token.ASTARISK) {
		p.nextToken()
		isArray = true
	}

	if !p.expectPeek(token.IDENT) {
		return nil
	}

	name := &ast.Ident{Token: p.curToken, Value: p.curToken.Literal}

	decl := &ast.TypedVarDecl{
		Token:   typeToken,
		IsArray: isArray,
		Name:    name,
	}

	if p.peekTokenIs(token.ASSIGN) {
		p.nextToken()
		p.nextToken()
		decl.Value = p.ParseExpression(LOWEST)
	}

	return decl
}

func (p *Parser) parseMappingLiteral() ast.Expression {
	mapping := &ast.MappingLiteral{
		Token: p.curToken,
		Pairs: make(map[ast.Expression]ast.Expression),
	}

	for !p.peekTokenIs(token.RBRACKET) {
		p.nextToken()

		key := p.ParseExpression(COMMA) // 🚀 改為 COMMA，遇到 : 或 , 會停止

		if !p.expectPeek(token.COLON) {
			return nil
		}

		p.nextToken()
		value := p.ParseExpression(COMMA) // 🚀 改為 COMMA，遇到 , 或 ] 會停止
		mapping.Pairs[key] = value

		if !p.peekTokenIs(token.RBRACKET) {
			if !p.expectPeek(token.COMMA) {
				return nil
			}
		}
	}
	if !p.expectPeek(token.RBRACKET) { 
		return nil
	}

	if !p.expectPeek(token.RPAREN) { 
		return nil
	}

	return mapping
}

func (p *Parser) parsePrefixScope() ast.Expression {
	if !p.expectPeek(token.IDENT) {
		return nil
	}
	return &ast.Ident{
		Token: p.curToken, 
		Value: "::" + p.curToken.Literal,
	}
}

func (p *Parser) parseInfixScope(left ast.Expression) ast.Expression {
	leftIdent, ok := left.(*ast.Ident)
	if !ok {
		p.addError(p.curToken, "在 '::' 前面必須是變數或類別名稱")
		return nil
	}

	if !p.expectPeek(token.IDENT) {
		return nil
	}

	return &ast.Ident{
		Token: p.curToken,
		Value: leftIdent.Value + "::" + p.curToken.Literal,
	}
}

func (p *Parser) parseCallOtherExpression(left ast.Expression) ast.Expression {
	expr := &ast.CallOtherExpression{
		Token:  p.curToken,
		Object: left,
	}

	if !p.expectPeek(token.IDENT) {
		return nil
	}

	expr.Method = &ast.Ident{
		Token: p.curToken,
		Value: p.curToken.Literal,
	}

	if !p.expectPeek(token.LPAREN) {
		return nil
	}

	expr.Arguments = p.ParseExpressionList(token.RPAREN)

	return expr
}

func (p *Parser) parseClosureLiteral() ast.Expression {
	lit := &ast.ClosureLiteral{Token: p.curToken}

	for !p.peekTokenIs(token.EOF) {
		// 1. 檢查結束符號 :) 或 )
		if p.peekTokenIs(token.COLON) && p.peekPeekToken.TokenType == token.RPAREN {
			p.nextToken() // cur = :
			p.nextToken() // cur = )
			return lit
		}
		if p.peekTokenIs(token.RPAREN) {
			p.nextToken() // cur = )
			return lit
		}

		// 2. 前進到下一個 token
		p.nextToken()

		// 3. 忽略空語句或多餘的分隔符
		if p.curTokenIs(token.SEMICOLON) || p.curTokenIs(token.COMMA) {
			continue
		}

		// 4. 解析 Return 語句或表達式
		if p.curTokenIs(token.RETURN) {
			lit.Elements = append(lit.Elements, p.parseReturnStatement())
		} else if p.isTypeToken(p.curToken.TokenType) {
			// 支援型別宣告 (例如 mapping qdata = ...;)
			lit.Elements = append(lit.Elements, p.parseTypePrefix())
		} else {
			expr := p.ParseExpression(LOWEST)
			if expr != nil {
				lit.Elements = append(lit.Elements, expr)
			}
		}
		
		// 5. 如果下一個是分隔符，可以在下一次循環處理它 (或在這裡跳過以提高效率)
		for p.peekTokenIs(token.SEMICOLON) || p.peekTokenIs(token.COMMA) || 
		    (p.peekTokenIs(token.COLON) && p.peekPeekToken.TokenType != token.RPAREN) {
			p.nextToken()
		}
	}

	return lit
}

func (p *Parser) parseForEachStatement() ast.Statement {
	stmt := &ast.ForEachStatement{Token: p.curToken}

	if !p.expectPeek(token.LPAREN) { return nil }
	p.nextToken()

	// 支援型別宣告: foreach (string s in stats)
	if p.isTypeToken(p.curToken.TokenType) {
		p.nextToken()
		// 支援陣列指標型別: foreach (string *s in stats)
		if p.curTokenIs(token.ASTARISK) {
			p.nextToken()
		}
	}

	firstIdent := &ast.Ident{Token: p.curToken, Value: p.curToken.Literal}
	
	p.nextToken() 

	if p.curToken.TokenType == token.COMMA {
		stmt.Key = firstIdent
		p.nextToken() 

		// 支援第二個變數的型別宣告: foreach (int k, string v in mapping)
		if p.isTypeToken(p.curToken.TokenType) {
			p.nextToken()
			if p.curTokenIs(token.ASTARISK) {
				p.nextToken()
			}
		}

		stmt.Value = &ast.Ident{Token: p.curToken, Value: p.curToken.Literal}
		p.nextToken()
	} else {
		stmt.Value = firstIdent
	}

	if p.curToken.TokenType != token.IN {
		p.addError(p.curToken, "foreach 語法缺少 'in' 關鍵字")
		return nil
	}
	p.nextToken() 

	stmt.Collection = p.ParseExpression(LOWEST)

	if !p.expectPeek(token.RPAREN) { return nil }
	
	if p.peekTokenIs(token.LBRACE) {
		p.nextToken()
		stmt.Body = p.parseBlockStatement()
	} else {
		// 支援單行 foreach (例如: foreach(s in list) write(s);)
		p.nextToken()
		bodyStmt := p.parseStatement(false)
		stmt.Body = &ast.BlockStatement{
			Token:      p.curToken,
			Statements: []ast.Statement{bodyStmt},
		}
	}

	return stmt
}

func (p *Parser) parseLPCArrayLiteral() ast.Expression {
	lit := &ast.ArrayLiteral{
		Token: p.curToken,
	}
	lit.Elements = p.ParseExpressionList(token.RBRACE)
	if !p.expectPeek(token.RPAREN) {
		return nil
	}
	return lit
}

func (p *Parser) parseTernaryExpression(condition ast.Expression) ast.Expression {
	expression := &ast.TernaryExpression{
		Token:     p.curToken,
		Condition: condition,
	}

	p.nextToken()
	// 🚀 關鍵：在三元運算子的分支中，應該允許較低優先權的表達式 (如賦值)
	expression.TrueResult = p.ParseExpression(COMMA)

	if !p.expectPeek(token.COLON) {
		return nil
	}

	p.nextToken()
	expression.FalseResult = p.ParseExpression(COMMA)

	return expression
}

func (p *Parser) parseRangeExpression(left ast.Expression) ast.Expression {
	expression := &ast.RangeExpression{
		Token: p.curToken,
		Start: left,
	}

	precedence := p.curPrecedence()
	p.nextToken()
	expression.End = p.ParseExpression(precedence)

	return expression
}
