/*
 * global variables
 */


#pragma once

#include <vector>
#include <string>
#include <map>
#include <set>

using namespace std;

namespace global {
extern int instance_deploy_num;
extern vector<int> instance_ids;
extern vector<int> instance_apps;
extern vector<int> instance_machines;

extern int app_resources_num;
extern vector<int> app_ids;
extern vector<vector<double> > app_cpu_line;
extern vector<vector<double> > app_mem_line;
extern vector<int> app_apply;
extern vector<int> app_p, app_m, app_pm;


extern int machine_resources_num;
extern vector<int> machine_ids;
extern vector<int> cpu_spec, mem_spec, disk_spec, p_lim, m_lim, pm_lim;

extern int app_interference_num;
extern vector<int> app_inter1, app_inter2, app_inter_max;
extern map<int, vector<pair<int, int> > > app_inter_list;
extern int time_len;


extern map<int,int> instance_index ; 
//extern vector<vector<int> > app_cpu_line;
//extern vector<vector<int> > app_mem_line;
extern vector<double> app_max_cpu;
extern vector<int> app_ins_num;
extern vector<double> sum_cpu_line;
extern vector<double> sum_mem_line;

extern map<int,set<pair<int,int> > > app_inter_set; 
extern map<int,set<pair<int,int> > > app_rvs_inter_set;
extern map<int,map<int,int> > app_inter_map; 
extern map<int,int> app_inter_counter;
extern vector<int> self_inter_num;

extern map<int, int> final_output;
};

namespace cst {
extern const double a ;
extern const double b ;
};
