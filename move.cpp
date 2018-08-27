#include "move.h"

MachineWithPreDeploy::MachineWithPreDeploy(int ids):m_ids(ids),disk(0),P(0),M(0),PM(0),ob_disk(0),ob_P(0),ob_M(0),ob_PM(0) {
    for (int i=0;i<time_len;i++) cpu.push_back(0); 
    for (int i=0;i<time_len;i++) mem.push_back(0); 
    for (int i=0;i<time_len;i++) ob_cpu.push_back(0); 
    for (int i=0;i<time_len;i++) ob_mem.push_back(0); 
    for (int i=0;i<time_len;i++) n_cpu.push_back(0); 
    for (int i=0;i<time_len;i++) n_mem.push_back(0); 
}

int MachineWithPreDeploy::get_apps( map<int,int> &sapps , int ins_app) {
    return sapps.count(ins_app)?sapps[ins_app]:0;
}

double MachineWithPreDeploy::compute_score() {
    double score = 0;
    for (int i=0;i<time_len;i++) {
        score +=  max( 0.0 , mem[i] + ob_mem[i] - n_mem[i] - mem_spec[m_ids] );
        //score += max( 0.0 , cpu[i] + ob_cpu[i] - n_cpu[i] - cpu_spec[m_ids] );
    }
    /*
    score +=  max( 0,  disk + ob_disk - n_disk -disk_spec[m_ids] )
               + max( 0, P + ob_P - n_P - p_lim[m_ids] ) *10
                + max(0, M + ob_M - n_M - m_lim[m_ids] ) *10//there's no positive value in app_m 
                + max(0, PM + ob_PM - n_PM - pm_lim[m_ids] ) *10;
                
    //set<int> total_apps;
    //for (int t:ins_ids) if (!n_ins_ids.count(t)) total_apps.insert(instance_apps[t]);
    //for (int t:todo_turn) total_apps.insert(instance_apps[t]);
    
    for (auto ins_app:ins_ids) if (!n_ins_ids.count(ins_app)) { 
        if (app_inter_set.count(ins_app)) 
        for (auto &t:app_inter_set[ins_app])
            if ( (get_apps(apps, t.first) + get_apps(ob_apps, t.first) - get_apps(n_apps, t.first) ) 
                    &&  t.second< get_apps(apps, ins_app) + get_apps(ob_apps, ins_app) - get_apps(n_apps, ins_app) ) { 
                //    cout << "INTERFERENCE " << t.first << " " << ins_app << " " << t.second << endl;
                score+=1000;
        }
        if (app_rvs_inter_set.count(ins_app)) 
        for (auto &t:app_rvs_inter_set[ins_app])
            if ( t.second < get_apps(apps, t.first) + get_apps(ob_apps, t.first) - get_apps(n_apps, t.first) ) {
                //    cout << "INTERFERENCE2 " << t.first << " " << ins_app << " " << t.second << endl;
                score+=1000;
        }
    }
    */
    
    return score;
}

