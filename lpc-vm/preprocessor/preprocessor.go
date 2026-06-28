// preprocessor/preprocessor.go

package preprocessor

import (
	"bufio"
	"fmt"
	"io/fs"
	"os"
	"path/filepath"
	"regexp"
	"sort"
	"strconv"
	"strings"
)

type Macro struct {
	Name string
	Args []string
	Body string
}

type Preprocessor struct {
	MudLibPath     string
	Macros         map[string]Macro
	EmbeddedFS     fs.FS                     // 🚀 新增：支援嵌入式檔案系統
	GlobalInclude  string                    // 🚀 新增：全域自動引入標頭檔
	StripModifiers bool                      // 🚀 新增：是否移除 static, varargs, nomask 等修飾詞
	regexCache     map[string]*regexp.Regexp // 🚀 新增：快取 Regex 以提升效能
}

func New(mudLibPath string) *Preprocessor {
	return &Preprocessor{
		MudLibPath: mudLibPath,
		Macros:     make(map[string]Macro),
		regexCache: make(map[string]*regexp.Regexp),
	}
}

// ParseMacros 專門用來解析標頭檔中的巨集定義
func (p *Preprocessor) ParseMacros(input string) map[string]Macro {
	p.processInternal("internal_macros.h", input, 0)
	return p.Macros
}

// ... rest of methods ...

func (p *Preprocessor) getRegex(name string) *regexp.Regexp {
	if re, ok := p.regexCache[name]; ok {
		return re
	}
	re := regexp.MustCompile(`\b` + regexp.QuoteMeta(name) + `\b`)
	p.regexCache[name] = re
	return re
}

// SetEmbeddedFS 設定嵌入式檔案系統
func (p *Preprocessor) SetEmbeddedFS(efs fs.FS) {
	p.EmbeddedFS = efs
}

// 用來追蹤 if/elif/else 狀態的結構
type condState struct {
	isSkipping bool // 目前這個區塊是否正在被跳過
	hasMatched bool // 這個 if/elif 鏈中，是否已經有任何一個條件成立過？
}

func (p *Preprocessor) Process(filename, input string) (string, error) {
	content, err := p.processInternal(filename, input, 0)
	if err != nil {
		return "", err
	}
	result, _ := p.replaceMacros(content, false)
	return result, nil
}

