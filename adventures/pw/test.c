void create() {
    write("Read Result: " + read_file("/runtime/locales/zh_TW.yaml"));
    shutdown(0);
}