bool MachineWithPreDeploy::spec_eval( int ins, bool is_object , bool inner, bool show) {
    
    int ins_app = instance_apps[ins];
    if (is_object) {
        if ( disk_spec[m_ids] < disk + app_apply[ins_app]           
            || p_lim[m_ids] < P + app_p[ins_app]
            || m_lim[m_ids] < M + app_m[ins_app]                 //there's no positive value in app_m 
            || pm_lim[m_ids] < PM + app_pm[ins_app] ) 
            return false;
        for (int i=0;i<time_len;i++) 
            if (  cpu_spec[m_ids] < cpu[i] + app_cpu_line[ins_app][i] - 1e-8
                || mem_spec[m_ids] < mem[i] + app_mem_line[ins_app][i] - 1e-8 ) {
                    //cout << "CPU & MEM" <<endl;
            return false;
        }
    }
    else {
        if (inner) {
            /*
            if ( disk_spec[m_ids] < disk + max(0, ob_disk - n_disk) 
                || p_lim[m_ids] < P + max(0, ob_P - n_P) 
                || m_lim[m_ids] < M + max(0, ob_M - n_M)  //there's no positive value in app_m 
                || pm_lim[m_ids] < PM + max(0, ob_PM - n_PM) ) 
                return false;
            for (int i=0;i<time_len;i++) 
                if (  cpu_spec[m_ids] < cpu[i] + max(0.0, ob_cpu[i] - n_cpu[i]) - 1e-8 
                    || mem_spec[m_ids] < mem[i] + max(0.0, ob_mem[i] - n_mem[i]) - 1e-8 ) {
                        //cout << "CPU & MEM" <<endl;
                return false;
            }
            */
            if ( disk_spec[m_ids] < disk + ob_disk - n_disk 
                || p_lim[m_ids] < P + ob_P - n_P
                || m_lim[m_ids] < M + ob_M - n_M  //there's no positive value in app_m 
                || pm_lim[m_ids] < PM + ob_PM - n_PM ) 
                return false;
            for (int i=0;i<time_len;i++) 
                if (  cpu_spec[m_ids] < cpu[i] + ob_cpu[i] - n_cpu[i] - 1e-8 
                    || mem_spec[m_ids] < mem[i] + ob_mem[i] - n_mem[i] - 1e-8 ) {
                        //cout << "CPU & MEM" <<endl;
                return false;
            }
        }
        else {
            if ( disk_spec[m_ids] < disk + max(0, ob_disk - n_disk) + app_apply[ins_app]           
                || p_lim[m_ids] < P + max(0, ob_P - n_P) + app_p[ins_app]
                || m_lim[m_ids] < M + max(0, ob_M - n_M) + app_m[ins_app]                 //there's no positive value in app_m 
                || pm_lim[m_ids] < PM + max(0, ob_PM - n_PM) + app_pm[ins_app] ) 
                return false;
            for (int i=0;i<time_len;i++) 
                if (  cpu_spec[m_ids] < cpu[i] + max(0.0, ob_cpu[i] - n_cpu[i]) + app_cpu_line[ins_app][i] - 1e-8 
                    || mem_spec[m_ids] < mem[i] + max(0.0, ob_mem[i] - n_mem[i]) + app_mem_line[ins_app][i] - 1e-8 ) {
                        //cout << "CPU & MEM" <<endl;
                if (show ) {
                    cout << cpu_spec[m_ids] - (cpu[i] + max(0.0, ob_cpu[i] - n_cpu[i]) + app_cpu_line[ins_app][i] )
                     << " " << mem_spec[m_ids] - (mem[i] + max(0.0, ob_mem[i] - n_mem[i]) + app_mem_line[ins_app][i] ) << endl; 
                }
                return false;
            }
        }
    }
    return true;
}

