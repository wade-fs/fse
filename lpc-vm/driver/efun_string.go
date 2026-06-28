// driver/efun_string.go
package driver

import (
	"crypto/md5"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"regexp"
	"strconv"
	"strings"

	"golang.org/x/crypto/bcrypt"
	"mudscript/lpc-vm/object"
)

// ==========================================
// 8. 字串操作 (Strings)
// ==========================================
func (d *Driver) registerStringEfuns(obj *object.LPCObject) {

	// 語法: string *get_dir(string path, [int recursive])
	// 說明: 取得指定路徑下的所有檔案與目錄清單。
	//       - 支援萬用字元，例如 "/cmds/*.c"
	//       - 若為目錄，回傳的名稱結尾會帶有 "/" 方便判斷
	//       - recursive = 1 時，會遞迴往下掃描所有子目錄 (此模式下不支援萬用字元，需傳入明確目錄)
	// 範例:
	//   get_dir("/cmds/")          -> ({ "cmd_info.c", "cmd_look.c", "login.c", ... })
	//   get_dir("/data/user/*.o")  -> ({ "wade.o", "admin.o" })
	//   get_dir("/cmds/", 1)       -> ({ "cmd_info.c", "admin/cmd_shutdown.c", ... })
	obj.Vars.Set("get_dir", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Array{Elements: []object.Object{}}
			}
			pathArg, ok := args[0].(*object.String)
			if !ok {
				return object.NewError("get_dir 的第一個參數必須是字串")
			}

			resolvedPath := d.ResolvePath(obj.Filename, pathArg.Value)

			// 🚀 權限檢查
			allowed, errMsg := d.checkReadPermission(obj, resolvedPath, "get_dir")
			if !allowed {
				if p := d.GetCurrentPlayer(); p != nil {
					p.Send(fmt.Sprintf("\r\n⚠️ 系統安全攔截：%s\r\n", errMsg))
				}
				return &object.Array{Elements: []object.Object{}}
			}

			recursive := false
			details := false
			if len(args) > 1 {
				if flag, ok := args[1].(*object.Integer); ok {
					if flag.Value > 0 {
						recursive = true
					} else if flag.Value == -1 {
						details = true
					}
				}
			}

			// 處理路徑與安全防護 (防止 ../ 跳出 MudLib 目錄)
			searchPath := resolvedPath
			if !strings.HasPrefix(searchPath, "/") {
				searchPath = "/" + searchPath
			}
			fullPath := filepath.Clean(filepath.Join(d.Config.MudLibPath, searchPath))
			if !strings.HasPrefix(fullPath, filepath.Clean(d.Config.MudLibPath)) {
				return object.NewError("get_dir 權限錯誤：無法存取根目錄以外的檔案")
			}

			// 定義結果結構
			type fileInfo struct {
				Name  string
				Size  int64
				MTime int64
				IsDir bool
			}
			var results []fileInfo

			if recursive {
				// ── 模式 1：遞迴掃描目錄 ──
				info, err := os.Stat(fullPath)
				if err == nil && info.IsDir() {
					filepath.WalkDir(fullPath, func(path string, entry os.DirEntry, err error) error {
						if err != nil {
							return nil
						}
						if path == fullPath {
							return nil
						} // 略過根目錄自己

						// 取得相對於目標目錄的路徑
						rel, _ := filepath.Rel(fullPath, path)
						// 統一轉換路徑斜線為 LPC 習慣的 "/"
						rel = filepath.ToSlash(rel)

						info, _ := entry.Info()
						var size, mtime int64
						if info != nil {
							size = info.Size()
							mtime = info.ModTime().Unix()
						}

						name := rel
						if entry.IsDir() {
							name += "/"
						}

						results = append(results, fileInfo{
							Name:  name,
							Size:  size,
							MTime: mtime,
							IsDir: entry.IsDir(),
						})
						return nil
					})
				}
			} else {
				// ── 模式 2：單層目錄或萬用字元 ──
				if strings.Contains(searchPath, "*") || strings.Contains(searchPath, "?") {
					// 處理萬用字元 (例如 /cmds/*.c)
					matches, err := filepath.Glob(fullPath)
					if err == nil {
						for _, match := range matches {
							info, err := os.Stat(match)
							if err != nil {
								continue
							}
							_, name := filepath.Split(match)
							if info.IsDir() {
								name += "/"
							}
							results = append(results, fileInfo{
								Name:  name,
								Size:  info.Size(),
								MTime: info.ModTime().Unix(),
								IsDir: info.IsDir(),
							})
						}
					}
				} else {
					// 處理單純目錄讀取 (例如 /cmds/)
					entries, err := os.ReadDir(fullPath)
					if err == nil {
						for _, entry := range entries {
							info, _ := entry.Info()
							var size, mtime int64
							if info != nil {
								size = info.Size()
								mtime = info.ModTime().Unix()
							}
							name := entry.Name()
							if entry.IsDir() {
								name += "/"
							}
							results = append(results, fileInfo{
								Name:  name,
								Size:  size,
								MTime: mtime,
								IsDir: entry.IsDir(),
							})
						}
					} else {
						// 如果不是目錄但檔案存在，就回傳它自己
						info, err := os.Stat(fullPath)
						if err == nil && !info.IsDir() {
							_, name := filepath.Split(fullPath)
							results = append(results, fileInfo{
								Name:  name,
								Size:  info.Size(),
								MTime: info.ModTime().Unix(),
								IsDir: false,
							})
						}
					}
				}
			}

			// 轉換為 LPC Array 回傳
			elements := make([]object.Object, len(results))
			for i, res := range results {
				if details {
					// 回傳 ({ name, size, mtime })
					elements[i] = &object.Array{Elements: []object.Object{
						&object.String{Value: res.Name},
						&object.Integer{Value: res.Size},
						&object.Integer{Value: res.MTime},
					}}
				} else {
					elements[i] = &object.String{Value: res.Name}
				}
			}

			return &object.Array{Elements: elements}
		},
	})

	// 語法: string lower_case(string str)
	// 說明: 將字串中所有的大寫英文字母轉換為小寫。
	// 範例: lower_case("HELLO") -> "hello"
	obj.Vars.Set("lower_case", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) == 0 {
				return &object.String{Value: ""}
			}
			if s, ok := args[0].(*object.String); ok {
				return &object.String{Value: strings.ToLower(s.Value)}
			}
			return &object.String{Value: ""}
		},
	})

	// 語法: string upper_case(string str)
	// 說明: 將字串中所有的小寫英文字母轉換為大寫。
	// 範例: upper_case("hello") -> "HELLO"
	obj.Vars.Set("upper_case", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) == 0 {
				return &object.String{Value: ""}
			}
			if s, ok := args[0].(*object.String); ok {
				return &object.String{Value: strings.ToUpper(s.Value)}
			}
			return &object.String{Value: ""}
		},
	})

	// 語法: string capitalize(string str)
	// 說明: 將字串的第一個英文字母轉換為大寫。
	// 範例: capitalize("apple") -> "Apple"
	obj.Vars.Set("capitalize", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) == 0 {
				return &object.String{Value: ""}
			}
			if s, ok := args[0].(*object.String); ok && len(s.Value) > 0 {
				runes := []rune(s.Value)
				runes[0] = []rune(strings.ToUpper(string(runes[0])))[0]
				return &object.String{Value: string(runes)}
			}
			return &object.String{Value: ""}
		},
	})

	// 語法: string trim(string str, [string cutset])
	// 說明: 移除字串前後的空白字元 (包含空白、換行與 Tab)。
	// 範例: trim("  hello  ") -> "hello"
	obj.Vars.Set("trim", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) == 0 {
				return &object.String{Value: ""}
			}
			s, ok := args[0].(*object.String)
			if !ok {
				return &object.String{Value: ""}
			}
			if len(args) > 1 {
				if cutset, ok := args[1].(*object.String); ok {
					return &object.String{Value: strings.Trim(s.Value, cutset.Value)}
				}
			}
			return &object.String{Value: strings.TrimSpace(s.Value)}
		},
	})

	// 語法: string replace_string(string str, string pattern, string replace)
	// 說明: 將字串 str 中的所有 pattern 替換為 replace。
	// 範例: replace_string("hello world", "world", "mud") -> "hello mud"
	obj.Vars.Set("replace_string", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 3 {
				return object.NewError("replace_string 需要 3 個參數")
			}
			str, ok1 := args[0].(*object.String)
			pattern, ok2 := args[1].(*object.String)
			repl, ok3 := args[2].(*object.String)
			if !ok1 || !ok2 || !ok3 {
				return object.NewError("replace_string 的參數必須都是字串")
			}
			return &object.String{Value: strings.ReplaceAll(str.Value, pattern.Value, repl.Value)}
		},
	})

	// 語法: string sprintf(string format, ...)
	// 說明: C 語言風格的字串格式化。
	// 範例: sprintf("HP: %d/%d", 10, 20) -> "HP: 10/20"
	obj.Vars.Set("sprintf", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) == 0 {
				return object.NewError("sprintf 需要參數")
			}
			formatObj, ok := args[0].(*object.String)
			if !ok {
				return object.NewError("第一個參數必須是字串")
			}

			// 🚀 對於 %O 格式符（LPC 物件 Inspect），將其替換為 %s（因為 arguments 都會被轉成字串表示法）
			// 保留寬度/對齊修飾，例如 %-20O 轉成 %-20s
			var reO = regexp.MustCompile(`(%[-+ 0]*[0-9]*(\.[0-9]+)?)O`)
			formatStr := reO.ReplaceAllString(formatObj.Value, "${1}s")
			
			var goArgs []interface{}
			for _, arg := range args[1:] {
				if arg == nil {
					goArgs = append(goArgs, "nil")
				} else {
					switch v := arg.(type) {
					case *object.Integer:
						goArgs = append(goArgs, v.Value)
					case *object.Float:
						goArgs = append(goArgs, v.Value)
					case *object.String:
						goArgs = append(goArgs, v.Value)
					default:
						goArgs = append(goArgs, arg.Inspect())
					}
				}
			}
			
			// 防護：使用 recover 避免 sprintf 拋出 panic
			defer func() {
				if r := recover(); r != nil {
					log.Printf("sprintf error: %v, format: %s", r, formatStr)
				}
			}()

			result := fmt.Sprintf(formatStr, goArgs...)
			return &object.String{Value: result}
		},
	})

	// 語法: int strlen(string str)
	// 說明: 回傳字串長度。
	// 範例: strlen("hello") -> 5
	obj.Vars.Set("strlen", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) != 1 {
				return &object.Integer{Value: 0}
			}
			if str, ok := args[0].(*object.String); ok {
				return &object.Integer{Value: int64(len([]rune(str.Value)))}
			}
			return &object.Integer{Value: 0}
		},
	})

	// 語法: string substr(string str, int start, [int length])
	// 說明: 截取子字串。
	// 範例: substr("hello", 1, 3) -> "ell"
	obj.Vars.Set("substr", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return object.NewError("substr 需 2 個參數")
			}
			str, ok1 := args[0].(*object.String)
			start, ok2 := args[1].(*object.Integer)
			if !ok1 || !ok2 {
				return object.NewError("substr 型別錯誤")
			}

			runes := []rune(str.Value)
			length := len(runes)
			sIdx := int(start.Value)
			if sIdx < 0 {
				sIdx = length + sIdx
			}
			if sIdx < 0 {
				sIdx = 0
			}
			if sIdx >= length {
				return &object.String{Value: ""}
			}

			eIdx := length
			if len(args) > 2 {
				if l, ok := args[2].(*object.Integer); ok {
					eIdx = sIdx + int(l.Value)
				}
			}
			if eIdx > length {
				eIdx = length
			}
			if eIdx < sIdx {
				return &object.String{Value: ""}
			}

			return &object.String{Value: string(runes[sIdx:eIdx])}
		},
	})

	// 語法: mixed regexp(mixed list, string pattern)
	// 說明: 若 list 為字串，則回傳 1 (匹配) 或 0 (不匹配)。
	// 範例: regexp(({ "apple", "banana" }), "a") -> ({ "apple", "banana" })
	obj.Vars.Set("regexp", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return object.NewError("regexp 需 2 個參數")
			}
			patternObj, ok := args[1].(*object.String)
			if !ok {
				return object.NewError("regexp 第二個參數必須是字串")
			}

			re, err := regexp.Compile(patternObj.Value)
			if err != nil {
				return object.NewError("regexp 格式錯誤: %v", err)
			}

			switch list := args[0].(type) {
			case *object.String:
				if re.MatchString(list.Value) {
					return &object.Integer{Value: 1}
				}
				return &object.Integer{Value: 0}
			case *object.Array:
				var result []object.Object
				for _, el := range list.Elements {
					if s, ok := el.(*object.String); ok {
						if re.MatchString(s.Value) {
							result = append(result, s)
						}
					}
				}
				return &object.Array{Elements: result}
			default:
				return object.NewError("regexp 第一個參數必須是字串或陣列")
			}
		},
	})

	// 語法: string break_string(string str, int width, [int indent])
	// 說明: 將字串按指定寬度折行。
	// 範例: break_string(long_desc, 78)
	obj.Vars.Set("break_string", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return args[0]
			}
			str, ok1 := args[0].(*object.String)
			width, ok2 := args[1].(*object.Integer)
			if !ok1 || !ok2 {
				return args[0]
			}

			indent := ""
			if len(args) > 2 {
				if i, ok := args[2].(*object.String); ok {
					indent = i.Value
				}
				if i, ok := args[2].(*object.Integer); ok {
					indent = strings.Repeat(" ", int(i.Value))
				}
			}

			w := int(width.Value)
			if w < 1 {
				w = 80
			}

			words := strings.Fields(str.Value)
			if len(words) == 0 {
				return &object.String{Value: ""}
			}

			var result strings.Builder
			currentLine := indent
			for _, word := range words {
				if len(currentLine)+len(word)+1 > w {
					result.WriteString(currentLine + "\n")
					currentLine = indent + word
				} else {
					if currentLine == indent {
						currentLine += word
					} else {
						currentLine += " " + word
					}
				}
			}
			result.WriteString(currentLine + "\n")
			return &object.String{Value: result.String()}
		},
	})

	// 語法: int strsrch(string str, string pattern, [int reverse])
	// 說明: 尋找 pattern 在 str 中第一次出現的位置。
	// 範例: strsrch("hello", "l") -> 2
	obj.Vars.Set("strsrch", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return object.NewError("strsrch 需 2 個參數")
			}
			str, ok1 := args[0].(*object.String)
			if !ok1 {
				return object.NewError("strsrch 第一個參數必須是字串")
			}

			var patternStr string
			if p, ok := args[1].(*object.String); ok {
				patternStr = p.Value
			} else if p, ok := args[1].(*object.Integer); ok {
				patternStr = string(byte(p.Value))
			} else {
				return object.NewError("strsrch 第二個參數必須是字串或整數")
			}

			reverse := false
			if len(args) > 2 {
				if flag, ok := args[2].(*object.Integer); ok && flag.Value != 0 {
					reverse = true
				}
			}

			var byteIdx int
			if reverse {
				byteIdx = strings.LastIndex(str.Value, patternStr)
			} else {
				byteIdx = strings.Index(str.Value, patternStr)
			}

			if byteIdx == -1 {
				return &object.Integer{Value: -1}
			}
			runeIdx := len([]rune(str.Value[:byteIdx]))
			return &object.Integer{Value: int64(runeIdx)}
		},
	})

	// 語法: string pad_str(string str, int width)
	// 說明: 計算終端機顯示寬度，將字串向右補空白直到滿足 width。
	// 範例: pad_str("攻擊", 10) -> "攻擊      "
	obj.Vars.Set("pad_str", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return &object.String{Value: ""}
			}
			str, ok1 := args[0].(*object.String)
			width, ok2 := args[1].(*object.Integer)
			if !ok1 || !ok2 {
				return &object.String{Value: ""}
			}

			s := str.Value
			targetWidth := int(width.Value)
			currentWidth := 0
			for _, r := range s {
				if r > 127 {
					currentWidth += 2
				} else {
					currentWidth += 1
				}
			}
			if currentWidth < targetWidth {
				s += strings.Repeat(" ", targetWidth-currentWidth)
			}
			return &object.String{Value: s}
		},
	})

	// 語法: string|int crypt(string str, [string seed])
	// 說明: 使用 bcrypt 進行密碼雜湊或驗證。
	// 範例: crypt("1234")
	obj.Vars.Set("crypt", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.String{Value: ""}
			}
			str, ok := args[0].(*object.String)
			if !ok {
				return &object.String{Value: ""}
			}

			if len(args) > 1 {
				if hash, isStr := args[1].(*object.String); isStr {
					err := bcrypt.CompareHashAndPassword([]byte(hash.Value), []byte(str.Value))
					if err == nil {
						return hash
					}
					return &object.Integer{Value: 0}
				}
			}
			hashBytes, err := bcrypt.GenerateFromPassword([]byte(str.Value), bcrypt.DefaultCost)
			if err != nil {
				return &object.String{Value: ""}
			}
			return &object.String{Value: string(hashBytes)}
		},
	})

	// 語法: void printf(string format, ...)
	// 說明: 格式化輸出到當前物件。
	// 範例: printf("等級: %d", level);
	obj.Vars.Set("printf", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			fmtStr, ok := args[0].(*object.String)
			if !ok {
				return &object.Nil{}
			}

			// 簡易處理 %O：在格式化前先將 Object 轉為 Inspect() 字串，並將 %O 替換為 %s
			format := fmtStr.Value
			var fmtArgs []interface{}
			argIdx := 1

			// 簡單處理格式化，不使用原始的 fmt.Sprintf，避免 %O 處理問題
			// 註：這是一個非常基礎的實作
			result := format
			for strings.Contains(result, "%O") && argIdx < len(args) {
				result = strings.Replace(result, "%O", "%s", 1)
				fmtArgs = append(fmtArgs, args[argIdx].Inspect())
				argIdx++
			}

			for _, arg := range args[argIdx:] {
				switch v := arg.(type) {
				case *object.Integer:
					fmtArgs = append(fmtArgs, v.Value)
				case *object.String:
					fmtArgs = append(fmtArgs, v.Value)
				default:
					fmtArgs = append(fmtArgs, arg.Inspect())
				}
			}

			msg := fmt.Sprintf(result, fmtArgs...)
			d.TellObject(obj, msg)
			return &object.Nil{}
		},
	})
}

