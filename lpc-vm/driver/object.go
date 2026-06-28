// driver/object.go
package driver

import (
	"fmt"
	"log" // 🚀 改用 log 以確保輸出的即時性 (Unbuffered)
	"os"
	"strings"
	"time"

	"github.com/google/uuid"
	"mudscript/lpc-vm/ast"
	"mudscript/lpc-vm/evaluator"

	"mudscript/lpc-vm/lexer"
	"mudscript/lpc-vm/object"
	"mudscript/lpc-vm/parser"
	"mudscript/lpc-vm/preprocessor"
)

// LoadObject 載入並編譯一個 LPC 檔案成為藍圖物件 (含執行 create)
func (d *Driver) LoadObject(filename string) (*object.LPCObject, error) {
	obj, newlyLoaded, err := d.loadObjectInternal(filename)
	if err != nil {
		return nil, err
	}

	if newlyLoaded {
		d.CallFunction(obj, "create", nil)
	}
	return obj, nil
}

func (d *Driver) loadObjectInternal(filename string) (*object.LPCObject, bool, error) {
	filename = d.NormalizePath(filename)
	if filename == "" {
		return nil, false, fmt.Errorf("file not found: empty path")
	}

	// 🚀 關鍵強化：強制進行 .c 尾碼正規化
	if !strings.HasSuffix(filename, ".c") && !strings.Contains(filename, "#") {
		filename += ".c"
	}
	
	d.mu.RLock()
	if obj, exists := d.ObjectTable[filename]; exists {
		d.mu.RUnlock()
		return obj, false, nil
	}
	d.mu.RUnlock()


	// 使用混合模式讀取檔案內容
	content, err := d.ReadFile(filename)
	if err != nil {
		// 🚀 虛擬物件 (Virtual Objects) 支援
		// 只有當確定不是實體 .c 檔案時，才回呼 Master
		if d.MasterObject != nil && filename != d.Config.MasterFile {
			// 去除 .c 尾碼再詢問，因為虛擬路徑通常不帶 .c
			virtualPath := strings.TrimSuffix(filename, ".c")
			res := d.CallFunction(d.MasterObject, "compile_object", []object.Object{
				&object.String{Value: virtualPath},
			})
			if virtualObj, ok := res.(*object.LPCObject); ok && virtualObj != nil {
				virtualObj.Filename = filename
				d.mu.Lock()
				d.ObjectTable[filename] = virtualObj
				d.mu.Unlock()
				return virtualObj, true, nil
			}
		}
		return nil, false, err
	}

	pp := preprocessor.New(d.Config.MudLibPath)
	if d.GlobalMacros != nil {
		// Seed with global macros
		for k, v := range d.GlobalMacros {
			pp.Macros[k] = v
		}
	} else {
		pp.GlobalInclude = d.Config.GlobalInclude
	}

	// 🚀 新增：定義標準內建巨集
	pp.Macros["__FILE__"] = preprocessor.Macro{Name: "__FILE__", Body: "\"" + filename + "\""}
	dir := filename
	if idx := strings.LastIndex(filename, "/"); idx != -1 {
		dir = filename[:idx+1]
	} else {
		dir = "/"
	}
	pp.Macros["__DIR__"] = preprocessor.Macro{Name: "__DIR__", Body: "\"" + dir + "\""}

	pp.StripModifiers = d.Config.StripModifiers
	if d.Config.EmbeddedFS != nil {
		pp.SetEmbeddedFS(d.Config.EmbeddedFS)
	}
	processedContent, err := pp.Process(filename, string(content))
	if err != nil {
		return nil, false, fmt.Errorf("preprocessor error: %v", err)
	}

	l := lexer.New(processedContent)
	p := parser.New(l)
	program := p.ParseProgram()

	if len(p.Errors()) > 0 {
		for _, err := range p.Errors() {
			log.Printf("PARSER ERROR in %s: %s\n", filename, err)
		}
		debugPath := "debug_" + strings.ReplaceAll(strings.TrimPrefix(filename, "/"), "/", "_") + ".txt"
		os.WriteFile(debugPath, []byte(processedContent), 0644)
		return nil, false, d.formatParserErrors(filename, p.Errors())
	}


	env := object.NewEnvironment()
	lpcObj := &object.LPCObject{
		UUID:         uuid.New().String(),
		Filename:     filename,
		Vars:         env,
		Functions:    make(map[string]*object.Function),
		Inherits:     make([]*object.LPCObject, 0),
		LastActivity: time.Now().Unix(),
	}

	// 🚩 關鍵：先註冊到 ObjectTable 再初始化，防止循環繼承/移動导致的崩潰
	d.mu.Lock()
	d.ObjectTable[filename] = lpcObj
	d.UUIDTable[lpcObj.UUID] = lpcObj
	d.mu.Unlock()

	d.SetupEfuns(lpcObj)
	env.Set("__file__", &object.String{Value: filename})

	// 🚀 [修正] 兩階段初始化：第一階段先處理繼承、函式宣告與變數名稱 (Hoisting)
	// 這樣頂層變數初始化時才能正確參照到後面才定義的函式，且變數名稱已存在於環境中。
	for _, stmt := range program.Statements {
		if inheritStmt, ok := stmt.(*ast.InheritStatement); ok {
			parentFile := d.ResolvePath(filename, inheritStmt.Path)
			if !strings.HasSuffix(parentFile, ".c") {
				parentFile += ".c"
			}
			parentObj, parentNewlyLoaded, err := d.loadObjectInternal(parentFile)
			if err != nil {
				return nil, false, fmt.Errorf("無法繼承 %s: %v", parentFile, err)
			}

			if parentNewlyLoaded {
				d.CallFunction(parentObj, "create", nil)
			}

			lpcObj.Inherits = append(lpcObj.Inherits, parentObj)

			for k, v := range parentObj.Vars.GetAll() {
				if _, isBuiltin := v.(*object.Builtin); isBuiltin {
					continue
				}
				// 🚀 [修正] 不要繼承父物件的特殊系統變數
				if k == "__file__" || k == "this_object" || k == "__simul_efun_obj" {
					continue
				}

				var copiedVal object.Object
				if fn, ok := v.(*object.Function); ok {
					copiedVal = &object.Function{
						Parameters: fn.Parameters,
						Body:       fn.Body,
						Env:        env, // 👈 關鍵：指派為子物件的環境
						OriginFile: fn.OriginFile,
					}
				} else {
					copiedVal = deepCopyLPCValue(v)
				}
				env.Set(k, copiedVal)
			}
		} else if funcDef, ok := stmt.(*ast.FunctionDef); ok {
			// 提前註冊函式 (Hoisting)
			res := evaluator.Eval(funcDef, env)
			if _, ok := res.(*object.Function); ok {
				// fmt.Printf("DEBUG: Hoisting function %s in %s, origin %s\n", funcDef.Name.Value, filename, fn.OriginFile)
			}
		} else if varDecl, ok := stmt.(*ast.TypedVarDecl); ok {
			// 提前註冊變數名稱，初始值設為 Nil (Hoisting)
			if _, exists := env.Get(varDecl.Name.Value); !exists {
				env.Set(varDecl.Name.Value, &object.Nil{})
			}
		}
	}

	// 🚀 第二階段：執行所有頂層陳述式 (變數初始化等)
	res := evaluator.Eval(program, env)
	if errObj, ok := res.(*object.Error); ok {
		return nil, false, fmt.Errorf("evaluation error in %s: %s", filename, errObj.Message)
	}

	return lpcObj, true, nil
}