//查找干扰和反向干扰 
bool MachineWithPreDeploy::inter_eval( int ins, bool is_object, bool inner ) {
    
    int ins_app = instance_apps[ins];
    if (is_object) {
        if (app_inter_set.count(ins_app)) 
        for (auto &t:app_inter_set[ins_app])
            if ( apps.count(t.first) && (apps.count(ins_app) ? t.second<=apps[ins_app] : !t.second ) ) { 
                //    cout << "INTERFERENCE " << t.first << " " << ins_app << " " << t.second << endl;
                return false;
        }
        if (app_rvs_inter_set.count(ins_app)) 
        for (auto &t:app_rvs_inter_set[ins_app])
            if ( apps.count(t.first) && t.second<apps[t.first] ) {
                //    cout << "INTERFERENCE2 " << t.first << " " << ins_app << " " << t.second << endl;
                return false;
        }
    }
    else {
        if (inner) {
            if (app_inter_set.count(ins_app)) 
            for (auto &t:app_inter_set[ins_app])
                if ( (get_apps(apps, t.first) + get_apps(ob_apps, t.first) - get_apps(n_apps, t.first) ) 
                        &&  t.second< get_apps(apps, ins_app) + get_apps(ob_apps, ins_app) - get_apps(n_apps, ins_app) ) { 
                    //    cout << "INTERFERENCE " << t.first << " " << ins_app << " " << t.second << endl;
                    return false;
            }
            if (app_rvs_inter_set.count(ins_app)) 
            for (auto &t:app_rvs_inter_set[ins_app])
                if ( t.second < get_apps(apps, t.first) + get_apps(ob_apps, t.first) - get_apps(n_apps, t.first) ) {
                    //    cout << "INTERFERENCE2 " << t.first << " " << ins_app << " " << t.second << endl;
                    return false;
            }
        }
        else {
            if (app_inter_set.count(ins_app)) 
            for (auto &t:app_inter_set[ins_app])
                if ( (apps.count(t.first)+ob_apps.count(t.first)) 
                        &&  t.second<=get_apps(apps, ins_app)+get_apps(ob_apps, ins_app) ) { 
                    //    cout << "INTERFERENCE " << t.first << " " << ins_app << " " << t.second << endl;
                    return false;
            }
            if (app_rvs_inter_set.count(ins_app)) 
            for (auto &t:app_rvs_inter_set[ins_app])
                if ( t.second < get_apps(apps, t.first)+get_apps(ob_apps, t.first) ) {
                    //    cout << "INTERFERENCE2 " << t.first << " " << ins_app << " " << t.second << endl;
                    return false;
            }
        }
        
    }
    return true;
}

bool MachineWithPreDeploy::add_object_instance( int ins ) {
    
    int ins_app = instance_apps[ins];
    
    ob_apps[ins_app]++;
    assert(!ob_ins_ids.count(ins));
    ob_ins_ids.insert(ins);
    
    for (int i=0;i<time_len;i++) { 
        ob_cpu[i] += app_cpu_line[ins_app][i];
        ob_mem[i] += app_mem_line[ins_app][i];
    }
    ob_disk += app_apply[ins_app] ;
    ob_P += app_p[ins_app];
    ob_M += app_m[ins_app];                 //there's no positive value in app_m 
    ob_PM += app_pm[ins_app]; 
    
    return true;
}

bool MachineWithPreDeploy::add_instance( int ins ) {
    
    int ins_app = instance_apps[ins];
    
    if (ins_ids.count(ins)) {
        cout << ins << " " << instance_ids[ins] << " " << m_ids << endl;
        exit(0);
    }
    ins_ids.insert(ins);
    apps[ins_app]++;
    
    for (int i=0;i<time_len;i++) {
        cpu[i] += app_cpu_line[ins_app][i];
        mem[i] += app_mem_line[ins_app][i];
    }
    disk += app_apply[ins_app] ;
    P += app_p[ins_app];
    M += app_m[ins_app];                 //there's no positive value in app_m 
    PM += app_pm[ins_app]; 
    
    if (ob_ins_ids.count(ins)) {
        del_object_instance(ins);
        //cout<< ins <<endl;
    }
    else {
        assert(!todo_turn.count(ins));
        todo_turn.insert(ins);
    }
    
    return true;
}

bool MachineWithPreDeploy::del_object_instance( int ins ) {
    
    int ins_app = instance_apps[ins];
    
    assert(ob_ins_ids.count(ins));
    ob_ins_ids.erase(ins);
    ob_apps[ins_app]--;
    if (ob_apps[ins_app] == 0) ob_apps.erase(ins_app);
    
    for (int i=0;i<time_len;i++) {
        ob_cpu[i] -= app_cpu_line[ins_app][i];
        ob_mem[i] -= app_mem_line[ins_app][i];
    }
    ob_disk -= app_apply[ins_app] ;
    ob_P -= app_p[ins_app];
    ob_M -= app_m[ins_app];                 //there's no positive value in app_m 
    ob_PM -= app_pm[ins_app]; 
    
    return true;
}

