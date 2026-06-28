// driver/efun_network.go
package driver

import (
	"mudscript/lpc-vm/object"
)

func (d *Driver) registerNetworkEfuns(obj *object.LPCObject) {
	// 🚀 Stub 掉所有 MudOS Network Efuns 以避免啟動崩潰
	// 這些暫時不實作真正的 Socket 邏輯，僅回傳 0 或 Dummy 值

	stubs := []string{
		"socket_create", "socket_bind", "socket_listen", "socket_accept",
		"socket_write", "socket_close", "socket_release", "socket_address",
		"socket_status", "socket_connect",
	}

	for _, name := range stubs {
		obj.Vars.Set(name, &object.Builtin{
			Fn: func(args ...object.Object) object.Object {
				return &object.Integer{Value: 0}
			},
		})
	}
}