// replaceMacros 執行巨集替換，支援遞迴替換
func (p *Preprocessor) replaceMacros(line string, initialInString bool) (string, bool) {
	if len(p.Macros) == 0 {
		return line, initialInString
	}

	// 🚀 關鍵修正：支援遞迴替換並追蹤字串狀態
	currentLine := line
	finalInString := initialInString

	// 取得所有巨集名稱並按長度排序 (由長到短)，避免 prefix 提早被取代
	var macroNames []string
	for name, m := range p.Macros {
		if len(m.Args) == 0 {
			macroNames = append(macroNames, name)
		}
	}
	sort.Slice(macroNames, func(i, j int) bool {
		return len(macroNames[i]) > len(macroNames[j])
	})

	for depth := 0; depth < 10; depth++ {
		// 1. 先處理函式型巨集 (目前簡化，仍使用 Regex)
		for name, m := range p.Macros {
			if len(m.Args) > 0 {
				currentLine = p.replaceFuncMacro(currentLine, name, m)
			}
		}

		// 2. 處理一般巨集，但必須避開字串
		var result strings.Builder
		inString := initialInString
		changed := false

		for i := 0; i < len(currentLine); i++ {
			if currentLine[i] == '"' && (i == 0 || currentLine[i-1] != '\\') {
				result.WriteByte(currentLine[i])
				inString = !inString
				continue
			}

			if inString {
				result.WriteByte(currentLine[i])
				continue
			}

			// 尋找巨集名稱
			found := false
			for _, name := range macroNames {
				m := p.Macros[name]
				if strings.HasPrefix(currentLine[i:], name) {
					// 檢查邊界
					endIdx := i + len(name)
					isStartWord := (i == 0 || !isAlphaNumeric(currentLine[i-1]))
					isEndWord := (endIdx == len(currentLine) || !isAlphaNumeric(currentLine[endIdx]))

					if isStartWord && isEndWord {
						result.WriteString(m.Body)
						i = endIdx - 1 // 跳過巨集名稱 (迴圈會再 +1)
						found = true
						changed = true
						break
					}
				}
			}

			if !found {
				result.WriteByte(currentLine[i])
			}
		}


		currentLine = result.String()
		finalInString = inString
		if !changed {
			break
		}
	}

	return currentLine, finalInString
}
func (p *Preprocessor) replaceFuncMacro(line, name string, m Macro) string {
	outLine := line
	searchIdx := 0
	for {
		idx := strings.Index(outLine[searchIdx:], name)
		if idx == -1 {
			break
		}
		idx += searchIdx

		// 檢查是否在字串內 (簡單判斷)
		if p.isInString(outLine, idx) {
			searchIdx = idx + 1
			continue
		}

		// 檢查是否為獨立單字
		isWord := true
		if idx > 0 && isAlphaNumeric(outLine[idx-1]) {
			isWord = false
		}
		if idx+len(name) < len(outLine) && isAlphaNumeric(outLine[idx+len(name)]) {
			isWord = false
		}

		if !isWord {
			searchIdx = idx + 1
			continue
		}

		// 尋找 '('
		rest := outLine[idx+len(name):]
		lparenIdx := strings.Index(rest, "(")
		if lparenIdx == -1 || strings.TrimSpace(rest[:lparenIdx]) != "" {
			searchIdx = idx + 1
			continue
		}

		// 尋找成對的 ')' (支援巢狀括號，且必須避開字串)
		start := idx + len(name) + lparenIdx + 1
		depth := 1
		rparenIdx := -1
		inString := false
		for i := start; i < len(outLine); i++ {
			if outLine[i] == '"' && (i == 0 || outLine[i-1] != '\\') {
				inString = !inString
				continue
			}
			if inString {
				continue
			}
			if outLine[i] == '(' {
				depth++
			} else if outLine[i] == ')' {
				depth--
				if depth == 0 {
					rparenIdx = i
					break
				}
			}
		}

		if rparenIdx == -1 {
			searchIdx = idx + 1
			continue
		}

		// 提取參數並拆分
		argStr := outLine[start:rparenIdx]
		providedArgs := splitMacroArgs(argStr)

		if len(providedArgs) != len(m.Args) {
			searchIdx = rparenIdx + 1
			continue
		}

		// 執行替換
		finalBody := m.Body
		for i, argName := range m.Args {
			val := strings.TrimSpace(providedArgs[i])
			argRe := p.getRegex(argName)
			finalBody = argRe.ReplaceAllString(finalBody, val)
		}

		// 組裝回原字串
		outLine = outLine[:idx] + finalBody + outLine[rparenIdx+1:]

		// 🚀 關鍵修正：確保 searchIdx 至少前進 1，防止 body 為空時陷入無限迴圈
		advance := len(finalBody)
		if advance == 0 {
			advance = 1
		}
		searchIdx = idx + advance
	}
	return outLine
}

func (p *Preprocessor) isInString(line string, pos int) bool {
	inString := false
	for i := 0; i < pos; i++ {
		if line[i] == '"' && (i == 0 || line[i-1] != '\\') {
			inString = !inString
		}
	}
	return inString
}

