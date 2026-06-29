// /runtime/core/virtual_object.c
// FSE 核心通用虛擬物件基底 (Virtual Object Base)
// 提供虛擬路徑解析與資料驅動 (YAML) 的通用基礎

inherit "/runtime/core/entity";

private string virtual_dir;
private mapping virtual_config;

// 初始化虛擬物件 (解析路徑、讀取 yaml 等)
// prefix: 虛擬路徑的類型，例如 "nodes", "monsters"
// config_file: 該目錄下的設定檔名，例如 "node.yaml", "monster.yaml"
void setup_virtual(string prefix, string config_file) {
    string fname = file_name(this_object());
    string *parts = explode(fname, "/");
    
    if (sizeof(parts) >= 2) {
        string id = parts[sizeof(parts)-2];
        virtual_dir = "/content/" + prefix + "/" + id + "/";
        
        string yaml_path = virtual_dir + config_file;
        string yaml_content = read_file(yaml_path);
        
        if (yaml_content) {
            mixed err = catch(virtual_config = yaml_decode(yaml_content));
            if (err) {
                log_file("sys_error.log", sprintf("[%s] virtual_object yaml_decode error (%s): %s\n", ctime(time()), virtual_dir, err));
            }
        }
    }
}

mapping query_virtual_config() { return virtual_config; }
string query_virtual_dir() { return virtual_dir; }
