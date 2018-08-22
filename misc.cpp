#include "global.h"
#include "misc.h"
#include "read.h"
#include "code.h"
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <cstring>
#include <utility>
#include <algorithm>
#include <queue>

#define NIN(k, m) (m.find(k) == m.end())

using namespace std;
using namespace global;

vector<int> ins_machine;
map<int, set<int> > machine_state, target_machine_state;
map<int, int> alloc_machine_app_count;
map<int, vector<int> > alloc_machine_cpu, alloc_machine_mem;
map<int, int> alloc_machine_p, alloc_machine_m, alloc_machine_pm, alloc_machine_disk;

vector<pair<int, int> > pair_output;

const int MAX_POOL_SIZE = 1e6;

int free_machines[MAX_POOL_SIZE];

int machines_queue[MAX_POOL_SIZE];
int q_back = 0, q_front = 0;

bool check_move(int ins_line, int machine_id) {
    int app = instance_apps[ins_line];
    if(NIN(machine_id, alloc_machine_cpu)) {
        alloc_machine_cpu[machine_id] = vector<int>(time_len, cpu_spec[machine_id]);
        alloc_machine_mem[machine_id] = vector<int>(time_len, mem_spec[machine_id]);
        alloc_machine_p[machine_id] = p_lim[machine_id];
        alloc_machine_m[machine_id] = m_lim[machine_id];
        alloc_machine_pm[machine_id] = pm_lim[machine_id];
        alloc_machine_disk[machine_id] = disk_spec[machine_id];
    }
    vector<int>& machine_cpu = alloc_machine_cpu[machine_id];
    vector<int>& machine_mem = alloc_machine_mem[machine_id];
    for(int i = 0; i < time_len; i++) {
        if(machine_cpu[i] < app_cpu_line[app][i]) return false;
        if(machine_mem[i] < app_mem_line[app][i]) return false;
    }
    if(alloc_machine_p[machine_id] < app_p[app]) return false;
    if(alloc_machine_m[machine_id] < app_m[app]) return false;
    if(alloc_machine_pm[machine_id] < app_pm[app]) return false;
    if(alloc_machine_disk[machine_id] < app_apply[app]) return false;
    return true;
}

void remove_ins(int ins_line, int machine_id) {
    int app = instance_apps[ins_line];
    ins_machine[ins_line] = -1;
    alloc_machine_app_count[app]--;
    machine_state[machine_id].erase(ins_line);
    assert(not NIN(machine_id, alloc_machine_cpu));
    vector<int>& machine_cpu = alloc_machine_cpu[machine_id];
    vector<int>& machine_mem = alloc_machine_mem[machine_id];
    for(int i = 0; i < time_len; i++) {
        machine_cpu[i] += app_cpu_line[app][i];
        machine_mem[i] += app_mem_line[app][i];
    }
    alloc_machine_p[machine_id] += app_p[app];
    alloc_machine_m[machine_id] += app_m[app];
    alloc_machine_pm[machine_id] += app_pm[app];
    alloc_machine_disk[machine_id] += app_apply[app];
}

void put_ins(int ins_line, int machine_id, bool write=true) {
    if(write) pair_output.push_back(make_pair(ins_line, machine_id));
    machine_state[machine_id].insert(ins_line);
    int app = instance_apps[ins_line];
    ins_machine[ins_line] = machine_id;
    alloc_machine_app_count[app]++;
    if(NIN(machine_id, alloc_machine_cpu)) {
        alloc_machine_cpu[machine_id] = vector<int>(time_len, cpu_spec[machine_id]);
        alloc_machine_mem[machine_id] = vector<int>(time_len, mem_spec[machine_id]);
        alloc_machine_p[machine_id] = p_lim[machine_id];
        alloc_machine_m[machine_id] = m_lim[machine_id];
        alloc_machine_pm[machine_id] = pm_lim[machine_id];
        alloc_machine_disk[machine_id] = disk_spec[machine_id];
    }
    vector<int>& machine_cpu = alloc_machine_cpu[machine_id];
    vector<int>& machine_mem = alloc_machine_mem[machine_id];
    for(int i = 0; i < time_len; i++) {
        machine_cpu[i] -= app_cpu_line[app][i];
        machine_mem[i] -= app_mem_line[app][i];
    }
    alloc_machine_p[machine_id] -= app_p[app];
    alloc_machine_m[machine_id] -= app_m[app];
    alloc_machine_pm[machine_id] -= app_pm[app];
    alloc_machine_disk[machine_id] -= app_apply[app];
}

bool tmp_alloc(int ins) {
    int tmp_machine = -1;
    for(int i = 1; i <= free_machines[0]; i++) {
        if(check_move(ins, free_machines[i])) {
            tmp_machine = free_machines[i];
        }
    }
    if(tmp_machine == -1) {
        for(int i = q_back - 1; i > q_front; i--) {
            if(check_move(ins, machines_queue[i])) {
                tmp_machine = machines_queue[i];
            }
        }
    }
    if(tmp_machine == -1) {
        cerr << "tmp_alloc free machine not found" << endl;
        exit(0);
    }
    put_ins(ins, tmp_machine);
    //cerr << "put 1" << endl;
    return true;
}

