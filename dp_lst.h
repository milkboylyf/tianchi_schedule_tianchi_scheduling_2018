#ifndef DYNAMIC_PROG
#define DYNAMIC_PROG

#include "code.h"
#include <algorithm>
#include <string.h>

struct InstanceData {
    int ins_id , app_id;
    bool is_deployed;
    bool operator< ( const InstanceData &t ) const ;
    InstanceData(int id, bool deployed);
};

struct DP :Code {
    vector<vector<InstanceData> > data;      //用来排序应用实例
    
    map<int,int> disk_index;       //磁盘规格下标 
    vector<int> sim_disk_spec;          //磁盘规格
    vector<int> ins_remain;         //每种规格剩余实例数
    vector<set<int> > disk_ins_set;    //每种规格的磁盘的实例 
    
    int f[2000];                //dp数组 
    int v[2000];
    int tr[2000];
    
    int disk_plan_num[20][ 2000 ];
    vector<set<int> > imposible_apps;
    
    double u_score;
    
    DP(int _len) ;
    
    void init() ;
    
    int dynamic_programming(int space) ;
    
    bool dfs_deploy ( int m, int left , int j);
    
    int dfs_disk_plans( int k, int space );

    void dp_plan() ;
    
    void package_up( int m, int max_space );
    
    int get_next_ins( Machine &m ) ;
};



#endif
