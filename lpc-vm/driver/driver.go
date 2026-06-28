// driver/driver.go
package driver

import (
	"fmt"
	"io/fs"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"sync"
	"time"

	"mudscript/lpc-vm/evaluator"
	"mudscript/lpc-vm/lexer"
	"mudscript/lpc-vm/object"
	"mudscript/lpc-vm/parser"
	"mudscript/lpc-vm/preprocessor"
)

// DriverConfig 運行時期的配置
type DriverConfig struct {
	MudLibPath      string
	MasterFile      string
	HeartBeatTick   time.Duration
	CleanUpInterval time.Duration // 🚀 新增：垃圾回收間隔
	EmbeddedFS      fs.FS
	EmbeddedPath    string // 🚀 新增：嵌入式路徑前綴
	GlobalInclude   string // 🚀 新增：全域自動引入標頭檔
	SimulEfunFile   string // 🚀 新增：指定 SimulEfun 檔案路徑
	StripModifiers  bool   // 🚀 新增：是否移除 static, varargs, nomask 等修飾詞 (用於相容 Legacy fs)
}

// Driver MUD 伺服器核心
type Driver struct {
	mu           sync.RWMutex
	ObjectTable  map[string]*object.LPCObject
	UUIDTable    map[string]*object.LPCObject // 🚀 新增：UUID 快速索引表
	Heartbeats   map[*object.LPCObject]bool
	CallOuts     []*ScheduledCall
	Config       DriverConfig
	GlobalMacros map[string]preprocessor.Macro // 🚀 新增：全域巨集快取
	shutdownCh   chan struct{}
	MasterObject *object.LPCObject
	SimulEfunObj *object.LPCObject // 🚀 新增：模擬 Efun 物件
	StartTime    time.Time
	RootUID      string
	BackboneUID  string

	// 使用 Goroutine ID 來追蹤當前正在執行的玩家
	playerContexts     sync.Map
	interactiveObjects sync.Map
	callStacks         sync.Map // 🚀 新增：goroutine ID -> []callFrame

	// 🚀 權限檢查遞迴防護
	inPermissionCheck sync.Map // goroutine ID -> bool

	// 🚀 P2P 整合
	OnP2PMessage     func(sender, content string)
	P2PSendChat      func(sender, content string)
	OnUsernameUpdate func(sid string, newName string)
}

func New(config DriverConfig) *Driver {
	os.MkdirAll(filepath.Join(config.MudLibPath, "data/user"), 0755)
	if config.HeartBeatTick == 0 {
		config.HeartBeatTick = 2 * time.Second
	}
	if config.CleanUpInterval == 0 {
		config.CleanUpInterval = 5 * time.Minute // 🚀 預設 5 分鐘
	}

	// 🚀 初始化全域內建函式表 (用於變數遮蔽時的 fallback)
	evaluator.RegisterBuiltin("time", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.Integer{Value: time.Now().Unix()}
		},
	})
	evaluator.RegisterBuiltin("ctime", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			if len(args) < 1 {
				return &object.String{Value: ""}
			}
			if t, ok := args[0].(*object.Integer); ok {
				return &object.String{Value: time.Unix(t.Value, 0).Format(time.ANSIC)}
			}
			return &object.String{Value: ""}
		},
	})

	// 🚀 標準巨集與環境變數 (標記為常數，自動展開)
	evaluator.RegisterBuiltin("__VERSION__", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.String{Value: "MudScript 1.0 (LPC Compatible)"}
		},
		IsConstant: true,
	})
	evaluator.RegisterBuiltin("__SAVE_EXTENSION__", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.String{Value: ".o"}
		},
		IsConstant: true,
	})
	evaluator.RegisterBuiltin("__ARCH__", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.String{Value: "Linux"}
		},
		IsConstant: true,
	})
	evaluator.RegisterBuiltin("MUD_NAME", &object.Builtin{
		Fn: func(args ...object.Object) object.Object {
			return &object.String{Value: "Fantasy Space"}
		},
		IsConstant: true,
	})

	return &Driver{
		ObjectTable: make(map[string]*object.LPCObject),
		UUIDTable:   make(map[string]*object.LPCObject),
		Heartbeats:  make(map[*object.LPCObject]bool),
		CallOuts:    make([]*ScheduledCall, 0),
		Config:      config,
		shutdownCh:  make(chan struct{}),
		StartTime:   time.Now(),
	}
}

// FindObject 尋找指定路徑的藍圖物件
func (d *Driver) FindObject(filename string) *object.LPCObject {
	filename = d.NormalizePath(filename)
	if !strings.HasSuffix(filename, ".c") && !strings.Contains(filename, "#") {
		filename += ".c"
	}

	d.mu.RLock()
	defer d.mu.RUnlock()
	return d.ObjectTable[filename]
}

