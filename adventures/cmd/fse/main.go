// cmd/webmud/main.go
package main

import (
	"flag"
	"log"
	"net/http"
	"os"
	"strings"
	"time"

	"mudscript/lpc-vm/driver"
	"mudscript/internal/p2p"
	"mudscript/internal/signaling"
	"mudscript/lpc-vm/object"
	"gopkg.in/yaml.v3"
)

func main() {
	hubURL := flag.String("hub", "wss://wade-fs-fsmud-hub.hf.space/ws", "Signaling hub URL (set to 'none' to run in isolation)")
	port := flag.String("port", "8080", "HTTP server port")
	mudlib := flag.String("mudlib", "mudlib", "mudlib directory")
	dataPath := flag.String("data", "", "External data directory path (auto-resolved if empty)")
	runtimePath := flag.String("runtime", "../../runtime", "Core runtime directory path")
	master := flag.String("master", "", "Master file path (auto-resolved if empty)")
	simul := flag.String("simul", "", "SimulEfun file path (optional, master's get_simul_efun takes precedence)")
	gInclude := flag.String("ginclude", "", "Global include file (e.g. globals.h)")
	legacy := flag.Bool("legacy", false, "Enable legacy mode (strip modifiers, etc.)")
	flag.Parse()

	log.Printf("正在初始化 WebMUD 伺服器 (%s 模式)...", *mudlib)

	// 嘗試從 mudlib 目錄讀取並解析 manifest.yaml 以實現零參數自適應
	mFile := *mudlib + "/manifest.yaml"
	autoDataPath := *dataPath
	autoMasterFile := *master

	if _, err := os.Stat(mFile); err == nil {
		log.Printf("🔍 [Manifest System] 檢測到宣告式配置: %s", mFile)
		raw, err := os.ReadFile(mFile)
		if err == nil {
			var manifest map[string]interface{}
			if err := yaml.Unmarshal(raw, &manifest); err == nil {
				// 自動設定 adventure_id 對應的 data 存檔目錄
				if advID, ok := manifest["adventure_id"].(string); ok && autoDataPath == "" {
					autoDataPath = "../../data/" + advID
					log.Printf("⚙️  [Manifest System] 自動推導數據存檔路徑 (DataPath): %s", autoDataPath)
				}
				// 預設 master.c 為主引導檔
				if autoMasterFile == "" {
					autoMasterFile = "master.c"
					log.Printf("⚙️  [Manifest System] 自動對齊引導檔案 (MasterFile): %s", autoMasterFile)
				}
			}
		}
	}

	// 若未有 manifest 且無手動指定，則套用 Fallback 預設值
	if autoDataPath == "" {
		autoDataPath = "data"
	}
	if autoMasterFile == "" {
		autoMasterFile = "master.c"
	}

	// 1. 準備 MUD 引擎配置
	config := driver.DriverConfig{
		MudLibPath:      *mudlib,
		DataPath:        autoDataPath,
		RuntimePath:     *runtimePath,
		MasterFile:      autoMasterFile,
		SimulEfunFile:   *simul,
		GlobalInclude:   *gInclude,
		StripModifiers:  *legacy,
		HeartBeatTick:   0,               // 預設關閉，由各物件自行開啟
		CleanUpInterval: 5 * time.Minute, // 5分鐘執行一次 GC
	}

	// 針對 legacy 模式的特別調整
	if *legacy {
		config.HeartBeatTick = 2 * time.Second
		config.CleanUpInterval = 15 * time.Minute
		if config.SimulEfunFile == "" {
			config.SimulEfunFile = "/adm/obj/simul_efun.c"
		}
		if config.GlobalInclude == "" {
			config.GlobalInclude = "globals.h"
		}
	}

	// 2. 初始化 MUD 腳本引擎
	d := driver.New(config)

	if err := d.Start(); err != nil {
		log.Fatalf("致命錯誤: %v", err)
	}
	log.Println("MUD 引擎啟動成功！")

	// 4. 初始化 WebSocket 與 P2P 信令中心
	hub := signaling.NewHub(d)
	go hub.Run()

	// 同步玩家名稱至信令中心
	d.OnUsernameUpdate = func(sid, newName string) {
		hub.UpdateClientUsername(sid, newName)
	}

	// 5. P2P 整合核心
	setupP2P(d, hub, *hubURL)

	// 6. 設定 HTTP 與 WebSocket 路由
	http.HandleFunc("/ws", func(w http.ResponseWriter, r *http.Request) {
		signaling.HandleWS(hub, w, r)
	})

	// 混合模式網頁服務
	setupStaticServer(*mudlib)

	// 7. 啟動 Web 伺服器
	log.Printf("Web 伺服器監聽中 :%s\n", *port)
	err := http.ListenAndServe(":"+*port, nil)
	if err != nil {
		panic(err)
	}
}

func setupP2P(d *driver.Driver, hub *signaling.Hub, hubURL string) {
	// A. 連結 P2P -> MUD (接收訊息)
	d.OnP2PMessage = func(sender, content string) {
		log.Printf("🌌 [P2P] 收到來自 %s 的訊息: %s", sender, content)
		interstellar, err := d.LoadObject("/secure/interstellar_d.c")
		if err == nil && interstellar != nil {
			msgType := "chat"
			if strings.HasPrefix(sender, "SYSTEM") {
				msgType = "system"
			}
			d.CallFunction(interstellar, "receive_p2p_message", []object.Object{
				&object.String{Value: sender},
				&object.String{Value: content},
				&object.String{Value: msgType},
			})
		}
	}

	// B. 連結 MUD -> P2P (發送訊息)
	isRemoteHub := hubURL != "" && hubURL != "none"

	if os.Getenv("SPACE_ID") != "" && strings.Contains(hubURL, os.Getenv("SPACE_ID")) {
		log.Println("ℹ️ 偵測到於雲端 Hub 執行，略過自我 P2P 連線以避免重複訊息。")
		isRemoteHub = false
	}

	if isRemoteHub {
		node := p2p.NewNode(d, hubURL)
		d.P2PSendChat = func(sender, content string) {
			node.SendChat(sender, content)
		}
		node.Start()
		log.Println("🚀 P2P 節點已啟動，連接至:", hubURL)
	} else {
		d.P2PSendChat = func(sender, content string) {
			hub.BroadcastChat(sender, content)
		}
	}
}

func setupStaticServer(mudlib string) {
	diskPath := "./" + mudlib + "/web/static"

	if info, err := os.Stat(diskPath); err == nil && info.IsDir() {
		log.Printf("🌐 [WEB] 使用外部磁碟網頁檔案: %s\n", diskPath)
		http.Handle("/", http.FileServer(http.Dir(diskPath)))
	} else {
		log.Fatalf("❌ 錯誤: 找不到外部磁碟網頁檔案目錄: %s\n", diskPath)
	}
}