// 語法: mixed *reg_assoc(string str, string *pat_arr, mixed *tok_arr, [mixed def])
// 說明: 使用正則表達式陣列分割字串，並將其關聯到對應的 Token。
// 範例: reg_assoc("a b c", ({"[a-z]"}), ({1}), 0);
func (d *Driver) registerAdvancedStringEfuns2(obj *object.LPCObject) {
	obj.Vars.Set("reg_assoc", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 3 {
				return &object.Array{Elements: []object.Object{}}
			}
			str, ok1 := args[0].(*object.String)
			patArr, ok2 := args[1].(*object.Array)
			tokArr, ok3 := args[2].(*object.Array)
			if !ok1 || !ok2 || !ok3 {
				return &object.Array{Elements: []object.Object{}}
			}

			// 簡化實作：MudOS 的 reg_assoc 非常複雜，這裡提供一個基礎相容介面
			// 僅支援單一模式匹配
			_ = tokArr // 標記為使用
			pattern := ""
			if len(patArr.Elements) > 0 {
				if p, ok := patArr.Elements[0].(*object.String); ok {
					pattern = p.Value
				}
			}

			re, err := regexp.Compile(pattern)
			if err != nil {
				return &object.Array{Elements: []object.Object{}}
			}

			matches := re.FindAllString(str.Value, -1)
			var res []object.Object
			for _, m := range matches {
				res = append(res, &object.String{Value: m})
			}
			return &object.Array{Elements: res}
		},
	})

	// 語法: string process_string(string str)
	// 說明: 處理字串中的 @@function:filename|arg@@ 格式並執行。
	// 範例: write(process_string("Current time: @@time@@"));
	obj.Vars.Set("process_string", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.String{Value: ""}
			}
			str, ok := args[0].(*object.String)
			if !ok {
				return &object.String{Value: ""}
			}

			re := regexp.MustCompile("@@([^@]+)@@")
			result := re.ReplaceAllStringFunc(str.Value, func(match string) string {
				content := re.FindStringSubmatch(match)[1]
				res := d.CallFunction(obj, content, nil)
				if s, ok := res.(*object.String); ok {
					return s.Value
				}
				return match
			})
			return &object.String{Value: result}
		},
	})

	// 語法: mixed process_value(string str)
	// 說明: 與 process_string 類似，但回傳物件而非字串。
	// 範例: mixed val = process_value("func");
	obj.Vars.Set("process_value", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.Nil{}
			}
			str, ok := args[0].(*object.String)
			if !ok {
				return &object.Nil{}
			}

			return d.CallFunction(obj, str.Value, nil)
		},
	})

	// 語法: int sscanf(string str, string fmtStr, mixed var1, ...)
	obj.Vars.Set("sscanf", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return &object.Integer{Value: 0}
			}
			str, ok1 := args[0].(*object.String)
			fmtStr, ok2 := args[1].(*object.String)
			if !ok1 || !ok2 {
				return &object.Integer{Value: 0}
			}

			// 🚀 強化版 sscanf：支援簡單的字串提取 (例如 "%s.c")
			// 我們使用正規表達式來模擬 sscanf 的行為
			pattern := fmtStr.Value
			// 將 %s 轉換為 ([^ ]+)，將 %d 轉換為 ([-+]?[0-9]+)
			// 注意：這只是簡易模擬
			pattern = regexp.QuoteMeta(pattern)
			pattern = strings.ReplaceAll(pattern, "\\%s", "(.+)")
			pattern = strings.ReplaceAll(pattern, "\\%d", "([-+]?[0-9]+)")
			pattern = strings.ReplaceAll(pattern, "\\%*s", ".+")
			pattern = strings.ReplaceAll(pattern, "\\%*d", "[0-9]+")
			
			re, err := regexp.Compile("^" + pattern + "$")
			if err != nil {
				return &object.Integer{Value: 0}
			}

			matches := re.FindStringSubmatch(str.Value)
			if matches == nil {
				return &object.Integer{Value: 0}
			}

			// 將匹配結果填回參數
			matchIdx := 1
			for i := 2; i < len(args); i++ {
				if matchIdx >= len(matches) {
					break
				}
				val := matches[matchIdx]
				
				switch target := args[i].(type) {
				case *object.String:
					target.Value = val
					matchIdx++
				case *object.Integer:
					if v, err := strconv.ParseInt(val, 10, 64); err == nil {
						target.Value = v
						matchIdx++
					}
				}
			}

			return &object.Integer{Value: int64(matchIdx - 1)}
		},
	})

	// 語法: string terminal_colour(string str, mapping m)
	obj.Vars.Set("terminal_colour", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 2 {
				return args[0]
			}
			str, ok1 := args[0].(*object.String)
			m, ok2 := args[1].(*object.Mapping)
			if !ok1 || !ok2 {
				return args[0]
			}

			// 🚀 關鍵最佳化：若字串不含 '$'，直接回傳，避免不必要的開銷
			if !strings.Contains(str.Value, "$") {
				return str
			}

			res := str.Value
			for _, pair := range m.Pairs {
				key, kOk := pair.Key.(*object.String)
				val, vOk := pair.Value.(*object.String)
				if kOk && vOk {
					res = strings.ReplaceAll(res, key.Value, val.Value)
				}
			}
			return &object.String{Value: res}
		},
	})

	// 語法: string md5(string str)
	// 說明: 計算 MD5 雜湊。
	obj.Vars.Set("md5", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.String{Value: ""}
			}
			strArg, ok := args[0].(*object.String)
			if !ok {
				return &object.String{Value: ""}
			}
			importMd5 := func(s string) string {
				// We can hash manually to avoid adding imports to header if it gets complex,
				// or we can use md5 from crypto/md5. Let's do it simply using standard golang way.
				// Since we might need import, let's verify if "crypto/md5" can be imported.
				// For safety without adding imports (which would require editing top of file),
				// let's do a simple checksum or see if we can edit imports.
				return fmt.Sprintf("%x", md5Hash(s))
			}
			return &object.String{Value: importMd5(strArg.Value)}
		},
	})
}

func md5Hash(s string) [16]byte {
	return md5.Sum([]byte(s))
}

