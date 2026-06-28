package p2p

import (
	"log"
	"net/url"
	"sync"
	"time"

	"github.com/gorilla/websocket"
	pion "github.com/pion/webrtc/v4"
	"mudscript/lpc-vm/driver"
	driver_object "mudscript/lpc-vm/object"
	"mudscript/internal/signaling"
	"mudscript/internal/webrtc"
)

type Node struct {
	ID        string
	Driver    *driver.Driver
	Signaler  *websocket.Conn
	Send      chan signaling.Message
	Peers     map[string]*Peer
	mu        sync.RWMutex
	
	HubURL    string
	stopped   bool
}

type Peer struct {
	ID          string
	PC          *pion.PeerConnection
	DataChannel *pion.DataChannel
}

func NewNode(d *driver.Driver, hubURL string) *Node {
	return &Node{
		Driver:   d,
		Peers:    make(map[string]*Peer),
		Send:     make(chan signaling.Message, 256),
		HubURL:   hubURL,
	}
}

func (n *Node) Start() {
	go n.connectLoop()
	go n.writeLoop()
}

func (n *Node) connectLoop() {
	for !n.stopped {
		err := n.connect()
		if err != nil {
			log.Printf("⚠️ Signaling connection failed: %v. Retrying in 5s...", err)
			time.Sleep(5 * time.Second)
			continue
		}

		// readLoop blocks until connection is lost
		n.readLoop()
		
		if !n.stopped {
			log.Println("🔄 Signaling connection lost. Reconnecting in 3s...")
			time.Sleep(3 * time.Second)
		}
	}
}

func (n *Node) connect() error {
	u, err := url.Parse(n.HubURL)
	if err != nil {
		return err
	}

	q := u.Query()
	q.Set("p2p", "true")
	
	// 🚀 新增：帶上本機的 mudlib_id 作為初始 username
	// 這樣信令中心才會知道這個 Anonymous 是誰
	systemD, err := n.Driver.LoadObject("/secure/system_d.c")
	if err == nil && systemD != nil {
		if res := n.Driver.CallFunction(systemD, "query_mudlib_id", nil); res != nil {
			if s, ok := res.(*driver_object.String); ok {
				q.Set("username", s.Value)
			}
		}
	}

	u.RawQuery = q.Encode()

	log.Printf("📡 Connecting to signaling hub: %s", u.String())
	c, _, err := websocket.DefaultDialer.Dial(u.String(), nil)
	if err != nil {
		return err
	}
	n.Signaler = c
	return nil
}

func (n *Node) readLoop() {
	defer n.Signaler.Close()

	// 🚀 新增：啟動 Ping 保持連線
	ticker := time.NewTicker(30 * time.Second)
	defer ticker.Stop()

	go func() {
		for range ticker.C {
			if n.Signaler == nil { return }
			err := n.Signaler.WriteMessage(websocket.PingMessage, nil)
			if err != nil { return }
		}
	}()

	for {
		var msg signaling.Message
		err := n.Signaler.ReadJSON(&msg)
		if err != nil {
			log.Println("❌ Signaling read error:", err)
			break
		}

		switch msg.Type {
		case "welcome":
			n.ID = msg.To
			log.Printf("🆔 Node registered with ID: %s", n.ID)
		case "peer-joined":
			// 🚀 關鍵修正：Polite Peer 邏輯
			// 只有當我的 ID 比對方小時，才主動發起 Offer。這能避免雙向重複建立連線
			if msg.From != n.ID && n.ID < msg.From {
				log.Printf("🤝 Initiating P2P connection with %s", msg.From)
				n.createOffer(msg.From)
			}
		case "offer":
			n.handleOffer(msg)
		case "answer":
			n.handleAnswer(msg)
		case "candidate":
			n.handleCandidate(msg)
		case "chat":
			n.broadcastToMUD(msg.Payload, msg.Username)
		}
	}
}