func (p *Preprocessor) processInternal(filename, input string, depth int) (string, error) {
	if depth > 50 {
		return "", fmt.Errorf("preprocessor recursion limit exceeded: %s", filename)
	}

	// 🚀 統一處理路徑格式，確保比對正確
	if !strings.HasPrefix(filename, "/") {
		filename = "/" + filename
	}

	// 🚀 關鍵修正：恢復全域自動引入處理 (只對非標頭檔且非自己進行)
	// 雖然我們有 Seed Macros，但在某些結構性陳述 (如 inherit) 中，
	// 顯式的標頭定義能確保解析器的行為與傳統 LPC 驅動程式一致。
	if p.GlobalInclude != "" && filename != p.GlobalInclude && !strings.HasSuffix(filename, ".h") {
		input = "#include <" + p.GlobalInclude + ">\n" + input
	}

	var output strings.Builder
	scanner := bufio.NewScanner(strings.NewReader(input))
	
	var condStack []condState // 改用新的狀態 Stack

	// 判斷目前是否處於被忽略的區塊中 (只要有任何一層正在跳過，就全部跳過)
	isSkipping := func() bool {
		for _, state := range condStack {
			if state.isSkipping {
				return true
			}
		}
		return false
	}
	inBlockComment := false
	inString := false

	for scanner.Scan() {
		line := scanner.Text()
		inString = false // 🚀 關鍵：LPC 字串不跨行，每行重置以避免狀態污染

		// 🚩 處理註解，支援 // 與 /* */
		var cleanLineBuilder strings.Builder
		
		// 🚀 關鍵：在剝離註解時，我們也需要追蹤字串狀態，
		// 但這只是「暫時」的，為了確保註解辨識正確。
		// 真正的持續性 inString 狀態會由 replaceMacros 維持。
		tempInString := inString

		for i := 0; i < len(line); i++ {
			// 1. 處理區塊註解結束
			if inBlockComment {
				if i < len(line)-1 && line[i] == '*' && line[i+1] == '/' {
					inBlockComment = false
					i++ // 跳過 '/'
				}
				continue
			}

			// 2. 處理字串切換 (暫時性，僅用於註解過濾)
			if line[i] == '"' && (i == 0 || line[i-1] != '\\') {
				tempInString = !tempInString
				cleanLineBuilder.WriteByte(line[i])
				continue
			}

			if !tempInString {
				// 3. 處理區塊註解開始
				if i < len(line)-1 && line[i] == '/' && line[i+1] == '*' {
					inBlockComment = true
					i++ // 跳過 '*'
					continue
				}
				// 4. 處理單行註解
				if i < len(line)-1 && line[i] == '/' && line[i+1] == '/' {
					break // 遇到單行註解，本行後續捨棄
				}
			}

			cleanLineBuilder.WriteByte(line[i])
		}

		cleanLine := cleanLineBuilder.String()

		trimmed := strings.TrimSpace(cleanLine)

		// ==========================================
		// 1. 處理條件編譯
		// ==========================================
		if strings.HasPrefix(trimmed, "#ifdef ") || strings.HasPrefix(trimmed, "#ifdef\t") {
			firstSpace := strings.IndexAny(trimmed, " \t")
			macroName := strings.TrimSpace(trimmed[firstSpace:])
			_, exists := p.Macros[macroName]
			condStack = append(condStack, condState{
				isSkipping: !exists,
				hasMatched: exists,
			})
			output.WriteString("\n")
			continue
		} else if strings.HasPrefix(trimmed, "#ifndef ") || strings.HasPrefix(trimmed, "#ifndef\t") {
			firstSpace := strings.IndexAny(trimmed, " \t")
			macroName := strings.TrimSpace(trimmed[firstSpace:])
			_, exists := p.Macros[macroName]
			condStack = append(condStack, condState{
				isSkipping: exists,
				hasMatched: !exists,
			})
			output.WriteString("\n")
			continue
		} else if strings.HasPrefix(trimmed, "#if ") || strings.HasPrefix(trimmed, "#if\t") { // [新增] 支援 #if 表達式
			firstSpace := strings.IndexAny(trimmed, " \t")
			condStr := strings.TrimSpace(trimmed[firstSpace:])
			isTrue := p.evalCondition(condStr)
			condStack = append(condStack, condState{
				isSkipping: !isTrue,
				hasMatched: isTrue,
			})
			output.WriteString("\n")
			continue
		} else if strings.HasPrefix(trimmed, "#elif ") || strings.HasPrefix(trimmed, "#elif\t") { // [升級] 讓 #elif 也支援表達式
			if len(condStack) > 0 {
				firstSpace := strings.IndexAny(trimmed, " \t")
				condStr := strings.TrimSpace(trimmed[firstSpace:])
				top := &condStack[len(condStack)-1]

				if top.hasMatched {
					// 前面已經有條件成立過，這裡必須強制跳過
					top.isSkipping = true
				} else {
					// 前面沒有條件成立，評估這個 elif 的表達式
					isTrue := p.evalCondition(condStr)
					top.isSkipping = !isTrue
					if isTrue {
						top.hasMatched = true
					}
				}
			}
			output.WriteString("\n")
			continue
		} else if trimmed == "#else" {
			if len(condStack) > 0 {
				top := &condStack[len(condStack)-1]
				if top.hasMatched {
					// 前面已經有條件成立過，else 必須跳過
					top.isSkipping = true
				} else {
					// 前面都沒成立，執行 else 區塊
					top.isSkipping = false
					top.hasMatched = true 
				}
			}
			output.WriteString("\n")
			continue
		} else if strings.HasPrefix(trimmed, "#endif") {
			if len(condStack) > 0 {
				condStack = condStack[:len(condStack)-1] // 彈出最後一層
			}
			output.WriteString("\n")
			continue
		}

		// ==========================================
		// 2. 處理巨集定義 (#define, #undef) 與 #pragma
		// ==========================================
		if strings.HasPrefix(trimmed, "#undef ") || strings.HasPrefix(trimmed, "#undef\t") {
			macroName := strings.TrimSpace(trimmed[6:])
			delete(p.Macros, macroName)
			output.WriteString("\n")
			continue
		}

		if strings.HasPrefix(trimmed, "#pragma ") || strings.HasPrefix(trimmed, "#pragma\t") {
			// 目前僅略過 #pragma，保留行號
			output.WriteString("\n")
			continue
		}

		if strings.HasPrefix(trimmed, "#define ") || strings.HasPrefix(trimmed, "#define\t") {
			fullDefine := trimmed
			// 支援多行定義 (結尾帶有 \)
			for strings.HasSuffix(strings.TrimSpace(fullDefine), "\\") {
				fullDefine = strings.TrimSuffix(strings.TrimSpace(fullDefine), "\\")
				if scanner.Scan() {
					nextLine := scanner.Text()
					// 多行定義時也要清理每一行的註解
					var cleanNextBuilder strings.Builder
					inStrNext := false
					for j := 0; j < len(nextLine); j++ {
						if nextLine[j] == '"' && (j == 0 || nextLine[j-1] != '\\') {
							inStrNext = !inStrNext
						}
						if !inStrNext && j < len(nextLine)-1 && nextLine[j] == '/' && nextLine[j+1] == '/' {
							break
						}
						cleanNextBuilder.WriteByte(nextLine[j])
					}
					fullDefine += " " + strings.TrimSpace(cleanNextBuilder.String())
					output.WriteString("\n") // 補償行號
				} else {
					break
				}
			}

			// 找出第一個空白或 tab 的位置
			firstSpace := strings.IndexAny(fullDefine, " \t")
			if firstSpace == -1 {
				output.WriteString("\n")
				continue
			}

			defineBody := strings.TrimSpace(fullDefine[firstSpace:])
			if defineBody == "" {
				output.WriteString("\n")
				continue
			}

			var name string
			var args []string
			var body string

			// 檢查是否為函式型巨集: #define YELLOW(x) CLR_YEL + x + CLR_NOR
			if idx := strings.Index(defineBody, "("); idx > 0 && !strings.Contains(defineBody[:idx], " ") {
				name = defineBody[:idx]
				endIdx := strings.Index(defineBody, ")")
				if endIdx > idx {
					argStr := defineBody[idx+1 : endIdx]
					if argStr != "" {
						for _, s := range strings.Split(argStr, ",") {
							args = append(args, strings.TrimSpace(s))
						}
					}
					body = strings.TrimSpace(defineBody[endIdx+1:])
				} else {
					// 格式錯誤，退回一般巨集處理
					parts := strings.Fields(defineBody)
					name = parts[0]
					if len(parts) > 1 {
						body = strings.TrimSpace(defineBody[len(name):])
					}
				}
			} else {
				// 一般巨集: #define PI 3.14
				parts := strings.Fields(defineBody)
				name = parts[0]
				if len(parts) > 1 {
					body = strings.TrimSpace(defineBody[len(name):])
				}
			}
			// 記錄巨集
			// fmt.Printf("DEBUG [%s]: Defining macro %s as %s (args: %v)\n", filename, name, body, args)
			p.Macros[name] = Macro{Name: name, Args: args, Body: body}

			// fmt.Printf("DEBUG: Defined macro %s = %s\n", name, body)
			output.WriteString("\n")
			continue
		}

		// 如果目前在忽略區塊內，直接塞入空行 (保留行號)
		if isSkipping() {
			output.WriteString("\n")
			continue
		}

		// ==========================================
		// 3. 處理檔案引入 (#include)
		// ==========================================
		if strings.HasPrefix(trimmed, "#include ") || strings.HasPrefix(trimmed, "#include\t") {
			firstSpace := strings.IndexAny(trimmed, " \t")
			includeBody := strings.TrimSpace(trimmed[firstSpace:])
			isSystem := strings.HasPrefix(includeBody, "<")
			pathStr := strings.Trim(includeBody, "\"<>")

			var relPath string
			if strings.HasPrefix(pathStr, "/") {
			        relPath = strings.TrimPrefix(pathStr, "/")
			} else {
			        // 先嘗試相對路徑
			        dir := filepath.Dir(strings.TrimPrefix(filename, "/"))
			        relPath = filepath.Join(dir, pathStr)

			        // 如果是系統引入，或者是相對路徑找不到，則嘗試 /include/
			        fullPath := filepath.Join(p.MudLibPath, relPath)
			        if _, statErr := os.Stat(fullPath); statErr != nil {
			            if isSystem || strings.Index(pathStr, "/") == -1 {
			                relPath = filepath.Join("include", pathStr)
			            }
			        }
			}
			relPath = strings.TrimPrefix(relPath, "/")
			var content []byte
			var err error

			// 1. 優先嘗試從實體磁碟讀取
			fullPath := filepath.Join(p.MudLibPath, relPath)
			if _, statErr := os.Stat(fullPath); statErr == nil {
				content, err = os.ReadFile(fullPath)
			} else if p.EmbeddedFS != nil {
				// 2. 備援嘗試從嵌入檔案讀取
				// 🚀 關鍵修正：根據配置的 MudLibPath 來尋找內嵌資源路徑
				embedPath := filepath.Join(p.MudLibPath, relPath)
				embedPath = filepath.ToSlash(embedPath)
				embedPath = strings.TrimPrefix(embedPath, "/")
				
				content, err = fs.ReadFile(p.EmbeddedFS, embedPath)
			} else {
				err = fmt.Errorf("找不到檔案: %s", pathStr)
			}

			if err != nil {
				return "", fmt.Errorf("前處理器錯誤: %v", err)
			}

			includedContent, err := p.processInternal(relPath, string(content), depth+1)
			if err != nil {
				return "", err
			}
			output.WriteString(includedContent + "\n")
			continue
		}

		// ==========================================
		// 4. 處理一般程式碼：修飾詞剝除
		// ==========================================
		outLine := cleanLine
		if p.StripModifiers {
			outLine = p.stripModifiers(outLine)
		}
		output.WriteString(outLine + "\n")

	}

	return output.String(), nil
}

