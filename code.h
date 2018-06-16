#include "global.h"
#include <map>
#include <vector>
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
    
    double score();
    
    void set_constant( int ins );
    
    bool add_instance(int ins);
    
    bool del_instance( int ins );
};

struct Code {
    set<int> running;
    set<int> stop;
    vector<Machine> m_ins;
    int len;
    
    double u_score;
    int running_num;
    map<int,int> ins_pos;
    
    Code(int _len);
    
    void init();
    
    bool move(int ins);
    
    void recover() ;
    
    int moving_ins_id, moving_machine_id;
    
    void show_status();
};