bool MachineWithPreDeploy::del_instance( int ins ) {
    
    int ins_app = instance_apps[ins];
    
    assert(!n_ins_ids.count(ins));
    n_ins_ids.insert(ins);
    n_apps[ins_app]++;
    
    for (int i=0;i<time_len;i++) {
        n_cpu[i] += app_cpu_line[ins_app][i];
        n_mem[i] += app_mem_line[ins_app][i];
    }
    n_disk += app_apply[ins_app] ;
    n_P += app_p[ins_app];
    n_M += app_m[ins_app];                 //there's no positive value in app_m 
    n_PM += app_pm[ins_app]; 
    
    return true;
}


bool MachineWithPreDeploy::del_new_instance( int ins ) {
    
    int ins_app = instance_apps[ins];
    
    assert(ins_ids.count(ins)&&todo_turn.count(ins));
    ins_ids.erase(ins);
    todo_turn.erase(ins);
    apps[ins_app]--;
    
    if (apps[ins_app] == 0) apps.erase(ins_app);
    
    for (int i=0;i<time_len;i++) {
        cpu[i] -= app_cpu_line[ins_app][i];
        mem[i] -= app_mem_line[ins_app][i];
    }
    disk -= app_apply[ins_app] ;
    P -= app_p[ins_app];
    M -= app_m[ins_app];                 //there's no positive value in app_m 
    PM -= app_pm[ins_app]; 
    
    return true;
}

bool MachineWithPreDeploy::recover_instance( int ins ) {
    
    int ins_app = instance_apps[ins];
    
    assert(n_ins_ids.count(ins));
    n_ins_ids.erase(ins);
    n_apps[ins_app]--;
    
    if (n_apps[ins_app] == 0) n_apps.erase(ins_app);
    
    for (int i=0;i<time_len;i++) {
        n_cpu[i] -= app_cpu_line[ins_app][i];
        n_mem[i] -= app_mem_line[ins_app][i];
    }
    n_disk -= app_apply[ins_app] ;
    n_P -= app_p[ins_app];
    n_M -= app_m[ins_app];                 //there's no positive value in app_m 
    n_PM -= app_pm[ins_app]; 
    
    return true;
}

void MachineWithPreDeploy::before_move() {
    n_apps.clear();
    n_ins_ids.clear();
    todo_turn.clear();
    for (int i=0;i<time_len;i++) {
        n_cpu[i] = n_mem[i] = 0;
    }
    n_disk= 0;
    n_P= 0;
    n_M = 0;
    n_PM = 0;
}

void MachineWithPreDeploy::after_move() {
    for (auto &t : n_apps) {
        apps[t.first]-=t.second;
        if (!apps[t.first])
            apps.erase(t.first);
    }
    for (auto &t : n_ins_ids) {
        assert(ins_ids.count(t)&&ins_todo.count(t));
        ins_ids.erase(t);
        ins_todo.erase(t);
    }
    for (int i=0;i<time_len;i++) {
        cpu[i] -= n_cpu[i];
        mem[i] -= n_mem[i];
    }
    disk -= n_disk;
    P -= n_P;
    M -= n_M;
    PM -= n_PM;
    for (auto &t : todo_turn )
        ins_todo.insert(t);
}

void MachineWithPreDeploy::print() {
    cout << disk << " " << P << " " << M << " " << PM << " " << m_ids << endl;
    for (auto t: ins_ids ) {
        cout << t << ":" << instance_apps[t] << " ";
    }
    cout << endl;
    for (auto t: n_ins_ids ) {
        cout << t << ":" << instance_apps[t] << " ";
    }
    cout << endl;
    for (auto t: ins_todo ) {
        cout << t << ":" << instance_apps[t] << " ";
    }
    cout << endl;
    for (auto t: todo_turn ) {
        cout << t << ":" << instance_apps[t] << " ";
    }
    cout << endl;
    for (auto t: ob_ins_ids ) {
        cout << t << ":" << instance_apps[t] << " ";
    }
    cout << endl;
}


