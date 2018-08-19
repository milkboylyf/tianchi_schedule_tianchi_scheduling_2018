#include "merge_parallel.h" 

ParallelMergeWorker::ParallelMergeWorker(int _len): len(_len), m1(0), m2(0) {}

void ParallelMergeWorker::dfs_m_divide(int x) {
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
            tmp_m_ins_ids = m1.ins_ids;
        }
            //cout << score <<endl;
        return ;
    }
    int ins_app = instance_apps[reserved_ins[x]];
    int action = rand()%2, prob = 40, t = 0;
    if (action==0) {
        if (m1.add_instance(reserved_ins[x])) {
            //m1.compute_score();
            apply1 += app_apply[ins_app];
            dfs_m_divide(x+1);
            apply1 -= app_apply[ins_app];
            m1.del_instance(reserved_ins[x]) ;
            //m1.compute_score();
        }
        else t =1;
        if ((t||rand()%200<prob)&& 
            //(x||disk_spec[m1.m_ids]!=disk_spec[m2.m_ids])           //如果两个服务器相同，那么第一元素永远放在m1 
            //    &&
                m2.add_instance(reserved_ins[x])) { 
            //m2.compute_score();
            apply2 += app_apply[ins_app];
            dfs_m_divide(x+1);
            apply2 -= app_apply[ins_app];
            m2.del_instance(reserved_ins[x]) ;
            //m2.compute_score();
        }
    }
    else {
        if (//(x||disk_spec[m1.m_ids]!=disk_spec[m2.m_ids])           //如果两个服务器相同，那么第一元素永远放在m1 
            //    &&
                m2.add_instance(reserved_ins[x])) { 
            //m2.compute_score();
            apply2 += app_apply[ins_app];
            dfs_m_divide(x+1);
            apply2 -= app_apply[ins_app];
            m2.del_instance(reserved_ins[x]) ;
            //m2.compute_score();
        }
        else t = 1;
        if ((t||rand()%200<prob)&& 
            m1.add_instance(reserved_ins[x])) {
            //m1.compute_score();
            apply1 += app_apply[ins_app];
            dfs_m_divide(x+1);
            apply1 -= app_apply[ins_app];
            m1.del_instance(reserved_ins[x]) ;
            //m1.compute_score();
        }
    }
        
}

inline bool ins_cmp( const int &a , const int &b ) {
    return app_max_cpu[instance_apps[a]]<app_max_cpu[instance_apps[b]];
}

void ParallelMergeWorker::before_merge( Code &coder, Machine &machine_1 , Machine &machine_2 ) {
    
    coder.accept();
    reserved_ins.clear();
    m1.m_ids = machine_1.m_ids;
    m2.m_ids = machine_2.m_ids;
    min_cpu_score = machine_1.compute_score() + machine_2.compute_score();
    tmp_m_ins_ids = machine_1.ins_ids;
    
    //u_score -= machine_1.score;
    //u_score -= machine_2.score;
    //machine_1.print();
    //machine_2.print();
    clear_machine(coder,machine_1);
    clear_machine(coder,machine_2);
    machine_1.clear();
    machine_2.clear();
    //sort(reserved_ins.begin(),reserved_ins.end(),ins_cmp);
    reserved_applys[reserved_ins.size()] = 0;
    for (int i=reserved_ins.size()-1;i>=0;i--) 
        reserved_applys[i] = reserved_applys[i+1]+reserved_ins[i];
    //for (auto ins : reserved_ins) cout << ins << " " ;
    //cout <<endl;
    coder.max_ins_pair = max(coder.max_ins_pair,(int)reserved_ins.size());
    apply1 = apply2 = 0;
    search_times = 0;
}

void ParallelMergeWorker::after_merge ( Code &coder, Machine &machine_1 , Machine &machine_2 ) {
    
    assert(min_cpu_score <100000);
    
    for (auto ins: reserved_ins ) if ( tmp_m_ins_ids.count(ins) ) {
        assert( coder.move(ins,machine_1.m_ids) );
    }
    
    for (auto ins: reserved_ins ) if ( !tmp_m_ins_ids.count(ins) ) {
        assert( coder.move(ins,machine_2.m_ids) ) ;
    }
    
    //u_score += machine_1.compute_score();
    //u_score += machine_2.compute_score();
}

