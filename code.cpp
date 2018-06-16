#ifndef _CODE
#define _CODE
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
        for (auto t:app_inter_set[ins_app])
            if (apps.count(t.first) && t.second<=apps[ins_app])
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
        
        return true;
    }
    
    bool Machine::del_instance( int ins ) {
        int ins_app = instance_apps[ins];
        if (constant.count(ins)) return false;
        
        ins_ids.erase(ins);
        if (!(--apps[ins_app])) apps.erase(ins_app);
        
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
            int tmp_m = 0;
            do {
                tmp_m ++;//= rand()%len;
            }
            while ( !m_ins[tmp_m].add_instance(i) );
            //if (i%100==0)
            //std::cout<< i<<" " <<tmp_m<<std::endl;
            running.insert(tmp_m);
            ins_pos[i] = tmp_m;
        }
        
        for (int i=1;i<=instance_deploy_num;i++) 
            if (ins_pos.count(i)==0)
        {
            int tmp_m= 0;
            do {
                tmp_m ++;//= rand()%len;
            }
            while ( !m_ins[tmp_m].add_instance(i) );
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
    
    bool Code::move(int ins) {
        
        int tmp_i = ins, flag = 0;
        
        int tmp_m= 0;
        for (int times = 0;times < 10;times ++ ) {
            tmp_m = rand()%len+1;
            u_score -= m_ins[tmp_m].score();
            if ( m_ins[tmp_m].add_instance(tmp_i) ) {
                flag = 1;
                break;
            }
            u_score += m_ins[tmp_m].score();
        }
        if(!flag)return false;
        u_score -= m_ins[ins_pos[tmp_i]].score();
        m_ins[ins_pos[tmp_i]].del_instance(tmp_i);
        u_score += m_ins[ins_pos[tmp_i]].score();
        u_score += m_ins[tmp_m].score();
        if (m_ins[ins_pos[tmp_i]].empty()) running.erase(ins_pos[tmp_i]);
        running.insert(tmp_m);
        moving_ins_id = tmp_i;
        moving_machine_id = ins_pos[tmp_i];
        ins_pos[tmp_i]=tmp_m;
        flag = 1;
        cerr << "end move:" << m_ins[ins_pos[ins]].ins_ids.size() << " --- " << m_ins[moving_machine_id].ins_ids.size() <<endl;
        return true;
        
    }
    
    void Code::recover() {
        cerr << "start recover:" << m_ins[ins_pos[moving_ins_id]].ins_ids.size() << " --- " << m_ins[moving_machine_id].ins_ids.size()<<endl;
        u_score -= m_ins[ins_pos[moving_ins_id]].score();
        u_score -= m_ins[moving_machine_id].score();
        bool add_successfully =  m_ins[moving_machine_id].add_instance(moving_ins_id);
        assert(add_successfully);
        bool del_successfully = m_ins[ins_pos[moving_ins_id]].del_instance(moving_ins_id);
        assert(del_successfully);
        u_score += m_ins[ins_pos[moving_ins_id]].score();
        u_score += m_ins[moving_machine_id].score();
        if (m_ins[ins_pos[moving_ins_id]].empty()) running.erase(ins_pos[moving_ins_id]);
        running.insert(moving_machine_id);
        cerr << "end recover:" << m_ins[ins_pos[moving_ins_id]].ins_ids.size() << " --- " << m_ins[moving_machine_id].ins_ids.size()<<endl;
        ins_pos[moving_ins_id]=moving_machine_id;
    }
    
    void Code::show_status() {
        cout << "machines num :" << running.size() << " u_score:" << u_score << " ave_score:" << u_score / running.size() / time_len << endl;
    }
    
#endif
