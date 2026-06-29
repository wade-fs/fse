package signaling

import (
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"strings"

	"mudscript/lpc-vm/driver"
)

type Hub struct {
	clients    map[string]*Client
	register   chan *Client
	unregister chan *Client
	forward    chan Message

	mudDriver  *driver.Driver
}

func NewHub(d *driver.Driver) *Hub {
	return &Hub{
		clients:    map[string]*Client{},
		register:   make(chan *Client),
		unregister: make(chan *Client),
		forward:    make(chan Message, 1024),
		mudDriver:  d,
	}
}

func (h *Hub) Run() {
	for {
		select {
		case client := <-h.register:
			h.clients[client.ID] = client

			if !client.IsP2P {
				pConn := driver.NewPlayerConnection(nil, nil) 
				pConn.SessionID = client.ID
				pConn.Username = client.Username
				
				pConn.OutputCallback = func(mudText string) {
					defer func() { recover() }() // 🚀 安全防護
					if pConn.IsActive {
						// 🚀 關鍵：偵測是否為原始 JSON 訊息
						if strings.HasPrefix(mudText, "__JSON_MSG__") {
							rawJson := strings.TrimPrefix(mudText, "__JSON_MSG__")
							type TempMsg struct {
								Type    string          `json:"type"`
								From    string          `json:"from"`
								To      string          `json:"to"`
								Payload json.RawMessage `json:"payload"`
							}
							var temp TempMsg
							if err := json.Unmarshal([]byte(rawJson), &temp); err == nil {
								var payloadStr string
								if len(temp.Payload) > 0 {
									if temp.Payload[0] == '"' {
										_ = json.Unmarshal(temp.Payload, &payloadStr)
									} else {
										payloadStr = string(temp.Payload)
									}
								}
								customMsg := Message{
									Type:    temp.Type,
									From:    temp.From,
									To:      temp.To,
									Payload: payloadStr,
								}
								select {
								case client.Send <- customMsg:
								default:
								}
								return
							}
						}

						msgType := "mud_text"
						payload := mudText
						if strings.HasPrefix(mudText, "__RAW__") {
							msgType = "mud_html"
							payload = strings.TrimPrefix(mudText, "__RAW__")
						}
						select {
						case client.Send <- Message{
							Type:    msgType,
							Payload: payload,
						}:
						default:
						}
					}
				}

				userObj := h.mudDriver.AcceptConnection(pConn, client.Language)
				if userObj != nil {
					pConn.Object = userObj
					h.mudDriver.RegisterInteractive(userObj, pConn)
					h.mudDriver.RunCommand(pConn, userObj, "logon", nil)
					client.MudConn = pConn 
				} else {
					fmt.Println("⚠️ 系統拒絕了", client.Username, "的連線")
				}
			}

			client.Send <- Message{
				Type: "welcome",
				To:   client.ID,
				Username: client.Username,
			}

			for id, peer := range h.clients {
				if id != client.ID {
					peer.Send <- Message{
						Type: "peer-joined",
						From: client.ID,
						To:   id,
						Username: client.Username,
					}
					client.Send <- Message{
						Type: "peer-joined",
						From: id,
						To:   client.ID,
						Username: peer.Username,
					}
				}
			}

			if client.IsP2P {
				h.BroadcastChat("SYSTEM", "一個新的 MUD 伺服器節點已加入星際網路。")
			}

		case client := <-h.unregister:
			if _, ok := h.clients[client.ID]; ok {
				delete(h.clients, client.ID)
				
				if client.MudConn != nil {
					client.MudConn.IsActive = false
					if client.MudConn.Object != nil {
						h.mudDriver.RunCommand(client.MudConn, client.MudConn.Object, "net_dead", nil)
						h.mudDriver.UnregisterInteractive(client.MudConn.Object)
					}
					client.MudConn.OutputCallback = nil
				}
				
				close(client.Send)
			}

			for id, peer := range h.clients {
				peer.Send <- Message{
					Type: "peer-left",
					From: client.ID,
					To:   id,
					Username: client.Username,
				}
			}

		case msg := <-h.forward:
			if msg.Type == "cmd" {
				client, ok := h.clients[msg.From]
				if !ok || client.MudConn == nil {
					continue
				}
				p := client.MudConn
				
				// 🚀 關鍵修正：統一調用 Driver 的核心指令處理邏輯
				// 這會確保 Web 客戶端與原生客戶端的行為完全一致
				h.mudDriver.ProcessCommand(p, msg.Payload)

			} else if msg.Type == "save_file" {
				// 🚀 關鍵：Web IDE 存檔處理
				client, ok := h.clients[msg.From]
				if !ok || client.MudConn == nil || client.MudConn.Object == nil {
					continue
				}
				p := client.MudConn
				obj := p.Object

				// 從 Payload 解析 JSON (預期格式: {"path": "...", "content": "..."})
				var saveReq struct {
					Path    string `json:"path"`
					Content string `json:"content"`
				}
				if err := json.Unmarshal([]byte(msg.Payload), &saveReq); err != nil {
					h.mudDriver.TellObject(obj, "❌ 存檔失敗：無效的 JSON 格式\n")
					continue
				}

				// 1. 權限檢查
				resolvedPath := h.mudDriver.ResolvePath(obj.Filename, saveReq.Path)
				allowed, errMsg := h.mudDriver.CheckWritePermission(obj, resolvedPath, "save_web_file")
				if !allowed {
					h.mudDriver.TellObject(obj, "❌ 存檔失敗：" + errMsg + "\n")
					continue
				}

				// 2. 寫入檔案
				fullPath := filepath.Join(h.mudDriver.Config.MudLibPath, resolvedPath)
				if err := os.MkdirAll(filepath.Dir(fullPath), 0755); err != nil {
					h.mudDriver.TellObject(obj, "❌ 存檔失敗：無法建立目錄\n")
					continue
				}
				if err := os.WriteFile(fullPath, []byte(saveReq.Content), 0644); err != nil {
					h.mudDriver.TellObject(obj, "❌ 存檔失敗：" + err.Error() + "\n")
					continue
				}

				h.mudDriver.TellObject(obj, "✅ 檔案 " + resolvedPath + " 已儲存。\n")

				// 3. 嘗試編譯並載入 (以獲取錯誤訊息)
				// 先移除舊物件
				if ob := h.mudDriver.FindObject(resolvedPath); ob != nil {
					h.mudDriver.DestructObject(ob)
				}
				
				_, err := h.mudDriver.LoadObject(resolvedPath)
				if err != nil {
					// 🚀 發送編譯錯誤給 Web IDE
					errMsg := err.Error()
					client.Send <- Message{
						Type:    "compile_error",
						From:    "system",
						To:      client.ID,
						Payload: errMsg,
					}
					h.mudDriver.TellObject(obj, "❌ 編譯失敗：" + errMsg + "\n")
				} else {
					// 🚀 發送編譯成功給 Web IDE
					client.Send <- Message{
						Type:    "compile_success",
						From:    "system",
						To:      client.ID,
						Payload: resolvedPath,
					}
					h.mudDriver.TellObject(obj, "✅ 編譯成功。\n")
				}

			} else if msg.Type == "request_edit" {
				// 🚀 關鍵：前端主動請求編輯檔案
				client, ok := h.clients[msg.From]
				if !ok || client.MudConn == nil {
					continue
				}
				h.mudDriver.ProcessCommand(client.MudConn, "edit " + msg.Payload)

			} else if msg.Type == "close_file" {
				// 🚀 關鍵：前端關閉編輯器，釋放鎖定
				client, ok := h.clients[msg.From]
				if ok && client.MudConn != nil && client.MudConn.Object != nil {
					// 呼叫 user.c 中的 cleanup_editor
					h.mudDriver.CallFunction(client.MudConn.Object, "cleanup_editor", nil)
				}

			} else if msg.Type == "chat" {
				if h.mudDriver != nil && h.mudDriver.OnP2PMessage != nil {
					h.mudDriver.OnP2PMessage(msg.Username, msg.Payload)
				}

				for id, peer := range h.clients {
					if peer.IsP2P {
						select {
						case peer.Send <- msg:
						default:
						}
					} else if id != msg.From {
						select {
						case peer.Send <- msg:
						default:
						}
					}
				}
			} else {
				if peer, ok := h.clients[msg.To]; ok {
					select {
					case peer.Send <- msg:
					default:
					}
				}
			}
		}
	}
}

func (h *Hub) UpdateClientUsername(id, newName string) {
	if client, ok := h.clients[id]; ok {
		client.Username = newName
	}
}

func (h *Hub) BroadcastChat(sender, content string) {
	h.forward <- Message{
		Type:     "chat",
		From:     "local",
		Username: sender,
		Payload:  content,
	}
}