func (d *Driver) CloneObject(filename string) (*object.LPCObject, error) {
	blueprint, err := d.LoadObject(filename)
	if err != nil {
		return nil, err
	}

	clone := &object.LPCObject{
		UUID:         uuid.New().String(),
		Filename:     filename + "#" + generateCloneID(),
		Vars:         object.NewEnvironment(),
		Functions:    blueprint.Functions,
		Inherits:     blueprint.Inherits,
		LastActivity: time.Now().Unix(),
	}

	d.mu.Lock()
	d.ObjectTable[clone.Filename] = clone
	d.UUIDTable[clone.UUID] = clone
	d.mu.Unlock()

	d.SetupEfuns(clone)
	for k, v := range blueprint.Vars.GetAll() {
		if _, isBuiltin := v.(*object.Builtin); isBuiltin {
			continue
		}
		var copiedVal object.Object
		if fn, ok := v.(*object.Function); ok {
			copiedVal = &object.Function{
				Parameters: fn.Parameters,
				Body:       fn.Body,
				Env:        clone.Vars,
				OriginFile: fn.OriginFile,
			}
		} else {
			copiedVal = deepCopyLPCValue(v)
		}
		clone.Vars.Set(k, copiedVal)
	}

	dummyConn := &PlayerConnection{Object: clone}
	gid := getGID()
	oldContext, hasOld := d.playerContexts.Load(gid)
	d.playerContexts.Store(gid, dummyConn)
	defer func() {
		if hasOld {
			d.playerContexts.Store(gid, oldContext)
		} else {
			d.playerContexts.Delete(gid)
		}
	}()

	d.CallFunction(clone, "create", nil)
	return clone, nil
}

