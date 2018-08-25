#include "move_parallel.h" 


ParallelMoveWorker::ParallelMoveWorker(int _len): len(_len), m1(0), m2(0) {}

inline bool add_ins( MachineWithPreDeploy &m , int ins, bool show = false) {
    if (m.ins_todo.count(ins)) {
        assert(m.n_ins_ids.count(ins));
        m.recover_instance(ins);
        return true;
    }
    else {
        if ( m.inter_eval(ins,true) && m.spec_eval(ins,true)) {
            m.add_instance(ins);
            return true;
        }
    }
    return false;
}

inline bool del_ins( MachineWithPreDeploy &m , int ins) {
    if (m.ins_todo.count(ins)) {
        m.del_instance(ins);
        return true;
    }
    else {
        m.del_new_instance(ins);
        return true;
    }
    return false;
}

void ParallelMoveWorker::dfs_m_divide(int x) {
    //cout << x << " " << reserved_ins[x] << endl;
    search_times ++;
    if ( search_times > 100000) return ;
    if ( x >= reserved_ins.size() ) {
        /*
        cout << m1.ins_ids.size() <<endl;
        for (auto ins :m1.ins_ids ) 
            cout << ins << " " ;
        cout << endl;
        //*/
        double score = m1.compute_score() + m2.compute_score();
        if ( score < min_cpu_score ) {
            min_cpu_score = score;
            tmp_n_ins_ids = m1.n_ins_ids;
            tmp_todo_turn = m1.todo_turn;
            tmp_n_ins_ids2 = m2.n_ins_ids;
            tmp_todo_turn2 = m2.todo_turn;
            check_ins = check_out;
        }
            //cout << score <<endl;
        return ;
    }
    //int ins_app = instance_apps[reserved_ins[x]];
    int action = rand()%2, prob = 40, t = 0;
    if (action==0) {
        if (add_ins(m1,reserved_ins[x])) {
            check_out.insert(x);
            assert(( m1.todo_turn.count(reserved_ins[x]) 
                    || ( m1.ins_todo.count(reserved_ins[x]) 
                        && !m1.n_ins_ids.count(reserved_ins[x]) ) )) ;
            //m1.compute_score();
            dfs_m_divide(x+1);
            del_ins(m1,reserved_ins[x]) ;
            check_out.erase(x);
            //m1.compute_score();
        }
        else t =1;
        if ((t||rand()%200<prob)&& 
            //(x||disk_spec[m1.m_ids]!=disk_spec[m2.m_ids])           //如果两个服务器相同，那么第一元素永远放在m1 
            //    &&
                add_ins(m2,reserved_ins[x])) { 
                
            assert (!( m1.todo_turn.count(reserved_ins[x]) 
                    || ( m1.ins_todo.count(reserved_ins[x]) 
                        && !m1.n_ins_ids.count(reserved_ins[x]) ) ));
            //m2.compute_score();
            dfs_m_divide(x+1);
            del_ins(m2,reserved_ins[x]) ;
            //m2.compute_score();
        }
    }
    else {
        if (//(x||disk_spec[m1.m_ids]!=disk_spec[m2.m_ids])           //如果两个服务器相同，那么第一元素永远放在m1 
            //    &&
                add_ins(m2,reserved_ins[x])) { 
            assert (!( m1.todo_turn.count(reserved_ins[x]) 
                    || ( m1.ins_todo.count(reserved_ins[x]) 
                        && !m1.n_ins_ids.count(reserved_ins[x]) ) )) ;
            //m2.compute_score();
            dfs_m_divide(x+1);
            del_ins(m2,reserved_ins[x]) ;
            //m2.compute_score();
        }
        else t = 1;
        if ((t||rand()%200<prob)&& 
            add_ins(m1,reserved_ins[x])) {
            check_out.insert(x);
            assert (( m1.todo_turn.count(reserved_ins[x]) 
                    || ( m1.ins_todo.count(reserved_ins[x]) 
                        && !m1.n_ins_ids.count(reserved_ins[x]) ) ));
            //m1.compute_score();
            dfs_m_divide(x+1);
            check_out.erase(x);
            del_ins(m1,reserved_ins[x]) ;
            //m1.compute_score();
        }
    }
        
}