func (d *Driver) Start() error {
	// 🚀 新增：預先載入全域標頭檔並註冊為環境變數
	if err := d.PreloadGlobalInclude(); err != nil {
		fmt.Printf("⚠️ 全域標頭檔預載入失敗: %v\n", err)
	}

	masterFile := d.Config.MasterFile
	if masterFile == "" {
		masterFile = d.DiscoverMasterFile()
	}

	fmt.Println("🚀 Driver 啟動中... 準備載入 Master Object:", masterFile)
	// 1. 只載入不執行 create
	master, _, err := d.loadObjectInternal(masterFile)
	if err != nil {
		return fmt.Errorf("致命錯誤: 無法載入 master.c: %v", err)
	}

	d.mu.Lock()
	d.MasterObject = master
	d.mu.Unlock()

	simulFile := d.Config.SimulEfunFile

	// 2. 如果 Config 沒指定，則從 Master 物件詢問 SimulEfun 路徑
	if simulFile == "" {
		if res := d.CallFunction(master, "get_simul_efun", nil); res != nil {
			if s, ok := res.(*object.String); ok && s.Value != "" {
				simulFile = s.Value
			}
		}
	}

	if simulFile != "" {
		simul, _, err := d.loadObjectInternal(simulFile)
		if err == nil {
			d.mu.Lock()
			d.SimulEfunObj = simul
			d.mu.Unlock()
			fmt.Printf("✅ SimulEfun 載入成功: %s\n", simulFile)

			// 注入 SimulEfuns 到已經載入的 master
			d.RegisterSimulEfuns(master)

			// 執行 SimulEfun 的 create
			d.CallFunction(simul, "create", nil)
		} else {
			fmt.Printf("⚠️ 無法載入 SimulEfun (%s): %v\n", simulFile, err)
		}
	}
	// 3. 取得 UIDs (在執行 create 之前)
	if res := d.CallFunction(master, "get_root_uid", nil); res != nil {
		if s, ok := res.(*object.String); ok {
			d.RootUID = s.Value
		}
	}
	if res := d.CallFunction(master, "get_bb_uid", nil); res != nil {
		if s, ok := res.(*object.String); ok {
			d.BackboneUID = s.Value
		}
	}

	// 🚀 新增：執行 Master 的 create
	d.CallFunction(master, "create", nil)

	fmt.Printf("✅ Master 準備就緒 (RootUID: %s, BackboneUID: %s)，開始執行初始化...\n", d.RootUID, d.BackboneUID)

	// 4. 回呼 Master 的 epilog 取得 preload 列表
	if res := d.CallFunction(master, "epilog", []object.Object{&object.Integer{Value: 0}}); res != nil {
		if arr, ok := res.(*object.Array); ok {
			for _, item := range arr.Elements {
				if s, ok := item.(*object.String); ok && s.Value != "" {
					d.CallFunction(master, "preload", []object.Object{s})
				}
			}
		}
	}

	go d.runGameLoop()    // 🚀 啟動遊戲主迴圈
	go d.runCleanUpLoop() // 🚀 啟動垃圾回收
	return nil
}

func (d *Driver) Stop() {
	close(d.shutdownCh)
}

func (d *Driver) PreloadGlobalInclude() error {
	if d.Config.GlobalInclude == "" {
		return nil
	}

	path := d.Config.GlobalInclude
	if !strings.HasPrefix(path, "/") {
		path = "/include/" + path
	}

	content, err := d.ReadFile(path)
	if err != nil {
		return err
	}

	pp := preprocessor.New(d.Config.MudLibPath)
	if d.Config.EmbeddedFS != nil {
		pp.SetEmbeddedFS(d.Config.EmbeddedFS)
	}

	macros := pp.ParseMacros(string(content))
	d.GlobalMacros = make(map[string]preprocessor.Macro)

	for name, m := range macros {
		d.GlobalMacros[name] = m
	}

	// fmt.Printf("✅ 全域標頭檔預載入完成: %s (共 %d 個巨集)\n", path, len(d.GlobalMacros))
	return nil
}

// EvalString 輔助函式：評估字串表達式
func (d *Driver) EvalString(input string) object.Object {
	// 簡單的常數優化
	if input == "" {
		return &object.Integer{Value: 0}
	}
	if strings.HasPrefix(input, "\"") && strings.HasSuffix(input, "\"") {
		return &object.String{Value: strings.Trim(input, "\"")}
	}
	if val, err := strconv.ParseInt(input, 10, 64); err == nil {
		return &object.Integer{Value: val}
	}

	// 完整評估
	l := lexer.New(input)
	p := parser.New(l)
	expr := p.ParseExpression(parser.LOWEST)
	if len(p.Errors()) > 0 {
		// 🚀 關鍵回退：若解析失敗 (例如不是合法的表達式)，回傳原始字串
		return &object.String{Value: input}
	}

	// 這裡使用空環境，但因為它會查找全域 builtins，所以依然能找到其他常數
	return evaluator.Eval(expr, object.NewEnvironment())
}
