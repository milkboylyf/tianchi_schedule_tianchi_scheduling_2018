#include "merge.h"


MergeWorker::MergeWorker(int _len): Code(_len), m1(0), m2(0) {}

void MergeWorker::dfs_m_divide(int x) {
    //cout << x << " " << reserved_ins[x] << endl;
    search_times ++;
    if ( min_cpu_score <= 196 || search_times > 1000000) return ;
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
    if (m1.add_instance(reserved_ins[x])) {
        apply1 += app_apply[ins_app];
        dfs_m_divide(x+1);
        apply1 -= app_apply[ins_app];
        m1.del_instance(reserved_ins[x]) ;
    }
    if ((x||disk_spec[m1.m_ids]!=disk_spec[m2.m_ids])           //如果两个服务器相同，那么第一元素永远放在m1 
            &&m2.add_instance(reserved_ins[x])) { 
        apply2 += app_apply[ins_app];
        dfs_m_divide(x+1);
        apply2 -= app_apply[ins_app];
        m2.del_instance(reserved_ins[x]) ;
    }
}

void MergeWorker::merge_machine_2( Machine &machine_1 , Machine &machine_2 ) {
    accept();
    reserved_ins.clear();
    m1.m_ids = machine_1.m_ids;
    m2.m_ids = machine_2.m_ids;
    min_cpu_score = 1e7;
    
    //u_score -= machine_1.score;
    //u_score -= machine_2.score;
    //machine_1.print();
    //machine_2.print();
    clear_machine(machine_1);
    clear_machine(machine_2);
    reserved_applys[reserved_ins.size()] = 0;
    for (int i=reserved_ins.size()-1;i>=0;i--) 
        reserved_applys[i] = reserved_applys[i+1]+reserved_ins[i];
    //for (auto ins : reserved_ins) cout << ins << " " ;
    //cout <<endl;
    apply1 = apply2 = 0;
    search_times = 0;
    dfs_m_divide(0);
    
    for (auto ins: reserved_ins ) if ( tmp_m_ins_ids.count(ins) ) {
        assert( move(ins,machine_1.m_ids) );
    }
    
    for (auto ins: reserved_ins ) if ( !tmp_m_ins_ids.count(ins) ) {
        assert( move(ins,machine_2.m_ids) ) ;
    }
    
    //u_score += machine_1.compute_score();
    //u_score += machine_2.compute_score();
}


void MergeWorker::remove_ins( int ins ) {
    u_score -= m_ins[ins_pos[ins]].compute_score();
    bool del_successfully = m_ins[ins_pos[ins]].del_instance(ins);
    assert(del_successfully);
    u_score += m_ins[ins_pos[ins]].compute_score();
    if (m_ins[ins_pos[ins]].empty()) running.erase(ins_pos[ins]);
    
    ins_remain[disk_index[app_apply[instance_apps[ins]]]]++;
    ins_pos[ins]=0;
}

void MergeWorker::clear_machine( Machine &m ) {
    while (m.empty()==0) {
        reserved_ins.push_back(*m.ins_ids.begin());
        remove_ins(*m.ins_ids.begin());
    }
}


void MergeWorker::merge() {
    accept();
    int m_a, m_b , times = 0;
    do {
        m_a = rand()%machine_resources_num+1;
        times ++;
    }
    while ( m_ins[m_a].score <= 98 && times <20000) ;
    if (times >=20000) return;
    do {
        m_b = rand()%machine_resources_num+1;
    }
    while ( m_b == m_a || m_ins[m_b].empty());
    merge_machine_2( m_ins[m_a], m_ins[m_b] ); 
    if (running.size()==5507) 
        scanf("%*d");
} 
    