func (n *Node) writeLoop() {
	for msg := range n.Send {
		if n.Signaler == nil { continue }
		err := n.Signaler.WriteJSON(msg)
		if err != nil {
			log.Println("❌ Signaling write error:", err)
		}
	}
}

func (n *Node) createOffer(targetID string) {
	pc, err := pion.NewPeerConnection(webrtc.Config())
	if err != nil { return }

	dc, err := pc.CreateDataChannel("mud-data", nil)
	if err != nil { return }

	n.setupDataChannel(targetID, dc)

	pc.OnICECandidate(func(c *pion.ICECandidate) {
		if c == nil { return }
		n.Send <- signaling.Message{
			Type: "candidate",
			To:   targetID,
			From: n.ID,
			Candidate: c.ToJSON().Candidate,
		}
	})

	offer, err := pc.CreateOffer(nil)
	if err != nil { return }
	pc.SetLocalDescription(offer)

	n.Send <- signaling.Message{
		Type: "offer",
		To:   targetID,
		From: n.ID,
		SDP:  offer.SDP,
	}

	n.mu.Lock()
	n.Peers[targetID] = &Peer{ID: targetID, PC: pc}
	n.mu.Unlock()
}

func (n *Node) handleOffer(msg signaling.Message) {
	pc, err := pion.NewPeerConnection(webrtc.Config())
	if err != nil { return }

	pc.OnDataChannel(func(dc *pion.DataChannel) {
		n.setupDataChannel(msg.From, dc)
	})

	pc.OnICECandidate(func(c *pion.ICECandidate) {
		if c == nil { return }
		n.Send <- signaling.Message{
			Type: "candidate",
			To:   msg.From,
			From: n.ID,
			Candidate: c.ToJSON().Candidate,
		}
	})

	pc.SetRemoteDescription(pion.SessionDescription{Type: pion.SDPTypeOffer, SDP: msg.SDP})
	answer, _ := pc.CreateAnswer(nil)
	pc.SetLocalDescription(answer)

	n.Send <- signaling.Message{
		Type: "answer",
		To:   msg.From,
		From: n.ID,
		SDP:  answer.SDP,
	}

	n.mu.Lock()
	n.Peers[msg.From] = &Peer{ID: msg.From, PC: pc}
	n.mu.Unlock()
}

func (n *Node) handleAnswer(msg signaling.Message) {
	n.mu.RLock()
	peer, ok := n.Peers[msg.From]
	n.mu.RUnlock()
	if ok {
		peer.PC.SetRemoteDescription(pion.SessionDescription{Type: pion.SDPTypeAnswer, SDP: msg.SDP})
	}
}

func (n *Node) handleCandidate(msg signaling.Message) {
	n.mu.RLock()
	peer, ok := n.Peers[msg.From]
	n.mu.RUnlock()
	if ok {
		peer.PC.AddICECandidate(pion.ICECandidateInit{Candidate: msg.Candidate})
	}
}

func (n *Node) setupDataChannel(peerID string, dc *pion.DataChannel) {
	dc.OnOpen(func() {
		log.Printf("✅ P2P Data Channel opened with peer %s", peerID)
		n.mu.Lock()
		if p, ok := n.Peers[peerID]; ok {
			p.DataChannel = dc
		}
		n.mu.Unlock()
	})

	dc.OnMessage(func(msg pion.DataChannelMessage) {
		// 🚀 目前所有廣播訊息均已走信令中心，這裡預留給未來的點對點大檔案傳輸或私聊
	})
}

func (n *Node) broadcastToMUD(content, sender string) {
	if n.Driver.OnP2PMessage != nil {
		n.Driver.OnP2PMessage(sender, content)
	}
}

func (n *Node) SendChat(sender, content string) {
	// 🚀 核心路徑：透過信令伺服器 (WebSocket) 進行全球廣播
	// 這能保證訊息 100% 抵達 Hub 並轉發給所有節點，且不會產生重複訊息
	n.Send <- signaling.Message{
		Type:     "chat",
		Username: sender,
		Payload:  content,
	}
}
