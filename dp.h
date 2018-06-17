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

struct DP {
    vector<Machine> m_ins;          //服务器实例 
    vector<vector<InstanceData> > data;      //用来排序应用实例
    
    map<int,int> ins_pos;           //实例位置 
    map<int,int> disk_index;       //磁盘规格下标 
    vector<int> disk_spec;          //磁盘规格
    vector<int> ins_remain;         //每种规格剩余实例数
    map<int,set<int> > disk_ins_set;    //每种规格的磁盘的实例 
    
    int f[2000];                //dp数组 
    int v[2000];
    int tr[2000];
    
    double u_score;
    
    DP() ;
    
    void init() ;
    
    void dynamic_programming() ;
    
    void dp_plan() ;
    
    void package_up( int m );
};



#endif
