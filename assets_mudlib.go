//go:build fsmud && !fs && !universal
// +build fsmud,!fs,!universal

package mudscript

import "embed"

// 🚀 專供給 fsmud 輕量版使用的資源
//go:embed all:adventures/fsmud/mudlib
var Assets embed.FS
