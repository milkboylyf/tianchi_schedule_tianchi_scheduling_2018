#include "global.h"

using namespace std;

namespace global {
int instance_deploy_num;
vector<int> instance_ids;
vector<int> instance_apps;
vector<int> instance_machines;

int app_resources_num;
vector<int> app_ids;
vector<vector<double> > app_cpu_line;
vector<vector<double> > app_mem_line;
vector<int> app_apply;
vector<int> app_p, app_m, app_pm;

int machine_resources_num;
vector<int> machine_ids;
vector<int> cpu_spec, mem_spec, disk_spec, p_lim, m_lim, pm_lim;

int app_interference_num;
vector<int> app_inter1, app_inter2, app_inter_max;

int time_len = 98 ;


vector<vector<int> > app_cpus;
vector<vector<int> > app_mems;
map<int,set<pair<int,int> > > app_inter_set; 

};

namespace cst {
    const double a = 10;
    const double b = 0.5;
};