void ParallelMoveWorker::before_merge( MoveWorker &coder, MachineWithPreDeploy &machine_1 , MachineWithPreDeploy &machine_2 ) {
    
    m1 = machine_1;
    m2 = machine_2;
    
}

void ParallelMoveWorker::after_merge ( MoveWorker &coder, MachineWithPreDeploy &machine_1 , MachineWithPreDeploy &machine_2 ) {
    
    machine_1 = m1;
    machine_2 = m2;
}

void ParallelMoveWorker::merge_machine_2() {
    reserved_ins.clear();
    min_cpu_score = m1.compute_score() + m2.compute_score();
    tmp_n_ins_ids = m1.n_ins_ids;
    tmp_todo_turn = m1.todo_turn;
    tmp_n_ins_ids2 = m2.n_ins_ids;
    tmp_todo_turn2 = m2.todo_turn;

    //m1.print();
    //m2.print();
    check_out.clear();
    clear_machine(m1);
    check_ins = check_out;
    clear_machine(m2);
    check_out.clear();
    
    search_times = 0;
    dfs_m_divide(0);
    
    for (int i=0 ;i<reserved_ins.size();i++) {
        int t = reserved_ins[i];
        if ( tmp_todo_turn.count(t) || ( m1.ins_todo.count(t) && !tmp_n_ins_ids.count(t) ) ) {
            if (!check_ins.count(i)) {
                m1.print();
                m2.print();
                for (auto &di: reserved_ins) cout <<di << " " ;cout << endl;
                for (auto &di: tmp_todo_turn) cout <<di << " " ;cout << endl;
                for (auto &di: tmp_n_ins_ids) cout <<di << " " ;cout << endl;
                for (auto &di: tmp_todo_turn2) cout <<di << " " ;cout << endl;
                for (auto &di: tmp_n_ins_ids2) cout <<di << " " ;cout << endl;
                cout << "###" << i << " " << t << ":" << instance_apps[t] << endl;
                exit(0);
            }
        }
        else {
            if (check_ins.count(i)) {
                m1.print();
                m2.print();
                search_times = 0;
                dfs_m_divide(0);
                for (auto &di: reserved_ins) cout <<di << " " ;cout << endl;
                for (auto &di: tmp_todo_turn) cout <<di << " " ;cout << endl;
                for (auto &di: tmp_n_ins_ids) cout <<di << " " ;cout << endl;
                for (auto &di: tmp_todo_turn2) cout <<di << " " ;cout << endl;
                for (auto &di: tmp_n_ins_ids2) cout <<di << " " ;cout << endl;
                cout << "###" <<  t << ":" << instance_apps[t] << endl;
                exit(0);
            }
        }
    }
    
    //for (int t : reserved_ins ) {
    for (int i=0 ;i<reserved_ins.size();i++) {
        int t = reserved_ins[i];
        if ( tmp_todo_turn.count(t) || ( m1.ins_todo.count(t) && !tmp_n_ins_ids.count(t) ) ) {
            if (!check_ins.count(i)) {
                m1.print();
                m2.print();
                cout << i << " " << t << ":" << instance_apps[t] << endl;
                exit(0);
            }
            //assert(check_ins.count(i));
            if (!add_ins( m1, t)) {
                m1.print();
                add_ins( m1, t);
                exit(0);
            }
        }
        else {
            if (check_ins.count(i)) {
                m1.print();
                m2.print();
                search_times = 0;
                //dfs_m_divide(0);
                cout << t << ":" << instance_apps[t] << endl;
                exit(0);
            }
            if (!add_ins( m2, t)) {
                m2.print();
                add_ins( m2, t , true);
                exit(0);
            }
        }
    }
}

