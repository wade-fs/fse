package webrtc

import pion "github.com/pion/webrtc/v4"

type Peer struct {
	ID   string
	Conn *pion.PeerConnection
}
