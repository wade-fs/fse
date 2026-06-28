//go:build !fsmud && !fs && !universal
// +build !fsmud,!fs,!universal

package mudscript

import "embed"

// 🚀 純淨驅動版：不內嵌任何資源，強制讀取磁碟檔案
var Assets embed.FS