func (d *Driver) MoveObject(item *object.LPCObject, dest *object.LPCObject) {
	if item == nil {
		return
	}

	// 如果物件已銷毀，只允許移往 nil (即移出目前房間)
	if item.IsDestructed && dest != nil {
		return
	}

	if item.Location != nil {
		oldInv := item.Location.Inventory
		newInv := make([]*object.LPCObject, 0, len(oldInv))
		for _, obj := range oldInv {
			if obj != item {
				newInv = append(newInv, obj)
			}
		}
		item.Location.Inventory = newInv
	}

	item.Location = dest
	if dest != nil && !dest.IsDestructed {
		dest.Inventory = append(dest.Inventory, item)
	}

	// 已銷毀物件不觸發 init
	if item.IsDestructed {
		return
	}

	// 🚀 改良版 init 觸發機制 (接近 MudOS 標準)

	// 1. 如果移動的是生物，清除其 Actions (指令集) 並準備重建
	// 🚀 關鍵相容性修正：只清除來自環境或其他物件的指令，保留物件自身註冊的指令
	if item.IsLiving && item.Actions != nil {
		newActions := make(map[string]*object.Action)
		for verb, act := range item.Actions {
			if act.Provider == item {
				newActions[verb] = act
			}
		}
		item.Actions = newActions
	} else if item.IsLiving {
		item.Actions = make(map[string]*object.Action)
	}

	// A. 如果 item 是生物，它進入了一個新環境
	if item.IsLiving {
		pConn := d.getPlayerConnection(item)
		if pConn == nil {
			pConn = &PlayerConnection{Object: item, IsActive: true}
		}

		restore := d.setPlayerContext(pConn)
		// item->init() 在 dest 與 dest 內的所有物件
		if dest != nil {
			d.CallFunction(dest, "init", nil)
			// 注意：這裡要預先複製一份 inventory，避免 init 過程中 inventory 變動導致 iterator 崩潰
			inv := make([]*object.LPCObject, len(dest.Inventory))
			copy(inv, dest.Inventory)
			for _, obj := range inv {
				if obj != item && obj != nil && !obj.IsDestructed {
					d.CallFunction(obj, "init", nil)
				}
			}
		}
		// item 也要 init 自己身上的東西 (例如包包裡的物品要註冊指令給玩家)
		invSelf := make([]*object.LPCObject, len(item.Inventory))
		copy(invSelf, item.Inventory)
		for _, obj := range invSelf {
			if obj != nil && !obj.IsDestructed {
				d.CallFunction(obj, "init", nil)
			}
		}
		restore()
	}

	// B. 如果 dest 是生物 (例如物品進了玩家背包)
	if dest != nil && dest.IsLiving {
		pConn := d.getPlayerConnection(dest)
		if pConn == nil {
			pConn = &PlayerConnection{Object: dest, IsActive: true}
		}

		restore := d.setPlayerContext(pConn)
		d.CallFunction(item, "init", nil)
		restore()
	}

	// C. 如果 item 進入一個環境 dest，且環境內有其他生物 O (且 item 不是 O)
	if dest != nil {
		inv := make([]*object.LPCObject, len(dest.Inventory))
		copy(inv, dest.Inventory)
		for _, o := range inv {
			if o != nil && o != item && o.IsLiving && !o.IsDestructed {
				pConn := d.getPlayerConnection(o)
				if pConn == nil {
					pConn = &PlayerConnection{Object: o, IsActive: true}
				}

				restore := d.setPlayerContext(pConn)
				d.CallFunction(item, "init", nil)
				// 如果 item 也是生物，則 O 也要在 item 身上 init (互看)
				if item.IsLiving {
					d.CallFunction(o, "init", nil)
				}
				restore()
			}
		}
	}
}

func (d *Driver) DestructObject(obj *object.LPCObject) {
	if obj == nil || obj.IsDestructed {
		return
	}
	d.SetHeartBeat(obj, false)

	// 🚩 關鍵：先移出房間再標記銷毀，否則 MoveObject 會攔截導致移不出去
	if obj.Location != nil {
		d.MoveObject(obj, nil)
	}

	obj.IsDestructed = true

	var targetConn *PlayerConnection
	if conn, ok := d.interactiveObjects.Load(obj.Filename); ok {
		targetConn = conn.(*PlayerConnection)
	}
	if targetConn == nil {
		d.interactiveObjects.Range(func(key, value interface{}) bool {
			if pconn, ok := value.(*PlayerConnection); ok && pconn.Object == obj {
				targetConn = pconn
				return false
			}
			return true
		})
	}
	if targetConn != nil {
		targetConn.IsActive = false
		if targetConn.Conn != nil {
			targetConn.Conn.Close()
		}
		d.UnregisterInteractive(targetConn.Object)
	}
	for _, item := range obj.Inventory {
		d.MoveObject(item, obj.Location)
	}
	if obj.Location != nil {
		d.MoveObject(obj, nil)
	}
	d.mu.Lock()
	delete(d.ObjectTable, obj.Filename)
	d.mu.Unlock()
}
