#ifndef MOVE_WORKER 
#define MOVE_WORKER

#include "global.h"
#include <iostream>
#include <assert.h>
#include <algorithm>

using namespace global;
using namespace std;

struct MachineWithPreDeploy {
    
    int m_ids;
    
    map<int,int> apps; 
    set<int> ins_ids;
    vector<double> cpu;
    vector<double> mem;
    int disk;
    int P;
    int M;
    int PM;
    
    map<int,int> n_apps; 
    set<int> n_ins_ids;
    vector<double> n_cpu;
    vector<double> n_mem;
    int n_disk;
    int n_P;
    int n_M;
    int n_PM;
    
    set<int> ins_todo;
    set<int> todo_turn;
    
    map<int,int> ob_apps; 
    set<int> ob_ins_ids;
    vector<double> ob_cpu;
    vector<double> ob_mem;
    int ob_disk;
    int ob_P;
    int ob_M;
    int ob_PM;
    
    double score;
    
    set<int> constant;
    
    MachineWithPreDeploy(int ids);
    
    int get_apps( map<int,int> &sapps , int ins_app);
    
    void before_move() ;
    
    void after_move();
    
    bool empty(); 
    
    bool spec_eval( int ins, bool is_object, bool inner = false , bool show = false);
    
    bool inter_eval( int ins, bool is_object, bool inner=false ) ;
    
    bool add_object_instance( int ins ) ;
    
    bool del_object_instance( int ins );
    
    bool add_instance(int ins );
    
    bool del_instance( int ins );
    
    bool del_new_instance( int ins );
    
    bool recover_instance( int ins );
    
    double compute_score();
    
    void print() ;
};

struct MoveWorker {
    
    set<int> running;
    set<int> stop;
    
    vector<MachineWithPreDeploy> m_ins;
    int len;
    
    double u_score;
    map<int,int> ins_pos;
    map<int,int> ob_pos;
    map<int,int> m_pos;
    
    vector<pair<int,int> > temp_results;
    vector<vector<pair<int,int> > > results;
    
    MoveWorker(int _len);
    
    void set_base_pos( vector<int> &ins_mch ) ;
    
    void set_object_pos( map<int,int> &pos );
    
    int move_ins_with_conflicts();
    
    int move_ins_with_conflicts_soft();
    
    int move_ins_directly();
    
    int move_ins_soft(int max_times = 1000,double mem_th= 0.0,  bool show = false);
    
    void before_move();
    
    void after_move();
    
    void init (map<int,int> &pos, vector<int> &ins_mch);
    
    double compute_score() ;
};


vector<pair<int,int> > test_move(map<int,int> &pos, vector<int> &ins_mch);


#endif
