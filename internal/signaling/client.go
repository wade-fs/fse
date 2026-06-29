package signaling

import (
	"encoding/json"
	"net/http"
	"strings"

	"github.com/google/uuid"
	"github.com/gorilla/websocket"
	"mudscript/lpc-vm/driver"
)

var upgrader = websocket.Upgrader{
	CheckOrigin: func(r *http.Request) bool {
		return true
	},
}

type Client struct {
	ID       string
	Username string
	Language string // 🚀 新增：瀏覽器語言
	IsP2P    bool   // 🚀 新增：標記是否為 P2P 節點
	Conn     *websocket.Conn
	Hub      *Hub
	Send     chan Message
	MudConn  *driver.PlayerConnection
}

func HandleWS(hub *Hub, w http.ResponseWriter, r *http.Request) {
	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		return
	}

	// 1. 從 URL 查詢參數中獲取資訊
	q := r.URL.Query()
	username := q.Get("username")
	if username == "" {
		username = "Anonymous"
	}
	isP2P := q.Get("p2p") == "true"
	token := q.Get("token")
	if token == "" {
		token = uuid.NewString()
	}

	// 🚀 新增：獲取瀏覽器語言 (Accept-Language)
	lang := r.Header.Get("Accept-Language")
	if lang != "" {
		// 簡化處理：取第一個主要語言標籤，例如 "zh-TW,zh;q=0.9" -> "zh-TW"
		if pos := strings.Index(lang, ","); pos != -1 {
			lang = lang[:pos]
		}
	}

	client := &Client{
		ID:       token, // 🚀 使用前端傳來的 token 作為唯一身分識別
		Username: username,
		Language: lang, 
		IsP2P:    isP2P, // 存入 Client
		Conn:     conn,
		Hub:      hub,
		Send:     make(chan Message, 256),
	}

	hub.register <- client

	go client.writeLoop()
	go client.readLoop()
}

func (c *Client) readLoop() {
	defer func() {
		c.Hub.unregister <- c
		c.Conn.Close()
	}()

	for {
		_, data, err := c.Conn.ReadMessage()
		if err != nil {
			return
		}

		var msg Message
		err = json.Unmarshal(data, &msg)
		if err != nil {
			continue
		}

		// 3. 伺服器安全性處理
		msg.From = c.ID
		// 🚀 關鍵修正：如果是 P2P 節點，我們信任它傳來的 Username (角色暱稱)
		// 如果是普通 Web 客戶端，則維持強制覆蓋，防止偽造身分
		if !c.IsP2P {
			msg.Username = c.Username
		}

		c.Hub.forward <- msg
	}
}

func (c *Client) writeLoop() {

	for msg := range c.Send {

		err := c.Conn.WriteJSON(msg)
		if err != nil {
			return
		}
	}
}