void ParallelMoveWorker::clear_machine( MachineWithPreDeploy &m ) {
    set<int> tmp_n = m.n_ins_ids;
    set<int> tmp_todo = m.todo_turn;
    for (int t : m.ins_todo) if (!tmp_n.count(t)) {
        check_out.insert(reserved_ins.size());
        reserved_ins.push_back(t);
        m.del_instance(t);
    }
    for (int t : tmp_todo ) {
        check_out.insert(reserved_ins.size());
        reserved_ins.push_back(t);
        m.del_new_instance(t);
    }
}





vector<pair<int,int> > test_move(map<int,int> &pos, vector<int> &ins_mch) {
    MoveWorker mw(machine_resources_num);
    
    mw.init(pos, ins_mch);
    
    //int test_ins = 4031;
    //mw.m_ins[test_ins].print();
    
    
    mw.before_move();
    
    ParallelMoveWorker pmw(machine_resources_num);
    
    //for (int t : mw.m_ins[test_ins].ins_todo) 
    //    if ( !mw.m_ins[test_ins].inter_eval(t,false,true) ) cout << t << " " ;
    //cout << endl;
    
    cout << mw.move_ins_with_conflicts() << endl;
    cout << mw.move_ins_directly() <<endl;
    //for (int t : mw.m_ins[test_ins].ins_todo) 
    //    if ( !mw.m_ins[test_ins].n_ins_ids.count(t) && !mw.m_ins[test_ins].inter_eval(t,false,true) ) cout << t << " " ;
    //cout << endl;
    
    cout << mw.move_ins_soft(100000,30,true) <<endl;
    cout << mw.move_ins_with_conflicts_soft() <<endl;
    cout << mw.move_ins_soft(500) <<endl;
    cout << mw.move_ins_soft(500) <<endl;
    cout << mw.move_ins_soft(500) <<endl;
    cout << mw.move_ins_soft(500) <<endl;
    cout << mw.move_ins_soft(500) <<endl;
    
    int counter = 0;
    double t_score = 1e8;
    /*
    while ( t_score > 800000 && counter < 10000) {
            int m_a, m_b , times = 0;
            do {
                m_a = rand()%machine_resources_num+1;
                times ++;
            }
            while ( (mw.m_ins[m_a].compute_score()<=0.1) && times <20000) ;
            if (times >=20000) break;
            do {
                m_b = rand()%machine_resources_num+1;
                times ++;
            }
            while ( (m_b==m_a||mw.m_ins[m_b].compute_score()<=0.1) && times < 200000 );
            if (times >=200000) break;
            pmw.before_merge(mw,mw.m_ins[m_a],mw.m_ins[m_b]);
            pmw.merge_machine_2();
            pmw.after_merge(mw,mw.m_ins[m_a],mw.m_ins[m_b]);
        if (counter%10==0) {
            t_score=mw.compute_score();
            cout << t_score <<endl;
        }
        counter++;
    }
    //*/
    
    //cout << mw.move_ins_with_conflicts_soft() <<endl;
    //cout << mw.move_ins_soft() <<endl;
    mw.after_move();
    
    cout << "#############" <<endl;
    mw.before_move();
    
    //for (int t : mw.m_ins[test_ins].ins_todo) 
    //    if ( !mw.m_ins[test_ins].inter_eval(t,false,true) ) cout << t << " " ;
    //cout << endl;
    
    cout << mw.move_ins_with_conflicts() << endl;
    cout << mw.move_ins_directly() <<endl;
    //for (int t : mw.m_ins[test_ins].ins_todo) 
    //    if ( !mw.m_ins[test_ins].n_ins_ids.count(t) && !mw.m_ins[test_ins].inter_eval(t,false,true) ) cout << t << " " ;
    //cout << endl;
    
    cout << mw.move_ins_with_conflicts_soft() <<endl;
    cout << mw.move_ins_soft(100000,0.0,true) <<endl;
    
    /*
    counter = 0;
    t_score = 1e8;
    while ( t_score > 0 && counter <10000 ) {
            int m_a, m_b , times = 0;
            do {
                m_a = rand()%machine_resources_num+1;
                times ++;
            }
            while ( (mw.m_ins[m_a].compute_score()>0) && times <20000) ;
            if (times >=20000) break;
            do {
                m_b = rand()%machine_resources_num+1;
                times ++;
            }
            while ( (m_b==m_a) && times < 200000 );
            if (times >=200000) break;
            pmw.before_merge(mw,mw.m_ins[m_a],mw.m_ins[m_b]);
            pmw.merge_machine_2();
            pmw.after_merge(mw,mw.m_ins[m_a],mw.m_ins[m_b]);
        if (counter%10==0) {
            t_score=mw.compute_score();
            cout << t_score <<endl;
        }
        counter++;
    }
    //*/
    
    mw.after_move();
    cout << "#############" <<endl;
    mw.before_move();
    
    //for (int t : mw.m_ins[test_ins].ins_todo) 
    //    if ( !mw.m_ins[test_ins].inter_eval(t,false,true) ) cout << t << " " ;
    //cout << endl;
    
    cout << mw.move_ins_with_conflicts() << endl;
    cout << mw.move_ins_directly() <<endl;
    //for (int t : mw.m_ins[test_ins].ins_todo) 
    //    if ( !mw.m_ins[test_ins].n_ins_ids.count(t) && !mw.m_ins[test_ins].inter_eval(t,false,true) ) cout << t << " " ;
    //cout << endl;
    cout << mw.move_ins_with_conflicts_soft() <<endl;
    cout << mw.move_ins_soft(10000) <<endl;
    mw.after_move();
    cout << "#############" <<endl;
    mw.before_move();
    
    //for (int t : mw.m_ins[test_ins].ins_todo) 
    //    if ( !mw.m_ins[test_ins].inter_eval(t,false,true) ) cout << t << " " ;
    //cout << endl;
    
    cout << mw.move_ins_with_conflicts() << endl;
    cout << mw.move_ins_directly() <<endl;
    //for (int t : mw.m_ins[test_ins].ins_todo) 
    //    if ( !mw.m_ins[test_ins].n_ins_ids.count(t) && !mw.m_ins[test_ins].inter_eval(t,false,true) ) cout << t << " " ;
    //cout << endl;
    cout << mw.move_ins_with_conflicts_soft() <<endl;
    cout << mw.move_ins_soft() <<endl;
    mw.after_move();
    cout << "#############" <<endl;
    mw.before_move();
    
    //for (int t : mw.m_ins[test_ins].ins_todo) 
    //    if ( !mw.m_ins[test_ins].inter_eval(t,false,true) ) cout << t << " " ;
    //cout << endl;
    
    cout << mw.move_ins_with_conflicts() << endl;
    cout << mw.move_ins_directly() <<endl;
    //for (int t : mw.m_ins[test_ins].ins_todo) 
    //    if ( !mw.m_ins[test_ins].n_ins_ids.count(t) && !mw.m_ins[test_ins].inter_eval(t,false,true) ) cout << t << " " ;
    //cout << endl;
    cout << mw.move_ins_with_conflicts_soft() <<endl;
    cout << mw.move_ins_soft() <<endl;
    mw.after_move();
    cout << "#############" <<endl;
    mw.before_move();
    
    //for (int t : mw.m_ins[test_ins].ins_todo) 
    //    if ( !mw.m_ins[test_ins].inter_eval(t,false,true) ) cout << t << " " ;
    //cout << endl;
    
    cout << mw.move_ins_with_conflicts() << endl;
    cout << mw.move_ins_directly() <<endl;
    //for (int t : mw.m_ins[test_ins].ins_todo) 
    //    if ( !mw.m_ins[test_ins].n_ins_ids.count(t) && !mw.m_ins[test_ins].inter_eval(t,false,true) ) cout << t << " " ;
    //cout << endl;
    cout << mw.move_ins_with_conflicts_soft() <<endl;
    cout << mw.move_ins_soft() <<endl;
    mw.after_move();
    
    return mw.temp_results;
    
}

