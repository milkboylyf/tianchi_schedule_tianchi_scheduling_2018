#ifndef _CODE
#define _CODE


#include "global.h"
#include <stack>
#include <cmath>
#include <iostream>
#include <assert.h>

using namespace std;
using namespace global;


struct Machine {
    
    int m_ids;
    
    set<int> ins_ids;
    vector<double> cpu;
    vector<double> mem;
    map<int,int> apps; 
    int disk;
    int P;
    int M;
    int PM;
    
    set<int> constant;
    
    Machine(int ids);
    
    bool empty(); 
    
    void clear();
    
    bool check_cpu_overload( int ins );
    
    double score();
    
    void set_constant( int ins );
    
    bool spec_eval( int ins );
    
    bool inter_eval( int ins_app );
    
    bool add_instance(int ins);
    
    bool del_instance( int ins );
    
    void print() ;
};

struct Code {
    set<int> running;
    set<int> stop;
    vector<Machine> m_ins;
    int len;
    
    map<int,int> disk_index;       //磁盘规格下标 
    vector<int> sim_disk_spec;          //磁盘规格
    vector<int> ins_remain;         //每种规格剩余实例数
    
    double u_score;
    map<int,int> ins_pos;
    
    Code(int _len);
    
    stack<pair<int,int> > move_log;
    
    int get_level( int ins );
    
    void reset();
    
    bool exchange();
    
    virtual void init();
    
    bool move_ins( int ins );
    
    bool move(int ins, int tmp_m);
    
    bool add_machine();
    
    bool del_machine();
    
    void recover(int len = 0);
    
    int moving_ins_id, moving_machine_id;
    
    void show_status();
    
    double ave_score() ;
    
    void accept();
    
    double recalculate_score() ;
    
    void show_extra_info () ;
};

#endif