void MoveWorker::set_base_pos( vector<int> &ins_mch ) {
    for (int i=1;i<=instance_deploy_num;i++) 
    if (ins_mch[i]!=-1)
    {
        m_ins[ins_mch[i]].add_instance(i);
        ins_pos[i] = ins_mch[i];
    }
    else ins_pos[i]=0;
}
    
void MoveWorker::set_object_pos( map<int,int> &pos ) {
    ob_pos = pos;
    for (auto &t : pos ) {
        m_ins[t.second].add_object_instance(t.first);
    }
}

MoveWorker::MoveWorker(int _len):len(_len),u_score(0){
    for (int i=0;i<=_len;i++) m_ins.push_back(MachineWithPreDeploy(i));
}

void MoveWorker::before_move() {
    temp_results.clear();
    for (int i=1;i<=len;i++) m_ins[i].before_move(); 
}

void MoveWorker::after_move() {
    for (int i=1;i<=len;i++) {
        for (auto t: m_ins[i].todo_turn) {
            ins_pos[t] = i; 
        } 
        m_ins[i].after_move();
    }
}

int MoveWorker::move_ins_with_conflicts() {
    int flag = 0;
    int a_inter = 0, a_spec = 0, a_pass = 0;
    for (int i=1;i<=len;i++) {
        MachineWithPreDeploy &m = m_ins[i];
        //cout << m.ins_todo.size() <<endl;
        for (int t : m.ins_todo) { 
            if ( !m.n_ins_ids.count(t) && !m.inter_eval(t,false,true) ) {
                int ob = ob_pos[t];
                assert( ob != i );
                if (!m_ins[ob].spec_eval(t,true)) {
                    flag |= 1;
                    a_inter ++;
                }
                else if (!m_ins[ob].inter_eval(t,true)) {
                    flag |= 2;
                    a_spec++;
                }
                else {
                    m_ins[ob].add_instance(t);
                    m_ins[i].del_instance(t);
                    temp_results.push_back(make_pair(t,ob));
                    flag |= 4;
                    a_pass++;
                }
            }
        }
    }
    cout << a_inter << " " << a_spec << " " << a_pass << endl;
    return flag;
}

int MoveWorker::move_ins_with_conflicts_soft() {
    int flag = 0;
    int a_inter = 0, a_spec = 0, a_pass = 0;
    for (int i=len;i>0;i--) {
        MachineWithPreDeploy &m = m_ins[i];
        //cout << m.ins_todo.size() <<endl;
        for (int t : m.ins_todo) { 
            if ( !m.n_ins_ids.count(t) && !m.inter_eval(t,false,true) ) {
                int tmp =0, times;
                for ( times= 0; times <=(machine_resources_num-i)/10+1000; times ++ ) {
                    int ob = (rand()%len)+1;
                    if ( ob == i || ob == ob_pos[t] ) continue;
                    if ( m_ins[ob].inter_eval(t,false) && m_ins[ob].spec_eval(t,false)) {
                        m_ins[ob].add_instance(t);
                        m_ins[i].del_instance(t);
                        temp_results.push_back(make_pair(t,ob));
                        tmp = 1;
                        break;
                    }
                }
                //cout << times << " " << i << endl;
                if (tmp) a_pass ++;
                else a_inter ++;
            }
        }
    }
    cout << a_inter << " " << a_spec << " " << a_pass << endl;
    return a_inter>0;
}

