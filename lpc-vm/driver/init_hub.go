package driver

import (
	"log"
	"os"
	"path/filepath"
)

// InitializeHubData 建立作為 Hub 時所需的預設資料檔
func (d *Driver) InitializeHubData() {
	log.Println("🌟 偵測到 Hub 模式，正在檢查並建立預設資料檔...")

	// 1. 建立 system.o (原為 server.o，但 user 要求 system.o 內容為 server.o 格式)
	systemPath := filepath.Join(d.Config.MudLibPath, "data/system.o")
	if _, err := os.Stat(systemPath); os.IsNotExist(err) {
		// 🚀 關鍵修正：確保 data/ 目錄存在
		os.MkdirAll(filepath.Dir(systemPath), 0755)
		
		content := `{"mudlib_id":"fantasy.space","mudlib_name":"狂想空間II"}`
		if err := os.WriteFile(systemPath, []byte(content), 0644); err != nil {
			log.Printf("❌ 無法建立 %s: %v", systemPath, err)
		} else {
			log.Printf("✅ 已建立預設的 %s", systemPath)
		}
	}

	// 2. 建立 wade.o
	wadePath := filepath.Join(d.Config.MudLibPath, "data/user/wade.o")
	if _, err := os.Stat(wadePath); os.IsNotExist(err) {
		// 確保目錄存在
		os.MkdirAll(filepath.Dir(wadePath), 0755)
		
		content := `{
  "aliases": {
    "d": "go down",
    "e": "go east",
    "east": "go east",
    "i": "inventory",
    "l": "look",
    "n": "go north",
    "north": "go north",
    "s": "go south",
    "sc": "score",
    "south": "go south",
    "u": "go up",
    "w": "go west",
    "west": "go west"
  },
  "can_open": 0,
  "id_list": [ "wade" ],
  "lang": "zh-TW",
  "long_name": "創世神。菠蘿麵包",
  "name": "菠蘿麵包",
  "password": "$2a$10$fxjmas/Ds.eM5QBqiGufGeHbf9BuAnmSDrNEGCweAkYfqltobKCHG",
  "role": "god",
  "short_name": "wade",
  "write_paths": [ "/" ]
}`
		if err := os.WriteFile(wadePath, []byte(content), 0644); err != nil {
			log.Printf("❌ 無法建立 %s: %v", wadePath, err)
		} else {
			log.Printf("✅ 已建立預設的 %s", wadePath)
		}
	}
}
