package signaling

type Message struct {
	Type      string `json:"type"`
	From      string `json:"from"`
	To        string `json:"to"`
	RoomID    string `json:"room_id,omitempty"`
	Username  string `json:"username,omitempty"`
	SDP       string `json:"sdp,omitempty"`
	Candidate string `json:"candidate,omitempty"`
	Payload   string `json:"payload,omitempty"`
}
