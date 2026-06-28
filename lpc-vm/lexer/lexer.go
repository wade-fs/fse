package lexer

import (
	"mudscript/lpc-vm/token"
	"strconv"
)

// Lexer represents a lexer for Monkey programming language.
type Lexer interface {
	NextToken() token.Token
	GetInput() string // [新增] 讓 Parser 能取得原始碼以產生漂亮的錯誤訊息
}

type lexer struct {
	input string
	position int
	readPosition int
	ch byte
	line int // [新增] 紀錄目前行號
}

// New returns a new Lexer.
func New(input string) Lexer {
	l := &lexer{input: input, line: 1} // 從第 1 行開始
	l.readChar()
	return l
}

func (l *lexer) GetInput() string {
	return l.input
}

func (l *lexer) readChar() {
	if l.ch == '\n' {
		l.line++ // 遇到換行，行號 +1
	}

	if l.readPosition >= len(l.input) {
		l.ch = 0
	} else {
		l.ch = l.input[l.readPosition]
	}
	l.position = l.readPosition
	l.readPosition++
}

// [修改] 讓 newToken 變成物件方法，這樣可以抓到 l.line
func (l *lexer) newToken(tokenType token.TokenType, ch byte) token.Token {
	return token.Token{
		TokenType: tokenType,
		Literal:   string(ch),
		Line:      l.line,
	}
}

