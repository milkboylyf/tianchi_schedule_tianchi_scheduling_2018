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
    vector<int> cpu;
    vector<int> mem;
    map<int,int> apps; 
    int disk;
    int P;
    int M;
    int PM;
    
    set<int> constant;
    
    Machine(int ids);
    
    bool empty(); 
    
    void clear();
    
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
};

#endif