func (p *Preprocessor) stripModifiers(line string) string {
	// 定義需要被移除的 LPC 修飾詞
	modifiers := []string{"static", "varargs", "nomask", "private", "protected", "public"}

	currentLine := line
	var result strings.Builder
	inString := false

	for i := 0; i < len(currentLine); i++ {
		// 🚩 避開字串內容
		if currentLine[i] == '"' && (i == 0 || currentLine[i-1] != '\\') {
			inString = !inString
			result.WriteByte(currentLine[i])
			continue
		}

		if inString {
			result.WriteByte(currentLine[i])
			continue
		}

		// 檢查是否遇到關鍵修飾詞
		found := false
		for _, mod := range modifiers {
			if strings.HasPrefix(currentLine[i:], mod) {
				endIdx := i + len(mod)
				// 必須是獨立單字 (前後非字母數字)
				isStart := (i == 0 || !isAlphaNumeric(currentLine[i-1]))
				isEnd := (endIdx == len(currentLine) || !isAlphaNumeric(currentLine[endIdx]))

				if isStart && isEnd {
					// 🚀 找到修飾詞，跳過它
					i = endIdx - 1
					found = true
					break
				}
			}
		}

		if !found {
			result.WriteByte(currentLine[i])
		}
	}
	return result.String()
}