bool fill_machine(int machine_id) {
    queue<int> to_move_out, to_move_in;
    for(int ins: machine_state[machine_id]) {
        if(NIN(ins, target_machine_state[machine_id])) {
            to_move_out.push(ins);
        }
    }
    for(int ins: target_machine_state[machine_id]) {
        if(NIN(ins, machine_state[machine_id])) {
            to_move_in.push(ins);
        }
    }
    while(not to_move_out.empty()) {
        remove_ins(to_move_out.front(), machine_id);
        if(not tmp_alloc(to_move_out.front())) {
            cerr << "tmp_alloc failed" << endl;
            exit(0);
        }
        to_move_out.pop();
    }
    while(not to_move_in.empty()) {
        if(ins_machine[to_move_in.front()] != -1) 
            remove_ins(to_move_in.front(), ins_machine[to_move_in.front()]);
        put_ins(to_move_in.front(), machine_id);
        to_move_in.pop();
    }
    return true;
}


vector<pair<int, int> > process_output2(map<int, int>& output) {
    Code c(machine_resources_num), st(machine_resources_num);
    vector<pair<int,int> > answer; 
    //bool Machine::check_all_inter()
    int v[machine_resources_num+2] ={};
    for (int i=1;i<=instance_deploy_num;i++) {
        if (instance_machines[i]!=-1)
        {
            assert(c.move(i,instance_machines[i],1));
        }
        ;
        st.move(i,output[i]);
    }
    for (int i=1;i<=machine_resources_num ;i++)
        v[i] = !c.m_ins[i].check_all_inter();
    for (int i=machine_resources_num,j ;i>0;i--) {
        set<int> tmp = c.m_ins[i].ins_ids;
        //cout << i <<endl;
        for (auto t :tmp ) {
            if (!st.m_ins[i].ins_ids.count(t)) {
                for (j=1;j<i;j++) 
                    if (c.move(t,j))
                        break;
                assert(i-j);
                answer.push_back(make_pair(t,j));
                //cout << t << " " << j << endl;
            }
        }
        v[i] = 0;
        for (auto t : st.m_ins[i].ins_ids ) 
            if (instance_machines[t]!=-1&&!tmp.count(t))
        {
            assert(c.move(t,i));
            answer.push_back(make_pair(t,i));
                //cout << t << " " << i << endl;
        }
    }
    for (int i=machine_resources_num,j ;i>0;i--) {
        set<int> tmp = c.m_ins[i].ins_ids;
        for (auto t : st.m_ins[i].ins_ids ) 
            if (!tmp.count(t))
        {
            assert(c.move(t,i));
            answer.push_back(make_pair(t,i));
            cout << t << " " << i << endl;
        }
    }
    return answer;
}

// map<instance line number, machine id/line number>
vector<pair<int, int> > process_output(const map<int, int>& output) {
    for(auto it: output) {
        target_machine_state[it.second].insert(it.first);
    }
    cerr << "used machine num " << target_machine_state.size() << endl;
    ins_machine = instance_machines;
    for(int i = 1; i < int(instance_ids.size()); i++) {
        if(instance_machines[i] == -1) {

        }
        else {
            put_ins(i, instance_machines[i], false);
        }
    }
    for(int i = 1; i < int(machine_ids.size()); i++) {
        if(NIN(i, target_machine_state) && NIN(i, machine_state)) {
            free_machines[++free_machines[0]] = i;
        }
    }
    cerr << "free machine num " << free_machines[0] << endl;
    for(auto it: target_machine_state) {
        if(machine_state[it.first] == it.second) {

        }
        else {
            machines_queue[q_back++] = it.first;
        }
    }

    while(q_back - q_front > 0) {
        if(not fill_machine(machines_queue[q_front])) {
            cerr << "fill_machine failed" << endl;
            exit(0);
        }
        q_front++;
        cerr << "filled machine " << q_front << " " << q_back - q_front << " left" << endl;
    }

    return pair_output;
}

// map<instance id, machine id>
void write_output(const vector<pair<int, int> >& output, string file_name) {
    ofstream f;
    f.open (file_name, ios::trunc);
    for(auto& p: output) {
        f << "inst_" << instance_ids[p.first] << ",machine_" << p.second << endl;
    }
    f.close();
}

void write_output_origin(const map<int, int>& output, string file_name) {
    ofstream f;
    f.open (file_name, ios::trunc);
    for(auto& p: output) {
        f << "inst_" << instance_ids[p.first] << ",machine_" << p.second << endl;
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

    double score = 0;
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
            score += cst::a*exp( max(0.0,(double)tmp_sum[i]/cpu_spec[it->first] - cst::b )) - 9 ;
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
            //if (it2->first == 
            //cout << "%%%%" << it2->first <<endl;
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
    cout << "total_score: " << score/time_len <<endl;
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

void transform_pos( map<int, int> &pos , vector<int> & ins_mch ) {
    int result[100000];
    for (auto &t:pos) result[t.first] = t.second;
    Code c(machine_resources_num);
    for (int i=1;i<=instance_deploy_num;i++) if ( ins_mch[i] !=-1 ){
        c.move(i,ins_mch[i]);
    }
    for (int i=1;i<=instance_deploy_num;i++) {
        int ob = result[i];
        if (!c.m_ins[ob].ins_ids.count(i))
        for (int r : c.m_ins[ob].ins_ids) if (instance_apps[i] == instance_apps[r]) {
            if (result[r]!=ob) {
                int tmp = result[r];
                result[r] = ob;
                result[i] = tmp;
                break;
            }
        }
    }
    pos.clear();
    for (int i=1;i<=instance_deploy_num;i++) pos[i] = result[i];
}

