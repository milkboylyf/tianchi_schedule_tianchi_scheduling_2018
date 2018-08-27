#ifndef MOVE_PARA
#define MOVE_PARA

#include "move.h"
#include "match.h"
#include "code.h"
#include <algorithm>
#include <string.h>

#include <thread>
#include <mutex>

struct ParallelMoveWorker {
    
    vector<int> reserved_ins;
    double min_cpu_score;
    set<int> tmp_n_ins_ids;
    set<int> tmp_todo_turn;
    set<int> tmp_n_ins_ids2;
    set<int> tmp_todo_turn2;
    int len;
    
    set<int> check_ins;
    set<int> check_out;
    
    MachineWithPreDeploy m1, m2 ;
    
    int constant_ins_num;
    
    int search_times ;
    
    ParallelMoveWorker(int _len) ;
    
    void dfs_m_divide( int x ) ;
    
    void before_merge( MoveWorker &coder, MachineWithPreDeploy &machine_1 , MachineWithPreDeploy &machine_2 );

    void after_merge ( MoveWorker &coder, MachineWithPreDeploy &machine_1 , MachineWithPreDeploy &machine_2 );
    
    void merge_machine_2 () ;
    
    void clear_machine( MachineWithPreDeploy &m );
    
    void load_machine( MachineWithPreDeploy &m );
    
};

#endif
