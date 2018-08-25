#ifndef MERGE_PARA
#define MERGE_PARA

#include "code.h"
#include "bestfit.h"
#include <algorithm>
#include <string.h>

struct ParallelMergeWorker {
    
    vector<int> reserved_ins;
    int reserved_applys[100];
    int apply1, apply2;
    double min_cpu_score;
    set<int> tmp_m_ins_ids;
    int len;
    
    Machine m1, m2 ;
    
    int constant_ins_num;
    
    int search_times ;
    
    ParallelMergeWorker(int _len) ;
    
    void dfs_m_divide( int x ) ;
    
    void before_merge( Code &coder, Machine &machine_1 , Machine &machine_2 );

    void after_merge ( Code &coder, Machine &machine_1 , Machine &machine_2 );
    
    void merge_machine_2( Code &coder, Machine &machine_1 , Machine &machine_2 ) ;
    
    void remove_ins( Code &coder, int ins );
    
    void clear_machine( Code &coder, Machine &m );
    
    void merge(Code &coder) ;
    
   // bool make_integer_result( int i_score ) ;
    
};

#endif
