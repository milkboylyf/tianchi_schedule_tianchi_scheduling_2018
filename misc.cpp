#include "global.h"
#include "misc.h"
#include <iostream>
#include <map>

using namespace std;
using namespace global;


// map<instance line number, machine id/line number>
map<int, int> process_output(const map<int, int>& output) {
    map<int, int> result;
    auto it = output.begin();
    while(it != output.end()) {
        int real_id = instance_ids[it->first];
        result[real_id] = it->second;
        it++;
    }
    return result;
}

// map<instance id, machine id>
void write_output(const map<int, int>& output, string file_name) {
    ofstream f;
    f.open (file_name, ios::trunc);
    auto it = output.being();
    while(it != output.end()) {
        f << "inst_" << it->first << ", machine_" << it->second << endl;
        it++;
    }
    f.close();
    return 0;
}