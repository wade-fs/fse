void create() {
    string raw = read_file("/runtime/locales/zh_TW.yaml");
    mapping m = yaml_decode(raw);
    write("YAML: " + json_encode(m) + "\n");
    shutdown(0);
}