int MoveWorker::move_ins_directly() {
    int flag = 0;
    int a_inter = 0, a_spec = 0, a_pass = 0;
    for (int i=1;i<=len;i++) {
        MachineWithPreDeploy &m = m_ins[i];
        //cout << m.ins_todo.size() <<endl;
        for (int t : m.ins_todo) { 
            if ( !m.n_ins_ids.count(t) && m.inter_eval(t,false,true) ) {
                int ob = ob_pos[t];
                if (!m_ins[ob].spec_eval(t,true)) {
                    flag |= 1;
                    a_inter ++;
                }
                else if (!m_ins[ob].inter_eval(t,true)) {
                    flag |= 2;
                    a_spec++;
                }
                else {
                    m_ins[ob].add_instance(t);
                    m_ins[i].del_instance(t);
                    temp_results.push_back(make_pair(t,ob));
                    flag |= 4;
                    a_pass++;
                }
            }
        }
    }
    cout << a_inter << " " << a_spec << " " << a_pass << endl;
    return flag;
}

int MoveWorker::move_ins_soft_cpu( int max_times , double cpu_th, bool show) {
    
    int flag = 0;
    int a_inter = 0, a_spec = 0, a_pass = 0;
    for (int i=1;i<=len;i++) {
        MachineWithPreDeploy &m = m_ins[i];
        //cout << m.ins_todo.size() <<endl;
        for (int t : m.ins_todo) { 
            if ( app_cpu_line[instance_apps[t]][0]>cpu_th && !m.n_ins_ids.count(t) && !m.spec_eval(t,false,true) ) {
                int tmp =0, times;
                for ( times= 0; times <=max_times; times ++ ) {
                    int ob = (rand()%len)+1;
                    if ( ob == i || ob == ob_pos[t] ) continue;
                    if ( m_ins[ob].inter_eval(t,false) && m_ins[ob].spec_eval(t,true)) {
                        m_ins[ob].add_instance(t);
                        m_ins[i].del_instance(t);
                        temp_results.push_back(make_pair(t,ob));
                        tmp = 1;
                        break;
                    }
                }
                //cout << times << " " << i << endl;
                if (tmp) a_pass ++;
                else {
                    if (show) cout << t << ":" << instance_apps[t] << endl;
                    a_inter ++;
                }
            }
        }
    }
    cout << a_inter << " " << a_spec << " " << a_pass << endl;
    return a_inter>0;
}

int MoveWorker::move_ins_soft( int max_times , double mem_th, bool show) {
    
    int flag = 0;
    int a_inter = 0, a_spec = 0, a_pass = 0;
    for (int i=1;i<=len;i++) {
        MachineWithPreDeploy &m = m_ins[i];
        //cout << m.ins_todo.size() <<endl;
        for (int t : m.ins_todo) { 
            if ( app_mem_line[instance_apps[t]][0]>mem_th && !m.n_ins_ids.count(t) && !m.spec_eval(t,false,true) ) {
                int tmp =0, times;
                for ( times= 0; times <=max_times; times ++ ) {
                    int ob = (rand()%len)+1;
                    if ( ob == i || ob == ob_pos[t] ) continue;
                    if ( m_ins[ob].inter_eval(t,false) && m_ins[ob].spec_eval(t,false)) {
                        m_ins[ob].add_instance(t);
                        m_ins[i].del_instance(t);
                        temp_results.push_back(make_pair(t,ob));
                        tmp = 1;
                        break;
                    }
                }
                //cout << times << " " << i << endl;
                if (tmp) a_pass ++;
                else {
                    if (show) cout << t << ":" << instance_apps[t] << endl;
                    a_inter ++;
                }
            }
        }
    }
    cout << a_inter << " " << a_spec << " " << a_pass << endl;
    return a_inter>0;
}

void MoveWorker::init(map<int,int> &pos, vector<int> &ins_mch) {
    before_move();
    set_object_pos( pos );
    set_base_pos( ins_mch );
    after_move();
}

double MoveWorker::compute_score() {
    double score =0;
    for (int i=1;i<=len;i++) {
        score += m_ins[i].compute_score();
    }
    return score;
}