void ParallelMergeWorker::merge_machine_2( Code &coder, Machine &machine_1 , Machine &machine_2 ) {
    coder.accept();
    reserved_ins.clear();
    m1.m_ids = machine_1.m_ids;
    m2.m_ids = machine_2.m_ids;
    min_cpu_score = machine_1.compute_score() + machine_2.compute_score();
    tmp_m_ins_ids = machine_1.ins_ids;
    
    //u_score -= machine_1.score;
    //u_score -= machine_2.score;
    //machine_1.print();
    //machine_2.print();
    clear_machine(coder,machine_1);
    clear_machine(coder,machine_2);
    machine_1.clear();
    machine_2.clear();
    //sort(reserved_ins.begin(),reserved_ins.end(),ins_cmp);
    reserved_applys[reserved_ins.size()] = 0;
    for (int i=reserved_ins.size()-1;i>=0;i--) 
        reserved_applys[i] = reserved_applys[i+1]+reserved_ins[i];
    //for (auto ins : reserved_ins) cout << ins << " " ;
    //cout <<endl;
    coder.max_ins_pair = max(coder.max_ins_pair,(int)reserved_ins.size());
    apply1 = apply2 = 0;
    search_times = 0;
    dfs_m_divide(0);
    
    assert(min_cpu_score <100000);
    
    for (auto ins: reserved_ins ) if ( tmp_m_ins_ids.count(ins) ) {
        assert( coder.move(ins,machine_1.m_ids) );
    }
    
    for (auto ins: reserved_ins ) if ( !tmp_m_ins_ids.count(ins) ) {
        assert( coder.move(ins,machine_2.m_ids) ) ;
    }
    
    //u_score += machine_1.compute_score();
    //u_score += machine_2.compute_score();
}


void ParallelMergeWorker::remove_ins( Code &coder, int ins ) {
    int pos = coder.ins_pos[ins];
    coder.u_score -= coder.m_ins[pos].compute_score();
    bool del_successfully = coder.m_ins[pos].del_instance(ins);
    assert(del_successfully);
    coder.u_score += coder.m_ins[pos].compute_score();
    if (coder.m_ins[pos].empty()) coder.running.erase(pos);
    
    coder.ins_remain[coder.disk_index[app_apply[instance_apps[ins]]]]++;
    coder.ins_pos[ins]=0;
}

void ParallelMergeWorker::clear_machine( Code &coder, Machine &m ) {
    while (m.empty()==0) {
        reserved_ins.push_back(*m.ins_ids.begin());
        remove_ins(coder,*m.ins_ids.begin());
    }
}
/*
void merge_machine_g(

void merge_g (ParallelMergeWorker &mw) {
    int m_a, m_b , times = 0;
    do {
        m_a = rand()%machine_resources_num+1;
        times ++;
    }
    while ( ((m_a <= 3000&&mw.m_ins[m_a].score <= 98) || (m_a > 3000 && mw.m_ins[m_a].score <=105)) && times <20000) ;
    if (times >=20000) return;
    do {
        m_b = rand()%machine_resources_num+1;
    }
    while ( ((m_b <= 3000&&mw.m_ins[m_b].score >100) || (m_b > 3000 && mw.m_ins[m_b].score > 110)) || m_b == m_a || mw.m_ins[m_b].empty() );
    merge_machine_2( mw.m_ins[m_a], mw.m_ins[m_b] );
}
*/

void ParallelMergeWorker::merge(Code &coder) {
    coder.accept();
    int m_a, m_b , times = 0;
    int cpu_score[len+2];
    for (int i=3001;i<=6000;i++) cpu_score[i] = coder.m_ins[i].score;
    sort(cpu_score+3001, cpu_score+6001);
    do {
        m_a = rand()%machine_resources_num+1;
        times ++;
    }
    //while( (m_a <= 3000||coder.m_ins[m_a].score <cpu_score[5700]) && times<20000);
    while ( ((m_a <= 3000&&coder.m_ins[m_a].score <= 98) 
        || (m_a > 3000 && coder.m_ins[m_a].score <=980)) && times <20000) ;
    if (times >=20000) return;
    do {
        m_b = rand()%machine_resources_num+1;
        times ++;
    }
    //while( (m_b <=3000||m_ins[m_a].score >cpu_score[3300])  && times<200000);
    while ( (m_b >3000 || m_b == m_a || coder.m_ins[m_b].empty()) && times < 200000 );
    merge_machine_2( coder, coder.m_ins[m_a], coder.m_ins[m_b] ); 
} 

