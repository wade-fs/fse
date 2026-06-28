package webrtc

import pion "github.com/pion/webrtc/v4"

func Config() pion.Configuration {

	return pion.Configuration{
		ICEServers: []pion.ICEServer{
			{
				URLs: []string{
					"stun:stun.l.google.com:19302",
				},
			},
			{
				URLs: []string{
					"turn:turn.example.com:3478",
				},
				Username:   "user",
				Credential: "pass",
			},
		},
	}
}
