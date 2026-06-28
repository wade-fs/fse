// /fse/std/object.c
// FSE 核心最基本變數屬性容器
private mapping temp_data;
private mapping dbase;

void create() {
    temp_data = ([]);
    dbase = ([]);
}

void set(string prop, mixed val) { dbase[prop] = val; }
mixed query(string prop) { return dbase[prop]; }
void delete(string prop) { map_delete(dbase, prop); }

void set_temp(string prop, mixed val) { temp_data[prop] = val; }
mixed query_temp(string prop) { return temp_data[prop]; }
void delete_temp(string prop) { map_delete(temp_data, prop); }
