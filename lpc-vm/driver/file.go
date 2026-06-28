// driver/file.go
package driver

import (
	"fmt"
	"io/fs"
	"os"
	"path/filepath"
	"strings"
)

func (d *Driver) NormalizePath(path string) string {
	cleanPath := filepath.Clean(path)
	if cleanPath == "." || cleanPath == "/" {
		return ""
	}
	cleanPath = filepath.ToSlash(cleanPath)
	if !strings.HasPrefix(cleanPath, "/") {
		cleanPath = "/" + cleanPath
	}
	return cleanPath
}

// 🚀 新增：路徑解析 (支援 ./ 與 ../，以及跨服沙盒自動映射)
func (d *Driver) ResolvePath(basePath, relPath string) string {
	// 1. 處理相對路徑
	if strings.HasPrefix(relPath, ".") {
		dir := filepath.Dir(basePath)
		resolved := filepath.Join(dir, relPath)
		res := filepath.ToSlash(resolved)
		if !strings.HasPrefix(res, "/") {
			res = "/" + res
		}
		return res
	}

	// 2. 處理絕對路徑 (關鍵：跨服沙盒自動重導向)
	// 如果發起呼叫的物件 (basePath) 位於遠端緩存目錄中
	// 例如：/data/fs_cache/fantasy.space/area/newbie/room_0_0.c
	if strings.HasPrefix(basePath, "/data/fs_cache/") {
		// 提取 mudlib_id
		parts := strings.Split(strings.TrimPrefix(basePath, "/data/fs_cache/"), "/")
		if len(parts) > 0 {
			mudlibID := parts[0]
			sandboxPrefix := "/data/fs_cache/" + mudlibID

			// 如果目標路徑是以 / 開頭的絕對路徑，且不在「全域白名單」中
			if strings.HasPrefix(relPath, "/") {
				// 白名單：不需要重導向的目錄
				if !strings.HasPrefix(relPath, "/std/") &&
					!strings.HasPrefix(relPath, "/secure/") &&
					!strings.HasPrefix(relPath, "/include/") &&
					!strings.HasPrefix(relPath, "/cmds/") &&
					!strings.HasPrefix(relPath, "/data/fs_cache/") {

					// 自動加上沙盒前綴
					// 例如：/area/newbie/... -> /data/fs_cache/fantasy.space/area/newbie/...
					return sandboxPrefix + relPath
				}
			}
		}
	}

	// 3. 一般絕對路徑處理
	if !strings.HasPrefix(relPath, "/") {
		return "/" + relPath
	}
	return relPath
}

// 🚀 新增：混合模式讀取檔案 (支援 .c 尾碼自動修復)
func (d *Driver) ReadFile(filename string) ([]byte, error) {
	tryFiles := []string{filename}
	if !strings.HasSuffix(filename, ".c") {
		tryFiles = append(tryFiles, filename+".c")
	}

	for _, f := range tryFiles {
		relPath := strings.TrimPrefix(f, "/")

		// 1. 優先從實體磁碟讀取
		// 🚀 關鍵：確保路徑不會逃脫 MudLibPath (Virtual Chroot)
		fullPath := filepath.Join(d.Config.MudLibPath, relPath)
		cleanMudLib := filepath.Clean(d.Config.MudLibPath)
		cleanFull := filepath.Clean(fullPath)
		if !strings.HasPrefix(cleanFull, cleanMudLib) {
			return nil, fmt.Errorf("chroot violation: %s", f)
		}

		if _, err := os.Stat(fullPath); err == nil {
			return os.ReadFile(fullPath)
		}

		// 2. 備援從嵌入式系統讀取
		if d.Config.EmbeddedFS != nil {
			// 🚀 關鍵修正：根據配置的 MudLibPath 來尋找內嵌資源路徑
			// 例如 MudLibPath 為 "fsmud", 則 relPath 為 "master.c" -> embedPath 為 "fsmud/master.c"
			embedPath := filepath.Join(d.Config.MudLibPath, relPath)
			
			// 正規化內嵌路徑 (必須是正斜線且不帶開頭斜線)
			embedPath = filepath.ToSlash(embedPath)
			embedPath = strings.TrimPrefix(embedPath, "/")

			if content, err := fs.ReadFile(d.Config.EmbeddedFS, embedPath); err == nil {
				return content, nil
			}
		}
	}

	return nil, fmt.Errorf("file not found: %s", filename)
}

func (d *Driver) DiscoverMasterFile() string {
	configPath := filepath.Join(d.Config.MudLibPath, "include/config.h")
	content, err := os.ReadFile(configPath)
	if err != nil {
		return "/master.c"
	}
	lines := strings.Split(string(content), "\n")
	for _, line := range lines {
		if strings.Contains(line, "#define MASTER_FILE") {
			parts := strings.Fields(line)
			if len(parts) >= 3 {
				path := strings.Trim(parts[2], "\"")
				return path
			}
		}
	}
	return "/master.c"
}

func (d *Driver) formatParserErrors(filename string, errors []string) error {
	var sb strings.Builder
	sb.WriteString(fmt.Sprintf("❌ 語法錯誤 in %s\n\n", filename))
	for i, err := range errors {
		sb.WriteString(fmt.Sprintf("   %2d. %s\n", i+1, err))
	}
	return fmt.Errorf("%s", sb.String())
}
