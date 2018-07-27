#ifndef MERGE_MC
#define MERGE_MC

#include "code.h"
#include <algorithm>
#include <string.h>

struct RestrictedMachine: Machine {
    
}

struct MergeWorker :Code {
    
    vector<int> reserved_ins;
    int reserved_applys[100];
    int apply1, apply2;
    double min_cpu_score;
    set<int> tmp_m_ins_ids;
    
    Machine m1, m2 ;
    
    int search_times ;
    
    MergeWorker(int _len) ;
    
    void dfs_m_divide( int x ) ;
    
    void merge_machine_2( Machine &machine_1 , Machine &machine_2 ) ;
    
    void remove_ins( int ins );
    
    void clear_machine( Machine &m );
    
    void merge() ;
    
    bool make_integer_result( int i_score ) ;
    
};



#endif
