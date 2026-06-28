// driver/connection.go
package driver

import (
	"net"
	"strings"

	"mudscript/lpc-vm/object"
)

type PlayerConnection struct {
	SessionID      string // 🚀 新增：WebSocket 工作階段 ID
	OutputRaw      bool   // 🚀 新增：標記當前發送是否為原始 HTML
	Conn           net.Conn
	Object         *object.LPCObject
	Username       string
	IsActive       bool
	History        []string
	MaxHist        int
	sendChan       chan string
	NextInputFunc    string
	NextInputClosure object.Object     // 🚀 新增：支援閉包回傳
	NextInputObj     *object.LPCObject // 🚀 新增：支援指定物件回傳
	NextInputArgs    []object.Object   // 🚀 新增：支援額外參數
	InputHidden      bool
	OutputCallback   func(msg string)
	CurrentVerb      string            // 🚀 新增：儲存當前執行的指令動詞
	NotifyFail       string            // 🚀 新增：指令執行失敗訊息
	SnoopedBy        *PlayerConnection // 🚀 新增：被誰監看 (Snoop)
}

func (p *PlayerConnection) Close() {
	if p.Conn != nil {
		p.Conn.Close()
	}
	p.IsActive = false
}

func NewPlayerConnection(conn net.Conn, obj *object.LPCObject) *PlayerConnection {
	p := &PlayerConnection{
		Conn:     conn,
		Object:   obj,
		IsActive: true,
		History:  make([]string, 0),
		MaxHist:  20,                     // 最多記錄 20 筆
		sendChan: make(chan string, 256), // 256 筆訊息的緩衝區
	}

	// 啟動專屬的「背景發送寫手」，避免阻塞主程式
	go p.writePump()
	return p
}

// 背景發送迴圈 (Write Pump)
func (p *PlayerConnection) writePump() {
	// 如果是傳統 TCP 連線，才需要 defer 關閉
	if p.Conn != nil {
		defer p.Conn.Close()
	}

	for msg := range p.sendChan {
		if !p.IsActive {
			break
		}

		// 👉 優先判定：如果有設定 Callback，就走 WebSocket 輸出
		if p.OutputCallback != nil {
			p.OutputCallback(msg)
			continue
		}

		// 否則，如果連線存在，走傳統 TCP 輸出
		if p.Conn != nil {
			// 如果是 TCP 連線，移除 __RAW__ 標記 (Telnet 不支援 HTML)
			cleanMsg := strings.TrimPrefix(msg, "__RAW__")
			_, err := p.Conn.Write([]byte(cleanMsg))
			if err != nil {
				p.IsActive = false
				break
			}
		}
	}
}

func (p *PlayerConnection) Send(msg string) {
	if !p.IsActive {
		return
	}

	// 🚀 Snoop 功能：如果有人正在監看此玩家，同步發送
	if p.SnoopedBy != nil && p.SnoopedBy.IsActive {
		p.SnoopedBy.Send("%" + msg) // Snoop 訊息通常加個前綴
	}

	finalMsg := msg
	if p.OutputRaw {
		finalMsg = "__RAW__" + msg
	}

	select {
	case p.sendChan <- finalMsg:
		// 成功放入緩衝區
	default:
		// 緩衝區滿了 (彈性丟棄)
	}
}

// 處理命令歷史與 ! 展開
func (p *PlayerConnection) ExpandHistory(input string) string {
	input = strings.TrimSpace(input)
	if input == "" {
		return ""
	}

	// 如果輸入 !!，展開為上一次的指令
	if input == "!!" {
		if len(p.History) == 0 {
			p.Send("沒有歷史指令可供重複。\r\n")
			return ""
		}
		cmd := p.History[len(p.History)-1]
		p.Send(cmd + "\r\n") // 顯示展開後的指令
		return cmd
	}

	if strings.HasPrefix(input, "!") {
		p.Send("目前僅支援 !! 重發上一個指令。\r\n")
		return ""
	}

	// 處理一般指令：加入歷史紀錄
	if len(p.History) == 0 || p.History[len(p.History)-1] != input {
		p.History = append(p.History, input)
		// 限制歷史紀錄長度
		if len(p.History) > p.MaxHist {
			p.History = p.History[1:]
		}
	}

	return input
}

// 註冊與取得互動玩家
func (d *Driver) RegisterInteractive(obj *object.LPCObject, conn *PlayerConnection) {
	if obj == nil || conn == nil {
		return
	}
	d.interactiveObjects.Store(obj.Filename, conn)
	obj.IsInteractive = true
}

func (d *Driver) UnregisterInteractive(obj *object.LPCObject) {
	if obj == nil {
		return
	}
	d.interactiveObjects.Delete(obj.Filename)
	obj.IsInteractive = false
}

// 🚀 新增：更新玩家顯示名稱到連線中
func (d *Driver) UpdatePlayerUsername(obj *object.LPCObject, name string) {
	if obj == nil {
		return
	}
	if val, ok := d.interactiveObjects.Load(obj.Filename); ok {
		if conn, ok := val.(*PlayerConnection); ok && conn != nil {
			conn.Username = name

			// 🚀 通知信令中心同步名稱
			if d.OnUsernameUpdate != nil {
				d.OnUsernameUpdate(conn.SessionID, name)
			}
		}
	}
}
func (d *Driver) GetConnectionFromObject(obj *object.LPCObject) *PlayerConnection {
	if obj == nil {
		return nil
	}
	if conn, ok := d.interactiveObjects.Load(obj.Filename); ok {
		return conn.(*PlayerConnection)
	}

	// 🚀 關鍵修正：當 Filename 對不上時 (例如 Clone 物件改名)，必須透過 Object 比對來尋找連線
	var found *PlayerConnection
	d.interactiveObjects.Range(func(key, value interface{}) bool {
		pconn := value.(*PlayerConnection)
		if pconn.Object == obj {
			found = pconn
			return false // stop iteration
		}
		return true
	})
	return found
}

// 🚀 新增：尋找物件對應的網路連線
func (d *Driver) getPlayerConnection(obj *object.LPCObject) *PlayerConnection {
	return d.GetConnectionFromObject(obj)
}
