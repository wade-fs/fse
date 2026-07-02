// /runtime/core/virtual_object.c
// FSE 核心通用虛擬物件基底 (Virtual Object Base)
// 提供虛擬路徑解析與資料驅動 (YAML) 的通用基礎

#include "/runtime/include/ansi.h"

// 提供虛擬路徑解析與資料驅動 (YAML) 的通用基礎

private string virtual_dir;
protected mapping virtual_config;


// 初始化虛擬物件 (解析路徑、讀取 yaml 等)
// prefix: 虛擬路徑的類型，例如 "nodes", "monsters"
// config_file: 該目錄下的設定檔名，例如 "node.yaml", "monster.yaml"
void setup_virtual(string prefix, string config_file) {
    string fname = file_name(this_object());
    string *parts = explode(fname, "/");
    
    if (sizeof(parts) >= 1) {
        string base_name = parts[sizeof(parts)-1];
        string yaml_path;
        
        // 去除可能的 #xxx clone 後綴
        if (strsrch(base_name, "#") != -1) {
            base_name = explode(base_name, "#")[0];
        }
        // 去除可能的 .c 後綴
        if (sscanf(base_name, "%s.c", base_name)) {}


        // 如果傳入了特定的 config_file (例如 node.yaml)，且檔案名稱等於 config_file (例如 /nodes/loop_swamp/node)
        // 則 ID 取上一層目錄名稱
        if (config_file != "" && base_name == explode(config_file, ".")[0] && sizeof(parts) >= 2) {
            string id = parts[sizeof(parts)-2];
            virtual_dir = "/content/" + prefix + "/" + id + "/";
            yaml_path = virtual_dir + config_file;
        } else {
            // 否則視為平坦結構，例如 /rooms/triassic_plains -> /content/rooms/triassic_plains.yaml
            string id = base_name;
            virtual_dir = "/content/" + prefix + "/";
            yaml_path = virtual_dir + id + ".yaml";
        }

        string yaml_content = read_file(yaml_path);
        if (yaml_content) {
            mixed err = catch(virtual_config = yaml_decode(yaml_content));
            if (err) {
                log_file("sys_error.log", sprintf("[%s] virtual_object yaml_decode error (%s): %s\n", ctime(time()), virtual_dir, err));
            }
        }
    }
}



// 透過外部注入的虛擬路徑進行初始化，避免 file_name(#clone) 導致的路徑丟失
void setup_virtual_by_path(string file_path, string prefix, string config_file) {
    if (sscanf(file_path, "%s.c", file_path)) {}
    string *parts = explode(file_path, "/");
    
    if (sizeof(parts) >= 1) {
        string base_name = parts[sizeof(parts)-1];
        string yaml_path;

        if (config_file != "" && base_name == explode(config_file, ".")[0] && sizeof(parts) >= 2) {
            string id = parts[sizeof(parts)-2];
            virtual_dir = "/content/" + prefix + "/" + id + "/";
            yaml_path = virtual_dir + config_file;
        } else {
            string id = base_name;
            virtual_dir = "/content/" + prefix + "/";
            yaml_path = virtual_dir + id + ".yaml";
        }

        if (getenv("MUD_TEST_MODE") || this_player()) {
            write(HIK "  [virtual_object trace] file_path=" + file_path + ", prefix=" + prefix + ", yaml_path=" + yaml_path + "\n" NOR);
        }

        string yaml_content = read_file(yaml_path);
        if (yaml_content) {
            mixed err = catch(virtual_config = yaml_decode(yaml_content));
            if (err) {
                log_file("sys_error.log", sprintf("[%s] virtual_object yaml_decode error (%s): %s\n", ctime(time()), virtual_dir, err));
            } else {
                if (getenv("MUD_TEST_MODE") || this_player()) {
                    write(HIG "  [virtual_object trace] yaml decoded successfully. node_id=" + virtual_config["node_id"] + "\n" NOR);
                }
            }
        } else {
            if (getenv("MUD_TEST_MODE") || this_player()) {
                write(HIR "  [virtual_object trace] 無法讀取 YAML 檔案: " + yaml_path + "\n" NOR);
            }
        }
    }
}

mapping query_virtual_config() { return virtual_config; }
string query_virtual_dir() { return virtual_dir; }
