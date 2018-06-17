#include "code.h"

inline int constant_ins( int ins ) {
    int ins_app = instance_apps[ins];
    
    
    if ( app_apply[ins_app] >= 600 ) return 2;
    if ( app_cpus[ins_app][0] >= 46000 ) return 2;
    if ( app_mems[ins_app][0] >= 144000 ) return 2; 
    
    if ( app_apply[ins_app] >= 400 ) return 1;
    if ( app_cpus[ins_app][0] >= 16000 ) return 1;
    if ( app_mems[ins_app][0] >= 40000 ) return 1; 
    return false;
}

Machine::Machine(int ids):m_ids(ids),disk(0),P(0),M(0),PM(0) {
        for (int i=0;i<time_len;i++) cpu.push_back(0); 
        for (int i=0;i<time_len;i++) mem.push_back(0); 
    }
    
    bool Machine::empty() {
        return ins_ids.empty();
    }
    
    double Machine::score() {
        double s = 0.0;
        if (empty()) return 0.0;
        for (auto t : cpu ) {
            s += cst::a*exp( max(0.0,(double)t/cpu_spec[m_ids] - cst::b )) - 9 ;
        }
        return s;
    }
    
    void Machine::set_constant( int ins ) {
        assert(ins_ids.count(ins));
        constant.insert(ins); 
    }
    
    bool Machine::add_instance(int ins) {
        
        
        int ins_app = instance_apps[ins];
        if ( m_ids == 951 && ins == 7682) {
            //cout << ins << " " << ins_app << " " ; 
            //print();
        }
        if (constant.count(ins)) return false;
        if ( disk_spec[m_ids] < disk + app_apply[ins_app] 
            || p_lim[m_ids] < P + app_p[ins_app]
            // || m_lim[m_ids] < M + app_m[ins_app]                 //there's no positive value in app_m 
            || pm_lim[m_ids] < PM + app_pm[ins_app] ) 
            return false;
        for (int i=0;i<time_len;i++) 
            if (  cpu_spec[m_ids] < cpu[i] + app_cpus[ins_app][i] 
                || mem_spec[m_ids] < mem[i] + app_mems[ins_app][i] )
            return false;
        
        if (app_inter_set.count(ins_app)) 
        for (auto &t:app_inter_set[ins_app])
            if ( apps.count(t.first)&& (apps.count(ins_app)? t.second<=apps[ins_app]:!t.second) )
            return false;
        if (app_rvs_inter_set.count(ins_app)) 
        for (auto &t:app_rvs_inter_set[ins_app])
            if ( apps.count(t.first) && t.second<apps[t.first] )
            return false;
            
        ins_ids.insert(ins);
        apps[ins_app]++;
        
        for (int i=0;i<time_len;i++) {
            cpu[i] += app_cpus[ins_app][i];
            mem[i] += app_mems[ins_app][i];
        }
        disk += app_apply[ins_app] ;
        P += app_p[ins_app];
        //M += app_m[ins_app];                 //there's no positive value in app_m 
        PM += app_pm[ins_app]; 
        
        if ( m_ids == 3093) {
            //cout << ins << " " << ins_app; 
            //print();
        }
        
        return true;
    }
    
    bool Machine::del_instance( int ins ) {
        int ins_app = instance_apps[ins];
        if (constant.count(ins)) return false;
        
        ins_ids.erase(ins);
        --apps[ins_app];
        if (apps[ins_app]==0) apps.erase(ins_app);
        
        for (int i=0;i<time_len;i++) {
            cpu[i] -= app_cpus[ins_app][i];
            mem[i] -= app_mems[ins_app][i];
        }
        disk -= app_apply[ins_app] ;
        P -= app_p[ins_app];
        //M -= app_m[ins_app];                 //there's no positive value in app_m 
        PM -= app_pm[ins_app]; 
        return true;
    }
    
    void Machine::print() {
        cout << disk << " " << P << " " << M << " " << PM << " " << m_ids << endl;
        for (auto t: ins_ids ) {
            cout << instance_apps[t] << " ";
        }
        cout << endl;
    }
    
    
    Code::Code(int _len) :len(_len),u_score(0) {
        for (int i=0;i<=_len;i++) m_ins.push_back(Machine(i));
    }
    
    void Code::init() {
        int ct = 3001;
        for (int i=1;i<=instance_deploy_num;i++) 
            if ( constant_ins(i)==2 )
        {
            //cout << ct <<endl;
            m_ins[ct].add_instance(i);
            m_ins[ct].set_constant(i);
            running.insert(ct);
            ins_pos[i] = ct++;
        }
        
        for (int i=1;i<=instance_deploy_num;i++) 
            if ( constant_ins(i) && ins_pos.count(i)==0)
        {
            int tmp_m = machine_resources_num+1;
            do {
                tmp_m --;//= rand()%len;
            }
            while ( m_ins[tmp_m].cpu[0]*2 >= cpu_spec[tmp_m] || !m_ins[tmp_m].add_instance(i) );
            //if (i%100==0)
            //std::cout<< i<<" " <<tmp_m<<std::endl;
            running.insert(tmp_m);
            ins_pos[i] = tmp_m;
        }
        
        for (int i=1;i<=instance_deploy_num;i++) 
            if (ins_pos.count(i)==0)
        {
            int tmp_m= machine_resources_num+1;
            do {
                tmp_m --;//= rand()%len;
            }
            while (  m_ins[tmp_m].cpu[0]*2 >= cpu_spec[tmp_m] || !m_ins[tmp_m].add_instance(i) );
            if (i%1000==0)
            std::cout<< i<<" " <<tmp_m<<std::endl;
            running.insert(tmp_m);
            ins_pos[i] = tmp_m;
        }
        for (auto m_id: running) {
            double t_s = m_ins[m_id].score();
            if (t_s > 300 ) 
            cout<< t_s <<endl;
            u_score += t_s;
        }
        running_num = running.size(); 
        cout << "score:" << u_score << endl; 
    }
    
    int Code::move(int ins) {
        
        if (constant_ins(ins)) return 0;
        int tmp_i = ins, flag = 0;
        
        int tmp_m= 0;
        for (int times = 0;times < 10;times ++ ) {
            tmp_m = rand()%len+1;
            u_score -= m_ins[tmp_m].score();
            if ( tmp_m != ins_pos[tmp_i] && m_ins[tmp_m].add_instance(tmp_i) ) {
                flag = 1;
                break;
            }
            u_score += m_ins[tmp_m].score();
        }
        if(!flag)return 0;
        u_score -= m_ins[ins_pos[tmp_i]].score();
        bool del_successfully = m_ins[ins_pos[tmp_i]].del_instance(tmp_i);
        assert(del_successfully);
        u_score += m_ins[ins_pos[tmp_i]].score();
        u_score += m_ins[tmp_m].score();
        if (m_ins[ins_pos[tmp_i]].empty()) running.erase(ins_pos[tmp_i]);
        running.insert(tmp_m);
        moving_ins_id = tmp_i;
        moving_machine_id = ins_pos[tmp_i];
        move_log.push(make_pair(moving_ins_id,moving_machine_id));
        ins_pos[tmp_i]=tmp_m;
        flag = 1;
        //cerr << "end move:" << m_ins[ins_pos[ins]].ins_ids.size() << " --- " << m_ins[moving_machine_id].ins_ids.size() <<endl;
        return tmp_m;
    }
    
    bool Code::add_machine() {
    }
    
    bool Code::del_machine() {
    }
    
    void Code::recover() {
        //cerr << "start recover:" << m_ins[ins_pos[moving_ins_id]].ins_ids.size() << " --- " << m_ins[moving_machine_id].ins_ids.size()<<endl;
        while ( !move_log.empty() ) {
            moving_ins_id = move_log.top().first;
            moving_machine_id = move_log.top().second;
            move_log.pop();
            u_score -= m_ins[ins_pos[moving_ins_id]].score();
            u_score -= m_ins[moving_machine_id].score();
            bool add_successfully =  m_ins[moving_machine_id].add_instance(moving_ins_id);
            //cerr << moving_ins_id <<endl;
            //m_ins[moving_machine_id].print();
            assert(add_successfully);
            bool del_successfully = m_ins[ins_pos[moving_ins_id]].del_instance(moving_ins_id);
            assert(del_successfully);
            u_score += m_ins[ins_pos[moving_ins_id]].score();
            u_score += m_ins[moving_machine_id].score();
            if (m_ins[ins_pos[moving_ins_id]].empty()) running.erase(ins_pos[moving_ins_id]);
            running.insert(moving_machine_id);
            //cerr << "end recover:" << m_ins[ins_pos[moving_ins_id]].ins_ids.size() << " --- " << m_ins[moving_machine_id].ins_ids.size()<<endl;
            ins_pos[moving_ins_id]=moving_machine_id;
        }
    }
    
    void Code::accept() {
        while (!move_log.empty()) move_log.pop();
    }
    
    void Code::show_status() {
        int instance_num = 0;
        double all_cpu = 0, max_cpu=0, cpu_limit = 0, all_mem =0, max_mem = 0, mem_limit = 0, all_disk = 0, disk_limit = 0;
        double min_machine_score = 10*98;
        for (auto &t :m_ins) {
            instance_num += t.ins_ids.size();
            if (!t.empty())min_machine_score = min(min_machine_score,t.score());
            int max_c = 0;
            for (auto value : t.cpu ) {all_cpu+= value;max_c=max(max_c,value);}
            max_cpu+= max_c;
            int max_m = 0;
            for (auto value : t.mem ) {all_mem+= value;max_m=max(max_m,value);}
            max_mem+= max_m;
            all_disk+= t.disk;
            cpu_limit+= cpu_spec[t.m_ids];
            mem_limit+= mem_spec[t.m_ids];
            disk_limit+= disk_spec[t.m_ids];
        }
        cout << "machines num :" << running.size() << " u_score:" << u_score
         << " delta_score:" << u_score / time_len << " instance num:" << instance_num << " min score:" << min_machine_score 
         << " max_cpu:" << max_cpu/cpu_limit << " all_cpu:" << all_cpu/time_len/cpu_limit << " max_mem:" << max_mem/mem_limit << " all_mem:" << all_mem/time_len/mem_limit
          << " all_disk:" << all_disk/disk_limit << endl;
         if ( instance_num < 68219 ) {
             cout << moving_machine_id << " " << moving_ins_id << " " << instance_apps[moving_ins_id] << " " << ins_pos[moving_ins_id] << endl;
             m_ins[moving_machine_id].print();
             m_ins[ins_pos[moving_ins_id]].print();
             exit(0);
         }
    }
    
    double Code:: ave_score() {
        return u_score / time_len;
    }
    
