#include "dp.h"
#include "global.h"
#include <cstring>
#include <cassert>

using namespace global;
const int MAX_DISK = 1050;
int dp_use[MAX_DISK];
int back_track[MAX_DISK];
int state_p[MAX_DISK], state_m[MAX_DISK], state_pm[MAX_DISK];
vector<int> state_cpu[MAX_DISK], state_mem[MAX_DISK];

bool check_put(int state_id, int ins_line) {
    
}

vector<int> multi_knapsack_fill(int machine_id, map<int, vector<int> >& items) {
    memset(back_track, -1, sizeof back_track);
    dp_use[0] = 0;
    auto it = items.end();
    do {
        it--;
        memset(dp_use, -1, sizeof dp_use);
        int cnt = it->second.size();
        for(int i = 0; i < MAX_DISK; i++) {
            if(back_track[i] != -1) 
                dp_use[i] = cnt;
        }
        for(int i = 0; i < MAX_DISK; i++) {
            if(i + it->first >= MAX_DISK) break;
            if(dp_use[i] > 0 && dp_use[i + it->first] == -1) {
                dp_use[i + it->first] = dp_use[i] - 1;
                back_track[i + it->first] = it->first;
            }
        }
    } while(it != items.end());
    int o = MAX_DISK - 1;
    for(; o >= 0; o--) {
        if(back_track[o] >= 0) break;
    }
    assert(o > 0);
    vector<int> used_items;
    do {
        used_items.push_back(items[back_track[o]].back());
        items[back_track[o]].pop_back();
        o -= back_track[o];
    } while(o != 0);
    return used_items;
}

void multi_knapsack_solve() {
    map<int, int> result;
    map<int, vector<int> > items;
    for(int ins_line = 1;  ins_line < int(instance_ids.size()); ins_line++) {
        int app = instance_apps[ins_line];
        if(items.find(app_apply[app]) == items.end()) items[app_apply[app]] = vector<int> ();
        items[app_apply[app]].push_back(ins_line);
    }

    for(int machine_id = 1; machine_id < int(machine_ids.size()); machine_id++) {
        vector<int> tmp = multi_knapsack_fill(machine_id, items);
        for(int ins: tmp) {
            assert(result.find(ins) == result.end());
            result[ins] = machine_id;
        }
    }
    global:: final_output = result;
}