func isAlphaNumeric(c byte) bool {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '$'
}

func splitMacroArgs(s string) []string {
	var args []string
	var current strings.Builder
	parenDepth := 0
	braceDepth := 0
	bracketDepth := 0
	inString := false
	for i := 0; i < len(s); i++ {
		c := s[i]
		if c == '"' && (i == 0 || s[i-1] != '\\') {
			inString = !inString
		}
		if !inString {
			if c == '(' { parenDepth++ } else if c == ')' { parenDepth-- } else if c == '{' { braceDepth++ } else if c == '}' { braceDepth-- } else if c == '[' { bracketDepth++ } else if c == ']' { bracketDepth-- } else if c == ',' && parenDepth == 0 && braceDepth == 0 && bracketDepth == 0 {
				args = append(args, current.String())
				current.Reset()
				continue
			}
		}
		current.WriteByte(c)
	}
	args = append(args, current.String())
	return args
}

// evalCondition 評估前處理器的邏輯判斷 (例如 "ENABLE_CRIT == 1")
func (p *Preprocessor) evalCondition(condStr string) bool {
	// 1. 先把條件式裡面的巨集替換為真實的數值字串
	for k, v := range p.Macros {
		// 使用 \b 確保只替換獨立的單字
		re := p.getRegex(k)
		// 如果巨集沒有值 (例如 #define MODE_A)，預設替換為 "1"
		replaceVal := v.Body
		if replaceVal == "" {
			replaceVal = "1"
		}
		condStr = re.ReplaceAllString(condStr, replaceVal)
	}

	condStr = strings.TrimSpace(condStr)

	// 如果替換後只剩下一個數字 (例如 "#if 1")
	if val, err := strconv.Atoi(condStr); err == nil {
		return val != 0
	}

	// 2. 尋找並拆解比較運算子
	operators := []string{"==", "!=", ">=", "<=", ">", "<"}
	var op string
	var parts []string

	for _, o := range operators {
		if strings.Contains(condStr, o) {
			op = o
			parts = strings.Split(condStr, o)
			break
		}
	}

	// 如果沒有合法的運算子，預設回傳 false
	if len(parts) != 2 {
		return false
	}

	leftStr := strings.TrimSpace(parts[0])
	rightStr := strings.TrimSpace(parts[1])

	// 3. 嘗試轉為整數進行比較
	leftVal, err1 := strconv.Atoi(leftStr)
	rightVal, err2 := strconv.Atoi(rightStr)

	// 如果不是數字 (可能是未定義的巨集，被當作字串 0)
	if err1 != nil { leftVal = 0 }
	if err2 != nil { rightVal = 0 }

	switch op {
	case "==": return leftVal == rightVal
	case "!=": return leftVal != rightVal
	case ">":  return leftVal > rightVal
	case "<":  return leftVal < rightVal
	case ">=": return leftVal >= rightVal
	case "<=": return leftVal <= rightVal
	}

	return false
}
