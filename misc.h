#include <iostream>
#include <map>
#include <string>
#include <cassert>

using namespace std;
using namespace global;

// map<instance line number, machine id/line number>
map<int, int> process_output(const map<int, int>& output);

// map<instance id, machine id>
void write_output(const map<int, int>& output, string file_name);

void check_log(bool res, string section) {
    if(res) cerr << section << " test succeeded" << endl;
    else cerr << section << " test failed !!" << endl;
}
double tmp_sum[1000000];
const double EPS = 1e-4;

void check_output(const map<int, int>& output) {
    cerr << "\n############## start check_output ###############\n" << endl;
    map<int, vector<int> > machine_alloc;
    for(auto it = output.begin(); it != output.end(); it++) {
        if(machine_alloc.find(it->second) == machine_alloc.end()) machine_alloc[it->second] = vector<int>();
        machine_alloc[it->second].push_back(it->first);
    }

    // check cpu limit
    bool flag = true;
    for(auto it = machine_alloc.begin(); it != machine_alloc.end(); it++) {
        const vector<int>& apps = it->second;
        assert(int(apps.size()) > 0);
        int length = app_cpu_line[apps[0]].size();
        memset(tmp_sum, 0, sizeof(double)*(length + 3));
        for(auto app: apps) {
            assert(length == int(app_cpu_line[app].size()));
            for(int i = 0; i < int(app_cpu_line[app].size()); i++) {
                tmp_sum[i] += app_cpu_line[app][i];
            }
        }
        for(int i = 0; i < length; i++) {
            if(tmp_sum[i] > cpu_spec[it->first] + EPS) {
                flag = false;
            }
        }
    }
    check_log(flag, "cpu limit");

    // check memory limit
    for(auto it = machine_alloc.begin(); it != machine_alloc.end(); it++) {
        const vector<int>& apps = it->second;
        assert(int(apps.size()) > 0);
        int length = app_mem_line[apps[0]].size();
        memset(tmp_sum, 0, sizeof(double)*(length + 3));
        for(auto app: apps) {
            assert(length == int(app_mem_line[app].size()));
            for(int i = 0; i < int(app_mem_line[app].size()); i++) {
                tmp_sum[i] += app_mem_line[app][i];
            }
        }
        for(int i = 0; i < length; i++) {
            if(tmp_sum[i] > mem_spec[it->first] + EPS) {
                flag = false;
            }
        }
    }
    check_log(flag, "memory limit");

    // disk limit
    for(auto it = machine_alloc.begin(); it != machine_alloc.end(); it++) {
        const vector<int>& apps = it->second;
        assert(int(apps.size()) > 0);
        double disk_sum = 0;
        for(auto app: apps) {
            disk_sum += app_apply[app];
        }
        if(disk_sum > disk_spec[it->first] + EPS) {
            flag = false;
        }
    }
    check_log(flag, "disk limit");

    // P M PM limit
    for(auto it = machine_alloc.begin(); it != machine_alloc.end(); it++) {
        const vector<int>& apps = it->second;
        assert(int(apps.size()) > 0);
        double P_sum = 0, M_sum = 0, PM_sum = 0;
        for(auto app: apps) {
            P_sum += app_p[app];
            M_sum += app_m[app];
            PM_sum += app_pm[app];
        }
        if(P_sum > p_lim[it->first] + EPS) {
            flag = false;
        }
        if(M_sum > m_lim[it->first] + EPS) {
            flag = false;
        }
        if(PM_sum > pm_lim[it->first] + EPS) {
            flag = false;
        }
    }
    check_log(flag, "P_M_PM limit");

    
}