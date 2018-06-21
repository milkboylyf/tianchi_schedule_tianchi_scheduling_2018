#ifndef ORDER_DEPLOYER
#define ORDER_DEPLOYER

#include "code.h"
#include <algorithm>
#include <string.h>

struct AppData {
    int app_id;
    int a[10];
    AppData(int id);
    bool operator< ( const AppData &t ) const ;
};

struct Deployer:Code {
    
    vector<AppData> app_view;       //app视图，用于排序app 
    
    
    map<int,int> disk_index;       //磁盘规格下标 
    vector<int> sim_disk_spec;          //磁盘规格
    vector<int> ins_remain;         //每种规格剩余实例数
    vector<set<int> > disk_ins_set;    //每种规格的磁盘的实例 
    
    vector<set<int> > app_ins_set;
    
    int f[2000];                //dp数组 
    
    map<int,int> imposible_apps;
    
    double u_score;
    
    void make_view (Machine &m); 
    
    bool able_to_deploy( int app ) ;
    
    Deployer(int _len) ;
    
    void update_imposible_apps( int t, int max_app);
    
    void init() ;
    
    int dynamic_programming(int space) ;
    
    void package_up();
    
    int get_next_ins( Machine &m ) ;
    
    bool set_ins( int ins, int m ,bool constant=false);
};



#endif
