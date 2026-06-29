// /std/object.c  (史前文明 PC)
// 最基本的屬性容器，與 runtime/core/object.c 同構
private mapping temp_data;
private mapping dbase;

void create() {
    temp_data = ([]);
    dbase = ([]);
}

void set(string prop, mixed val)   { dbase[prop] = val; }
mixed query(string prop)           { return dbase[prop]; }
void delete(string prop)           { map_delete(dbase, prop); }

void set_temp(string prop, mixed val) { temp_data[prop] = val; }
mixed query_temp(string prop)         { return temp_data[prop]; }
void delete_temp(string prop)         { map_delete(temp_data, prop); }
