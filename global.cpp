#include "global.h"

using namespace std;

namespace global {
int instance_deploy_num;
vector<int> instance_ids = {-1};
vector<int> instance_apps = {-1};
vector<int> instance_machines = {-1};

int app_resources_num;
vector<int> app_ids = {-1};
vector<vector<double> > app_cpu_line = {{}};
vector<vector<double> > app_mem_line = {{}};
vector<int> app_apply = {-1};
vector<int> app_p = {-1}, app_m = {-1}, app_pm = {-1};

int machine_resources_num;
vector<int> machine_ids = {-1};
vector<int> cpu_spec = {-1}, mem_spec = {-1}, disk_spec = {-1}, p_lim = {-1}, m_lim = {-1}, pm_lim = {-1};

int app_interference_num;
vector<int> app_inter1, app_inter2, app_inter_max;
map<int, vector<pair<int, int> > > app_inter_list;

int time_len = 98 ;


vector<vector<int> > app_cpus= {{}};
vector<vector<int> > app_mems= {{}};
vector<int> app_ins_num ;

map<int,set<pair<int,int> > > app_inter_set;
map<int,set<pair<int,int> > > app_rvs_inter_set; 
map<int,map<int,int> > app_inter_map; 
map<int,int> app_inter_counter;
vector<int> self_inter_num={-1};

map<int, int> final_output;
};

namespace cst {
    const double a = 10;
    const double b = 0.5;
};