func (l *lexer) NextToken() token.Token {
	var tok token.Token

	l.skipWhitespaceAndComments()

	currentLine := l.line // 記錄此 Token 發生的行號

	switch l.ch {
	case '(':
		if l.peekChar() == '[' { 
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.LBRACKET_MAP, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else if l.peekChar() == ':' { 
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.LPAREN_COLON, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else if l.peekChar() == '{' { 
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.LARRAY, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else {
			tok = l.newToken(token.LPAREN, l.ch)
		}
	case ':':
		if l.peekChar() == ':' { 
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.SCOPE, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else {
			tok = l.newToken(token.COLON, l.ch)
		}
	case ')':
		tok = l.newToken(token.RPAREN, l.ch)
	case ']':
		tok = l.newToken(token.RBRACKET, l.ch)
	case '[':
		tok = l.newToken(token.LBRACKET, l.ch)
	case '&':
		if l.peekChar() == '&' {
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.AND, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else if l.peekChar() == '=' {
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.AND_EQUALS, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else {
			tok = l.newToken(token.BIT_AND, l.ch) 
		}
	case '|':
		if l.peekChar() == '|' {
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.OR, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else if l.peekChar() == '=' {
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.OR_EQUALS, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else {
			tok = l.newToken(token.BIT_OR, l.ch) 
		}
	case '^':
		if l.peekChar() == '=' {
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.XOR_EQUALS, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else {
			tok = l.newToken(token.BIT_XOR, l.ch)
		}
	case '~':
		tok = l.newToken(token.BIT_NOT, l.ch)
	case '=':
		if l.peekChar() == '=' {
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.EQ, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else {
			tok = l.newToken(token.ASSIGN, l.ch)
		}
	case '!':
		if l.peekChar() == '=' {
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.NEQ, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else {
			tok = l.newToken(token.BANG, l.ch)
		}
	case ';':
		tok = l.newToken(token.SEMICOLON, l.ch)
	case '?':
		tok = l.newToken(token.QUESTION, l.ch)
	case ',':
		tok = l.newToken(token.COMMA, l.ch)
	case '+':
		if l.peekChar() == '+' {
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.INC, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else if l.peekChar() == '=' {
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.PLUS_EQUALS, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else {
			tok = l.newToken(token.PLUS, l.ch)
		}
	case '-':
		if l.peekChar() == '>' {
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.ARROW, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else if l.peekChar() == '-' {
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.DEC, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else if l.peekChar() == '=' {
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.MINUS_EQUALS, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else {
			tok = l.newToken(token.MINUS, l.ch)
		}
	case '*':
		if l.peekChar() == '=' {
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.ASTERISK_EQUALS, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else {
			tok = l.newToken(token.ASTARISK, l.ch)
		}
	case '/':
		if l.peekChar() == '=' {
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.SLASH_EQUALS, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else {
			tok = l.newToken(token.SLASH, l.ch)
		}
	case '%':
		if l.peekChar() == '=' {
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.MOD_EQUALS, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else {
			tok = l.newToken(token.MOD, l.ch)
		}
	case '<':
		if l.peekChar() == '=' {
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.LTE, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else if l.peekChar() == '<' {
			ch := l.ch
			l.readChar()
			if l.peekChar() == '=' {
				ch2 := l.ch
				l.readChar()
				tok = token.Token{TokenType: token.LSHIFT_EQUALS, Literal: string(ch) + string(ch2) + string(l.ch), Line: currentLine}
			} else {
				tok = token.Token{TokenType: token.LSHIFT, Literal: string(ch) + string(l.ch), Line: currentLine}
			}
		} else {
			tok = l.newToken(token.LT, l.ch)
		}
	case '>':
		if l.peekChar() == '=' {
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.GTE, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else if l.peekChar() == '>' {
			ch := l.ch
			l.readChar()
			if l.peekChar() == '=' {
				ch2 := l.ch
				l.readChar()
				tok = token.Token{TokenType: token.RSHIFT_EQUALS, Literal: string(ch) + string(ch2) + string(l.ch), Line: currentLine}
			} else {
				tok = token.Token{TokenType: token.RSHIFT, Literal: string(ch) + string(l.ch), Line: currentLine}
			}
		} else {
			tok = l.newToken(token.GT, l.ch)
		}
	case '.':
		if l.peekChar() == '.' {
			ch := l.ch
			l.readChar()
			tok = token.Token{TokenType: token.DOTDOT, Literal: string(ch) + string(l.ch), Line: currentLine}
		} else {
			tok = l.newToken(token.ILLEGAL, l.ch)
		}
	case '{':
		tok = l.newToken(token.LBRACE, l.ch)
	case '}':
		tok = l.newToken(token.RBRACE, l.ch)
	case '"':
		tok.TokenType = token.STRING
		tok.Literal = l.readString()
		tok.Line = currentLine
	case '\'':
		tok.TokenType = token.CHAR
		tok.Literal = l.readCharLiteral()
		tok.Line = currentLine
	case '@':
		l.readChar()
		if isLetter(l.ch) {
			marker := l.readIdent()
			tok.TokenType = token.STRING
			tok.Literal = l.readHeredoc(marker)
			tok.Line = currentLine
			return tok
		}
		tok = l.newToken(token.ILLEGAL, '@')
	case 0:
		tok.Literal = ""
		tok.TokenType = token.EOF
		tok.Line = currentLine
	default:
		if isDigit(l.ch) {
			tok = l.readNumberToken()
			tok.Line = currentLine
			return tok
		}

		if isLetter(l.ch) {
			tok.Literal = l.readIdent()
			tok.TokenType = token.LookupIdent(tok.Literal)
			tok.Line = currentLine
			return tok
		}

		tok = l.newToken(token.ILLEGAL, l.ch)
	}

	l.readChar()
	return tok
}

func (l *lexer) skipMultiLineComment() {
	l.readChar() 
	l.readChar() 
	for l.ch != 0 {
		if l.ch == '*' && l.peekChar() == '/' {
			l.readChar() 
			l.readChar() 
			return
		}
		l.readChar()
	}
}

func (l *lexer) skipSingleLineComment() {
    for l.ch != '\n' && l.ch != 0 {
        l.readChar()
    }
    l.skipWhitespace() 
}

func (l *lexer) skipWhitespace() {
	for l.ch == ' ' || l.ch == '\t' || l.ch == '\n' || l.ch == '\r' {
		l.readChar()
	}
}

func (l *lexer) peekChar() byte {
	if l.readPosition >= len(l.input) {
		return 0
	}
	return l.input[l.readPosition]
}

func (l *lexer) readString() string {
	var out []byte
	for {
		l.readChar()
		if l.ch == '"' || l.ch == 0 {
			break
		}

		if l.ch == '\\' {
			l.readChar()
			switch l.ch {
			case 'n':
				out = append(out, '\n')
			case 't':
				out = append(out, '\t')
			case 'r':
				out = append(out, '\r')
			case '"':
				out = append(out, '"')
			case '\\':
				out = append(out, '\\')
			case 'x':
				// 🚀 支援 \xHH 十六進位逸脫字元
				h1 := l.peekChar()
				if isHexDigit(h1) {
					l.readChar()
					h2 := l.peekChar()
					if isHexDigit(h2) {
						l.readChar()
						val, _ := strconv.ParseUint(string([]byte{h1, h2}), 16, 8)
						out = append(out, byte(val))
					} else {
						val, _ := strconv.ParseUint(string(h1), 16, 8)
						out = append(out, byte(val))
					}
				} else {
					out = append(out, '\\', 'x')
				}
			default:
				out = append(out, '\\', l.ch)
			}
		} else {
			out = append(out, l.ch)
		}
	}
	return string(out)
}

func (l *lexer) read(checkFn func(byte) bool) string {
	position := l.position
	for checkFn(l.ch) {
		l.readChar()
	}
	return l.input[position:l.position]
}

// 支援數字結尾的變數
func (l *lexer) readIdent() string {
	position := l.position
	for isLetter(l.ch) || isDigit(l.ch) {
		l.readChar()
	}
	return l.input[position:l.position]
}

func (l *lexer) readNumber() string {
	position := l.position

	if l.ch == '0' && (l.peekChar() == 'x' || l.peekChar() == 'X') {
		l.readChar() 
		l.readChar() 
		for isHexDigit(l.ch) {
			l.readChar()
		}
		return l.input[position:l.position]
	}

	for isDigit(l.ch) {
		l.readChar()
	}
	return l.input[position:l.position]
}

func (l *lexer) readNumberToken() token.Token {
	intPart := l.readNumber()
	if l.ch != '.' || l.peekChar() == '.' {
		return token.Token{
			TokenType: token.INT,
			Literal:   intPart,
		}
	}

	l.readChar()
	fracPart := l.readNumber()
	return token.Token{
		TokenType: token.FLOAT,
		Literal:   intPart + "." + fracPart,
	}
}

func isLetter(ch byte) bool {
	return 'a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' || ch == '_' || ch == '$'
}

func isDigit(ch byte) bool {
	return '0' <= ch && ch <= '9'
}

func (l *lexer) readCharLiteral() string {
	l.readChar() // skip first '
	
	var char byte
	if l.ch == '\\' {
		l.readChar()
		switch l.ch {
		case 'n':
			char = '\n'
		case 't':
			char = '\t'
		case 'r':
			char = '\r'
		case '\\':
			char = '\\'
		case '\'':
			char = '\''
		case 'x':
			// 🚀 支援 \xHH 十六進位逸脫字元
			h1 := l.peekChar()
			if isHexDigit(h1) {
				l.readChar()
				h2 := l.peekChar()
				if isHexDigit(h2) {
					l.readChar()
					val, _ := strconv.ParseUint(string([]byte{h1, h2}), 16, 8)
					char = byte(val)
				} else {
					val, _ := strconv.ParseUint(string(h1), 16, 8)
					char = byte(val)
				}
			} else {
				char = 'x'
			}
		default:
			char = l.ch
		}
	} else {
		char = l.ch
	}
	
	l.readChar() 
	if l.ch == '\'' {
		// correctly closed
	}
	return string(char)
}

func isHexDigit(ch byte) bool {
	return isDigit(ch) || ('a' <= ch && ch <= 'f') || ('A' <= ch && ch <= 'F')
}

// 🚀 支援 LPC Heredoc 語法 (@TEXT ... TEXT)
func (l *lexer) readHeredoc(marker string) string {
	var out []byte

	// 1. 吃掉 @TEXT 之後到行尾的所有東西
	for l.ch != '\n' && l.ch != 0 {
		l.readChar()
	}
	if l.ch == '\n' {
		l.readChar()
	}

	// 2. 逐行讀取，直到遇見 marker
	lineStart := true
	for l.ch != 0 {
		if lineStart {
			// 檢查前方是否剛好為 marker
			match := true
			for i := 0; i < len(marker); i++ {
				if l.position+i >= len(l.input) || l.input[l.position+i] != marker[i] {
					match = false
					break
				}
			}

			// 如果字串相符，且後面沒有跟著其他文字 (只有空白或換行或符號)
			if match {
				nextPos := l.position + len(marker)
				if nextPos >= len(l.input) || (!isLetter(l.input[nextPos]) && !isDigit(l.input[nextPos])) {
					// 確認這就是結尾 marker
					// 略過這個 marker，但不略過後面的符號 (例如 );)
					for i := 0; i < len(marker); i++ {
						l.readChar()
					}
					break // 結束 heredoc 讀取
				}
			}
		}

		out = append(out, l.ch)
		lineStart = (l.ch == '\n')
		l.readChar()
	}

	return string(out)
}

func (l *lexer) skipWhitespaceAndComments() {
	for {
		l.skipWhitespace()
		if l.ch == '/' && l.peekChar() == '/' {
			l.skipSingleLineComment()
		} else if l.ch == '/' && l.peekChar() == '*' {
			l.skipMultiLineComment()
		} else {
			break
		}
	}
}
