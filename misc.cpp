#include "global.h"
#include "misc.h"
#include "read.h"
#include "code.h"
#include <iostream>
#include <fstream>
#include <map>
#include <cstring>

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
    auto it = output.begin();
    while(it != output.end()) {
        f << "inst_" << it->first << ", machine_" << it->second << endl;
        it++;
    }
    f.close();
}

void check_log(bool res, string section) {
    if(res) cerr << section << " test passed" << endl;
    else cerr << section << " test failed ! <-----------------" << endl;
}
double tmp_sum[1000000];
const double EPS = 1e-7;

void check_output(const map<int, int>& output) {
    cerr << "\n\n############## start check_output ###############\n" << endl;
    map<int, vector<int> > machine_alloc;
    for(auto it = output.begin(); it != output.end(); it++) {
        if(machine_alloc.find(it->second) == machine_alloc.end()) machine_alloc[it->second] = vector<int>();
        machine_alloc[it->second].push_back(it->first);
    }

    // check cpu limit
    bool flag = true, check_result = true;
    for(auto it = machine_alloc.begin(); it != machine_alloc.end(); it++) {
        const vector<int>& instances = it->second;
        assert(int(instances.size()) > 0);
        int length = app_cpu_line[instance_apps[instances[0]]].size();
        memset(tmp_sum, 0, sizeof(double)*(length + 3));
        for(auto ins: instances) {
            int app = instance_apps[ins];
            assert(length == int(app_cpu_line[app].size()));
            for(int i = 0; i < int(app_cpu_line[app].size()); i++) {
                tmp_sum[i] += app_cpu_line[app][i];
            }
        }
        for(int i = 0; i < length; i++) {
            if(tmp_sum[i] > cpu_spec[it->first] + EPS) {
                check_result = flag = false;
            }
        }
    }
    check_log(flag, "cpu limit");

    // check memory limit
    flag = true;
    for(auto it = machine_alloc.begin(); it != machine_alloc.end(); it++) {
        const vector<int>& instances = it->second;
        assert(int(instances.size()) > 0);
        int length = app_mem_line[instance_apps[instances[0]]].size();
        memset(tmp_sum, 0, sizeof(double)*(length + 3));
        for(auto ins: instances) {
            int app = instance_apps[ins];
            assert(length == int(app_mem_line[app].size()));
            for(int i = 0; i < int(app_mem_line[app].size()); i++) {
                tmp_sum[i] += app_mem_line[app][i];
            }
        }
        for(int i = 0; i < length; i++) {
            if(tmp_sum[i] > mem_spec[it->first] + EPS) {
                check_result = flag = false;
            }
        }
    }
    check_log(flag, "memory limit");

    // disk limit
    flag = true;
    for(auto it = machine_alloc.begin(); it != machine_alloc.end(); it++) {
        const vector<int>& instances = it->second;
        double disk_sum = 0;
        for(auto ins: instances) {
            int app = instance_apps[ins];
            disk_sum += app_apply[app];
        }
        if(disk_sum > disk_spec[it->first] + EPS) {
            check_result = flag = false;
        }
    }
    check_log(flag, "disk limit");

    // P M PM limit
    flag = true;
    for(auto it = machine_alloc.begin(); it != machine_alloc.end(); it++) {
        const vector<int>& instances = it->second;
        double P_sum = 0, M_sum = 0, PM_sum = 0;
        for(auto ins: instances) {
            int app = instance_apps[ins];
            P_sum += app_p[app];
            M_sum += app_m[app];
            PM_sum += app_pm[app];
        }
        if(P_sum > p_lim[it->first] + EPS) {
            check_result = flag = false;
        }
        if(M_sum > m_lim[it->first] + EPS) {
            check_result = flag = false;
        }
        if(PM_sum > pm_lim[it->first] + EPS) {
            check_result = flag = false;
        }
    }
    check_log(flag, "P_M_PM limit");

    // interference limit
    flag = true;
    for(auto it = machine_alloc.begin(); it != machine_alloc.end(); it++) {
        map<int, int> app_count;
        for(int ins: it->second) {
            int app = instance_apps[ins];
            if(app_count.find(app) == app_count.end())
                app_count[app] = 0;
            app_count[app]++;
        }
        for(auto it2 = app_count.begin(); it2 != app_count.end(); it2++) {
            for(const pair<int, int>& inter: app_inter_list[it2->first]) {
                if(app_count.count(inter.first) &&  app_count[inter.first] > inter.second ) {
                    check_result = flag = false;
                    
                    //*
                    cerr << "interference limit " << it2->first << " " << inter.first << " " << inter.second 
                         << " conflict with k = " << app_count[inter.first] << endl;
                    //*/
                    
                }
            }
        }
    }
    check_log(flag, "interference limit");

    // every instance allocated?
    flag = true;
    if(output.size() != int(instance_ids.size()) - 1) {
        check_result = flag = false;
        //cerr << output.size() << " " << instance_ids.size() - 1 << endl;
    }
    check_log(flag, "every instance allocated");

    // valid instance id and machine id?

    if(!check_result) {
        cerr << "check failed" << endl;
        return;
    }

    // compute score
}

void check_output_file(string output_file_name) {
    map<int,int> result ;
    read_output_file(output_file_name,result);
    check_output(result);
    Code c(machine_resources_num);
    for (auto t:result) {
        c.move(t.first,t.second);
    }
    //cout << "score: " << c.ave_score() << endl;
    c.show_status();
    c.show_extra_info();
}
