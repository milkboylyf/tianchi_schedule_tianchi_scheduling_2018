#include "code.h"

int Code::get_level( int ins ) {
    int ins_app = instance_apps[ins];
    
    
    if ( app_apply[ins_app] > 600 ) return 100;
    if ( app_cpus[ins_app][0] >= 16000 ) return 100;
    if ( app_mems[ins_app][0] >= 144000 ) return 100; 
    
    if ( app_apply[ins_app] ==167 ) return 90;
    
    if ( app_apply[ins_app] >= 500 ) return 2;
    if ( app_cpus[ins_app][0] >= 16000 ) return 2;
    if ( app_mems[ins_app][0] >= 16000 ) return 2; 
    
    if ( app_apply[ins_app] >= 300 ) return 1;
    if ( app_cpus[ins_app][0] >= 4000 ) return 1;
    if ( app_mems[ins_app][0] >= 5400 ) return 1; 
    //if ( app_apply[ins_app] == 167 ) return 1;
    return false;
}

Machine::Machine(int ids):m_ids(ids),disk(0),P(0),M(0),PM(0) {
        for (int i=0;i<time_len;i++) cpu.push_back(0); 
        for (int i=0;i<time_len;i++) mem.push_back(0); 
    }
    
    bool Machine::empty() {
        return ins_ids.empty();
    }
    
    void Machine::clear() {
        ins_ids.clear();
        apps.clear();
        for (int i=0;i<time_len;i++) cpu[i]=mem[i]=0; 
        disk = 0;
        P = M = PM = 0;
        constant.clear();
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
    
    bool Machine::spec_eval( int ins_app ) {
         
        if ( disk_spec[m_ids] < disk + app_apply[ins_app]           //各项属性不能超分 
            || p_lim[m_ids] < P + app_p[ins_app]
            // || m_lim[m_ids] < M + app_m[ins_app]                 //there's no positive value in app_m 
            || pm_lim[m_ids] < PM + app_pm[ins_app] ) 
            return false;
        for (int i=0;i<time_len;i++) 
            if (  cpu_spec[m_ids] < cpu[i] + app_cpus[ins_app][i] 
                || mem_spec[m_ids] < mem[i] + app_mems[ins_app][i] ) {
                    //cout << "CPU & MEM" <<endl;
            return false;
        }
        return true;
    }
    
    bool Machine::inter_eval( int ins_app ) {
        if (m_ids==0) return true;
        if (app_inter_set.count(ins_app))                           //查找干扰和反向干扰 
        for (auto &t:app_inter_set[ins_app])
            if ( apps.count(t.first) && (apps.count(ins_app) ? t.second<=apps[ins_app] : !t.second ) ) {
                    //cout << "INTERFERENCE" << t.first << " " << ins_app << endl;
            return false;
        }
        if (app_rvs_inter_set.count(ins_app)) 
        for (auto &t:app_rvs_inter_set[ins_app])
            if ( apps.count(t.first) && t.second<apps[t.first] ) {
                    //cout << "INTERFERENCE" << t.first << " " << ins_app << endl;
            return false;
        }
        return true;
    }
    
    bool Machine::add_instance(int ins) {
        
        
        int ins_app = instance_apps[ins];
        
        if (constant.count(ins)) return false;                      //静态实例，不能移动 
         
        if (!spec_eval(ins_app)) return false;
        
        if (!inter_eval(ins_app)) return false;
        
        ins_ids.insert(ins);
        apps[ins_app]++;
        
        for (int i=0;i<time_len;i++) {
            cpu[i] += app_cpus[ins_app][i];
            mem[i] += app_mems[ins_app][i];
        }
        disk += app_apply[ins_app] ;
        P += app_p[ins_app];
        M += app_m[ins_app];                 //there's no positive value in app_m 
        PM += app_pm[ins_app]; 
        
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
        M -= app_m[ins_app];                 //there's no positive value in app_m 
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
    
    void Code::reset() {
        for (int i=1;i<=len;i++) m_ins[i].clear();
        running.clear();
        u_score = 0;
        ins_pos.clear(); 
    }
    
    void Code::init() {
        int ct = 3001;
        int index[instance_deploy_num+2];
        index[0]=0;
        
        //随机化实例序列，经测试影响不大 
        for (int i=1;i<=instance_deploy_num;i++) index[i]=i;
        for (int i=1;i<=instance_deploy_num;i++) swap(index[i],index[rand()%i+1]);    
        set<int> v; 
        for (int i=1;i<=instance_deploy_num;i++) {assert(v.count(index[i])==0);v.insert(index[i]);}
        
        //放置一些过大的实例，这些实例是每个服务器只有一个的，不能移动 
        for (int i=1;i<=instance_deploy_num;i++) 
            if ( get_level(i)==100 )
        {
            //cout << ct <<endl;
            m_ins[ct].add_instance(i);
            m_ins[ct].set_constant(i);
            running.insert(ct);
            ins_pos[i] = ct++;
        }
        
        //放置disk=167的实例，每个服务器放俩，用于将一些1024的服务器填满 
        for (int i=1;i<=instance_deploy_num;i++) 
            if ( get_level(i)==90 && ins_pos.count(i)==0 )
        {
            int tmp_m = machine_resources_num+1;
            do {
                tmp_m --;
                assert(tmp_m>0);
            }
            while ( (disk_spec[tmp_m]-m_ins[tmp_m].disk) % 10 == 0 
                || (m_ins[tmp_m].empty()==0 && (m_ins[tmp_m].cpu[0]+app_cpus[instance_apps[i]][0])*2 > cpu_spec[tmp_m] )
                || !m_ins[tmp_m].add_instance(i) );
            if (i%1000==0)
                std::cout<< i<<" " <<tmp_m<<std::endl;
            running.insert(tmp_m);
            ins_pos[i] = tmp_m;
        }
        
        //按等级放置实例，越大的等级，服务器规格越大 
        for (int level=3;level>=0;level--) {
            for (int i=1;i<=instance_deploy_num;i++) 
                if ( get_level(i)==level && ins_pos.count(i)==0)
            {
                //cout << i << " " << instance_apps[i] << endl;
                int tmp_m = machine_resources_num+1;
                do {
                    tmp_m --;
                    assert(tmp_m>0);
                }
                while ( (m_ins[tmp_m].empty()==0 && (m_ins[tmp_m].cpu[0]+app_cpus[instance_apps[i]][0])*2.01 > cpu_spec[tmp_m] ) 
                        || !m_ins[tmp_m].add_instance(i) );
                if (i%1000==0)
                    std::cout<< i<<" " <<tmp_m<<std::endl;
                running.insert(tmp_m);
                ins_pos[i] = tmp_m;
            }
        }
            
        for (auto m_id: running) {
            double t_s = m_ins[m_id].score();
            if (t_s > 300 ) 
            cout<< t_s <<endl;
            u_score += t_s;
        }
        cout << "score:" << u_score << endl; 
    }
    
    //没想好怎么写 
    bool Code::exchange() {
        int ins_;
    }
    
    bool Code::move_ins( int ins ) {
        if (get_level(ins)==100) return 0;
        for (int i=0;i<10;i++) {
            int tmp_m = rand()%len+1;
            if (move(ins,tmp_m)) return true; 
        }
        return false;
    }
    
    //用于移动实例ins至服务器m，记录移动记录 
    bool Code::move(int ins, int tmp_m) {
        
        int tmp_i = ins, flag = 0;
        
        //int tmp_m= 0;
        //for (int times = 0;times < 10;times ++ ) {
        //    tmp_m = rand()%len+1;
        u_score -= m_ins[tmp_m].score();
        if ( tmp_m == ins_pos[tmp_i] || !m_ins[tmp_m].add_instance(tmp_i) ) {
            u_score += m_ins[tmp_m].score();
            return false;
        }
        u_score += m_ins[tmp_m].score();
        running.insert(tmp_m);
        
        if (ins_pos[tmp_i]) {
            u_score -= m_ins[ins_pos[tmp_i]].score();
            bool del_successfully = m_ins[ins_pos[tmp_i]].del_instance(tmp_i);
            assert(del_successfully);
            u_score += m_ins[ins_pos[tmp_i]].score();
            if (m_ins[ins_pos[tmp_i]].empty()) running.erase(ins_pos[tmp_i]);
        }
        
        moving_ins_id = tmp_i;
        moving_machine_id = ins_pos[tmp_i];
        move_log.push(make_pair(moving_ins_id,moving_machine_id));
        ins_pos[tmp_i]=tmp_m;
        flag = 1;
        //cerr << "end move:" << m_ins[ins_pos[ins]].ins_ids.size() << " --- " << m_ins[moving_machine_id].ins_ids.size() <<endl;
        return true;
    }
    
    bool Code::add_machine() {
    }
    
    bool Code::del_machine() {
    }
    
    //按照移动记录恢复len个记录 
    void Code::recover(int len) {
        //cerr << "start recover:" << m_ins[ins_pos[moving_ins_id]].ins_ids.size() << " --- " << m_ins[moving_machine_id].ins_ids.size()<<endl;
        for (int i=0;(len==0||i<len) && !move_log.empty();i++ ) {
            moving_ins_id = move_log.top().first;
            moving_machine_id = move_log.top().second;
            move_log.pop();
            u_score -= m_ins[ins_pos[moving_ins_id]].score();
            bool del_successfully = m_ins[ins_pos[moving_ins_id]].del_instance(moving_ins_id);
            assert(del_successfully);
            u_score += m_ins[ins_pos[moving_ins_id]].score();
            if (m_ins[ins_pos[moving_ins_id]].empty()) running.erase(ins_pos[moving_ins_id]);
            
            if (moving_machine_id) {
                u_score -= m_ins[moving_machine_id].score();
                bool add_successfully =  m_ins[moving_machine_id].add_instance(moving_ins_id);
                //cerr << moving_ins_id << " " << instance_apps[moving_ins_id] << endl;
                //m_ins[moving_machine_id].print();
                assert(add_successfully);
                u_score += m_ins[moving_machine_id].score();
                running.insert(moving_machine_id);
            }
            //cerr << "end recover:" << m_ins[ins_pos[moving_ins_id]].ins_ids.size() << " --- " << m_ins[moving_machine_id].ins_ids.size()<<endl;
            ins_pos[moving_ins_id]=moving_machine_id;
        }
    }
    
    //接受改动，清空移动记录 
    void Code::accept() {
        while (!move_log.empty()) move_log.pop();
    }
    
    void Code::show_status() {
        int instance_num = 0 , overload_num = 0;
        double all_cpu = 0, max_cpu=0, cpu_limit = 0, all_mem =0, max_mem = 0, mem_limit = 0, all_disk = 0, disk_limit = 0;
        double min_machine_score = 10*98;
        for (auto &t :m_ins) 
            if (!t.empty())
        {
            instance_num += t.ins_ids.size();
            if (!t.empty())min_machine_score = min(min_machine_score,t.score());
            int max_c = 0;
            for (auto value : t.cpu ) {all_cpu+= value;max_c=max(max_c,value);}
            if ((double)max_c / cpu_spec[t.m_ids]>0.5) ++overload_num;
            max_cpu+= max_c;
            int max_m = 0;
            for (auto value : t.mem ) {all_mem+= value;max_m=max(max_m,value);}
            max_mem+= max_m;
            all_disk+= t.disk;
            cpu_limit+= cpu_spec[t.m_ids];
            mem_limit+= mem_spec[t.m_ids];
            disk_limit+= disk_spec[t.m_ids];
        }
        cout.precision(10);
        cout << endl << endl << "machines num :" << running.size() << " u_score:" << u_score
            << " delta_score:" << u_score / time_len << " min score:" << min_machine_score << endl
            << " max_cpu:" << max_cpu/cpu_limit << " all_cpu:" << all_cpu/time_len/cpu_limit 
            << " max_mem:" << max_mem/mem_limit << " all_mem:" << all_mem/time_len/mem_limit
            << " all_disk:" << all_disk/disk_limit << " overload_num:" << overload_num << endl;
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
    
    
    double Code:: recalculate_score() {
        u_score = 0;
        for(auto mch:m_ins) u_score+= mch.score();
        return u_score;
    }
    
