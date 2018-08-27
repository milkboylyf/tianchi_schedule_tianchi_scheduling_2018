#include "code.h"

bool constance_cmp( int &a, int &b ) {
    return app_mem_line[instance_apps[a]][45] > app_mem_line[instance_apps[b]][45];
}

bool running_mch[10000] ={} ;

void count_constance( vector<int> &pre_deploy ) {
    Code c(machine_resources_num);
    for (int i=1;i<=instance_deploy_num;i++) if (pre_deploy[i]!=-1) {
        c.move(i,pre_deploy[i],true);
    }
    for (int i=1;i<=machine_resources_num;i++) {
        //cout << i ;
        vector<int> instances ;
        for (auto t: c.m_ins[i].ins_ids) {
            instances.push_back(t);
        }
        sort(instances.begin(),instances.end(),constance_cmp);
        double cpu[ 100 ]={}, mem[ 100 ]={} ;
        int disk=0, P=0, M=0 ,PM=0;
        
        int j, ct =0;
        for (j=0;j<instances.size();j++) {
            int flag = 1;
            for (int t=0;t<time_len;t++) 
                if (cpu[t]+app_cpu_line[instance_apps[instances[j]]][t] > cpu_spec[i] *0.4)
                    flag = 0;
            for (int t=0;t<time_len;t++) 
                if (mem[t]+app_mem_line[instance_apps[instances[j]]][t] > mem_spec[i] *0.7)
                    flag = 0;
            if (disk+app_apply[instance_apps[instances[j]]] > disk_spec[i] *0.7)
                flag = 0;
            if (j==0&& disk_spec[i]>2000)flag =1;
            
            if (flag) {
                for (int t=0;t<time_len;t++) 
                    cpu[t] += app_cpu_line[instance_apps[instances[j]]][t];
                for (int t=0;t<time_len;t++) 
                    mem[t] += app_mem_line[instance_apps[instances[j]]][t];
                disk += app_apply[instance_apps[instances[j]]];
                instance_constance[instances[j]]=1;
                ct ++ ;
                //cout << " " << instances[j];
                if(ct>=4) break;
                running_mch[i]=1;
            }
        }
        //cout <<endl;
    }
}

    int Code::get_level( int ins ) {
        int ins_app = instance_apps[ins];
        
        if (instance_constance[ins]) return 100;
        
        //超大实例
        if ( app_apply[ins_app] > 500 ) return 4;                  
        if ( app_cpu_line[ins_app][0] >= 16 ) return 4;
        //if ( app_mem_line[ins_app][0] >= 16 ) return 100; 
        
        //特殊规格实例 
        //if ( app_apply[ins_app] ==167 ) return 90;
        
        
        //if ( (app_apply[ins_app] /10)%2 ) return 4;
        
        //大实例 
        //if ( app_inter_counter[ins_app] >300 && self_inter_num[ins_app]==1 ) return 3;
        //if ( app_apply[ins_app] >= 500 ) return 3;
        if ( app_cpu_line[ins_app][0] >= 16 ) return 3;
        if ( app_mem_line[ins_app][0] >= 16 ) return 3; 
        
        //中实例 
        //if ( app_apply[ins_app] >= 300 ) return 2;
        if ( app_max_cpu[ins_app] >= 4 ) return 2;
        if ( app_mem_line[ins_app][0] >= 5.4 ) return 2; 
        
        if ( app_max_cpu[ins_app] >= 2 ) return 1;
        
        //小实例 
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
    
    double Machine::compute_score() {
        score = 0.0;
        if (empty()) return 0.0;
        for (auto t : cpu ) {
            score += 1+(ins_ids.size()+1)*(exp( max(0.0,(double)t/cpu_spec[m_ids] - cst::b )) - 1) ;
        }
        return score;
    }
    
    void Machine::set_constant( int ins ) {
        assert(ins_ids.count(ins));
        constant.insert(ins); 
    }
    
    //各项属性不能超分 
    bool Machine::spec_eval( int ins_app, int no_inter ) {
         
        if ( disk_spec[m_ids] < disk + app_apply[ins_app]           
            || p_lim[m_ids] < P + app_p[ins_app]
            // || m_lim[m_ids] < M + app_m[ins_app]                 //there's no positive value in app_m 
            || pm_lim[m_ids] < PM + app_pm[ins_app] ) 
            return false;
        for (int i=0;i<time_len;i++) 
            if (  cpu_spec[m_ids] < cpu[i] + app_cpu_line[ins_app][i]  - 1e-8
                || mem_spec[m_ids] < mem[i] + app_mem_line[ins_app][i] - 1e-8 ) {
                    //cout << "CPU & MEM" <<endl;
            return false;
        }
        return true;
    }
    
    //查找干扰和反向干扰 
    bool Machine::inter_eval( int ins_app, int no_inter  ) {
        if (app_inter_set.count(ins_app))                           
        for (auto &t:app_inter_set[ins_app])
            if ( apps.count(t.first) && (apps.count(ins_app) ? t.second<=apps[ins_app] : !t.second ) ) {
                //if ( no_inter) 
                //    cout << "INTERFERENCE " << t.first << " " << ins_app << " " << t.second << endl;
                return false;
        }
        if (app_rvs_inter_set.count(ins_app)) 
        for (auto &t:app_rvs_inter_set[ins_app])
            if ( apps.count(t.first) && t.second<apps[t.first] ) {
                //if ( no_inter )
                //    cout << "INTERFERENCE2 " << t.first << " " << ins_app << " " << t.second << endl;
                return false;
        }
        return true;
    }
    
    bool Machine::check_all_inter() {
        for (auto it : apps ) {                        
            for (auto &t:app_inter_set[it.first]) if (apps.count(t.first) && t.second<it.second) {
                return false;
            }
        }
        return true;
    }
    
    bool Machine::add_instance( int ins, int no_inter ) {
        
        
        int ins_app = instance_apps[ins];
        
        if (constant.count(ins)) return false;                      //静态实例，不能移动 
         
        if (!spec_eval(ins_app,no_inter)) return false;                      //各项属性不能超分 
        
        if (!inter_eval(ins_app,no_inter )) {
            if (!no_inter)
                return false;                     //查找干扰和反向干扰 
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
        
        return true;
    }
    
    bool Machine::check_cpu_overload(int ins) {
        int ins_app = instance_apps[ins];
        double overload = 0, threshod = disk_spec[m_ids]>40?53:5;
        for (int i=0;i<time_len;i++) {
            double t = cpu[i] + app_cpu_line[ins_app][i];
            overload += cst::a*exp( max(0.0,t/cpu_spec[m_ids] - cst::b )) - 10 ;
            if ( overload > threshod ) return true;
        }
        
        return overload > threshod;
    }
    
    bool Machine::del_instance( int ins ) {
        int ins_app = instance_apps[ins];
        if (constant.count(ins)) return false;
        
        ins_ids.erase(ins);
        --apps[ins_app];
        if (apps[ins_app]==0) apps.erase(ins_app);
        
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
    
    void Machine::print() {
        cout << disk << " " << P << " " << M << " " << PM << " " << m_ids << endl;
        for (auto t: ins_ids ) {
            cout << t << ":" << instance_apps[t] << " ";
        }
        cout << endl;
    }
    
    
    Code::Code(int _len) :len(_len),u_score(0),max_ins_pair(0),
                            sim_disk_spec({0,40,60,80,100,120,150,167,180,200,250,300,500,600,650,1000,1024}) {
        for (int i=0;i<=_len;i++) m_ins.push_back(Machine(i));
        for (int i=0;i<sim_disk_spec.size();i++) {
            //cout << i <<endl;
            disk_index[sim_disk_spec[i]]=i;
            ins_remain.push_back(0);
        }
        for (int i=1;i<=instance_deploy_num;i++ ) {
            int tmp_spec = app_apply[instance_apps[i]];
            //cout << i <<" " << tmp_spec <<endl;
            ins_remain[disk_index[tmp_spec]]++;
        }
    }
    
    void Code::reset() {
        for (int i=1;i<=len;i++) m_ins[i].clear();
        running.clear();
        u_score = 0;
        ins_pos.clear(); 
    }
    
    void Code::init() {
        int ct = machine_resources_num;
        int index[instance_deploy_num+2];
        //count_constance(instance_machines);
        vector<int> mi;
        for (int i=1;i<=machine_resources_num;i++) if (!running_mch[i]) mi.push_back(i);
        cout << mi.size() <<endl;
        for (int i=1;i<=machine_resources_num;i++) if (running_mch[i]) mi.push_back(i);
        index[0]=0;
        
        //*
        for (int i=0;i<sim_disk_spec.size();i++) {
            cout << ins_remain[i] << ":" << sim_disk_spec[i] << " " ;
        }
        cout << endl;
        //*/
        //srand(37124); 
        //随机化实例序列，经测试影响不大 
        for (int i=1;i<=instance_deploy_num;i++) index[i]=i;
        //for (int i=1;i<=instance_deploy_num;i++) swap(index[i],index[rand()%i+1]);    
        set<int> v; 
        for (int i=1;i<=instance_deploy_num;i++) {assert(v.count(index[i])==0);v.insert(index[i]);}
        
        //放置一些过大的实例，这些实例是每个服务器只有一个的，不能移动 
        int t_ins_num = 0;
        for (int i=1;i<=instance_deploy_num;i++) 
            if ( get_level(index[i])==100 )
        {
            //*
            //instance_constance[index[i]] = 1;
            
            if (!m_ins[instance_machines[index[i]]].add_instance(index[i],true)) {
                cout << index[i] << " " << instance_apps[index[i]] << endl;
                exit(0);
            }
            running.insert(instance_machines[index[i]]);
            ins_pos[index[i]] = instance_machines[index[i]];
            ins_remain[disk_index[app_apply[instance_apps[index[i]]]]]--;
            //if (m_ins[instance_machines[index[i]]].compute_score() > 98 ) 
            //    cout << m_ins[instance_machines[index[i]]].compute_score() << endl;
            /*/
            
            //cout << ct <<endl;
            assert(m_ins[ct].add_instance(index[i]));
            //m_ins[ct].set_constant(index[i]);
            running.insert(ct);
            ins_pos[index[i]] = ct--;
            ins_remain[disk_index[app_apply[instance_apps[index[i]]]]]--;
            //*/
        }
        
        //放置disk=167的实例，每个服务器放俩，用于将一些1024的服务器填满 
        for (int i=1;i<=instance_deploy_num;i++) 
            if ( get_level(index[i])==90 && ins_pos.count(index[i])==0 )
        {
            int tmp_m_i = machine_resources_num+1, tmp_m;
            do {
                tmp_m_i --;
                assert(tmp_m_i>0);
                tmp_m = mi[tmp_m_i];
            }
            while ( (disk_spec[tmp_m]-m_ins[tmp_m].disk) % 10 == 0 
                || (m_ins[tmp_m].empty()==0 && (m_ins[tmp_m].cpu[0]+app_max_cpu[instance_apps[index[i]]])*1.9 > cpu_spec[tmp_m] )
                || !m_ins[tmp_m].add_instance(index[i]) );
            if (index[i]%1000==0)
                std::cout<< index[i]<<" " <<tmp_m<<std::endl;
            running.insert(tmp_m);
            ins_pos[index[i]] = tmp_m;
            ins_remain[disk_index[app_apply[instance_apps[index[i]]]]]--;
        }
        
        //按等级放置实例，越大的等级，服务器规格越大 
        for (int level=4;level>=0;level--) {
            for (int i=1;i<=instance_deploy_num;i++) 
                if ( get_level(index[i])==level && ins_pos.count(index[i])==0)
            {
                //cout << index[i] << " " << instance_apps[index[i]] << endl;
                int tmp_m_i = machine_resources_num+1, tmp_m;
                do {
                    tmp_m_i --;
                    assert(tmp_m_i>0);
                    tmp_m = mi[tmp_m_i];
                }
                while ( 0
                        //|| ( level == 4 && m_ins[tmp_m].disk >= 600 && (m_ins[tmp_m].disk/10)%2 == 0 )
                        //|| ( index[i] < 20000 && m_ins[tmp_m].disk+app_apply[instance_apps[index[i]]] +40 > disk_spec[tmp_m] && m_ins[tmp_m].disk+app_apply[instance_apps[index[i]]]  < disk_spec[tmp_m] -5 )
                        //|| ( index[i]>= 20000 && index[i] < 67600 && m_ins[tmp_m].disk+app_apply[instance_apps[index[i]]] +60 > disk_spec[tmp_m] && m_ins[tmp_m].disk+app_apply[instance_apps[index[i]]]  < disk_spec[tmp_m] -15 )
                        //|| (m_ins[tmp_m].empty()==0 && m_ins[tmp_m].check_cpu_overload(index[i]) ) 
                        || (m_ins[tmp_m].empty()==0 && (m_ins[tmp_m].cpu[0]+app_max_cpu[instance_apps[index[i]]])*(1.91+(level==4)*0.1) > cpu_spec[tmp_m] ) //level1_mem = 5310,1.973极限低分 level1_mem = 5400,2.01易交换  
                        //|| (((double)cpu_spec[tmp_m]/2- m_ins[tmp_m].cpu[0])/(disk_spec[tmp_m] - m_ins[tmp_m].disk)*5< (double)app_cpu_line[instance_apps[index[i]]][0] /app_apply[instance_apps[index[i]]])
                        || !m_ins[tmp_m].add_instance(index[i]) );
                if (i%1000==0)
                    std::cout<< i<<" " <<tmp_m<<std::endl;
                running.insert(tmp_m);
                ins_pos[index[i]] = tmp_m;
                ins_remain[disk_index[app_apply[instance_apps[index[i]]]]]--;
            }
        }
            
        for (auto m_id: running) {
            double t_s = m_ins[m_id].compute_score();
            //if (t_s > 300 ) 
            //cout<< t_s <<endl;
            u_score += t_s;
        }
        //cout << "score:" << u_score << endl; 
        
        if (move_last_instance()) cout << " exchange complete. "<< endl;
        cout << "End initialization." <<endl;
    }
    
    //移动一个实例至另一个machine，如果不能移动，则寻找二次交换 
    bool Code::move_last_instance() {
        accept();
        int ins_obj=-1, m_obj=-1;
        for (auto m :m_ins)
            if (m.disk == 60 ) ins_obj = *m.ins_ids.begin();
            else if (m.disk == 480) m_obj = m.m_ids;
        
        if (ins_obj == -1 || m_obj == -1) return false;
        if (move(ins_obj,m_obj)) return true;
        
        //return false;
        
        while (true) {
            recover();
            int tmp_ins , ins_app;
            do {
                tmp_ins = rand()%instance_deploy_num+1;
                ins_app = instance_apps[tmp_ins];
            }
            while ( app_apply[ins_app] != 60 || ins_pos[tmp_ins] == m_obj );
            int tmp_m = ins_pos[tmp_ins];
            if (!move( tmp_ins , m_obj )) continue;
            if (!move( ins_obj , tmp_m )) continue;
            accept();
            return true;
        }
    }
    
    bool Code::exchange() {
        accept();
        int ins_a, pos_a, app_a;
        do {
            ins_a = rand()%instance_deploy_num+1;
            pos_a = ins_pos[ins_a];
            app_a = instance_apps[ins_a];
        }
        while ( m_ins[pos_a].score <= 98 ) ;
        int ins_b , app_b , times = 0, pos_b ;
        do {
            ins_b = rand()%instance_deploy_num+1;
            app_b = instance_apps[ins_b];
            pos_b = ins_pos[ins_b];
            times ++;
        }
        while ( (pos_a==pos_b || app_apply[app_a] != app_apply[app_b] ) && times <200 );
        if (times ==200) return false;
        if (move( ins_a , 1 ) && move( ins_b , pos_a ) && move( ins_a , pos_b ) ) {
            //accept();
            return true;
        }
        //recover();
        return false;
    }
    
    bool Code::move_ins( int ins ) {
        //if (get_level(ins)==100) return 0;
        for (int i=0;i<10;i++) {
            int tmp_m = rand()%len+1;
            while (ins_pos[ins]==tmp_m) tmp_m = rand()%len+1;
            if (move(ins,tmp_m)) return true; 
        }
        return false;
    }
    
    //用于移动实例ins至服务器m，记录移动记录 
    bool Code::move(int ins, int tmp_m, int no_inter) {
        
        int tmp_i = ins, flag = 0;
        
        assert( ins_pos[ins]!=tmp_m && tmp_m!=0 && tmp_m <=len );
        
        //int tmp_m= 0;
        //for (int times = 0;times < 10;times ++ ) {
        //    tmp_m = rand()%len+1;
    
        u_score -= m_ins[tmp_m].compute_score();
        if ( tmp_m == ins_pos[tmp_i] || !m_ins[tmp_m].add_instance(tmp_i,no_inter) ) {
            u_score += m_ins[tmp_m].compute_score();
            return false;
        }
        u_score += m_ins[tmp_m].compute_score();
        running.insert(tmp_m);
        
        if (ins_pos[tmp_i]) {
            //assert(no_inter==0);
            u_score -= m_ins[ins_pos[tmp_i]].compute_score();
            bool del_successfully = m_ins[ins_pos[tmp_i]].del_instance(tmp_i);
            assert(del_successfully);
            u_score += m_ins[ins_pos[tmp_i]].compute_score();
            if (m_ins[ins_pos[tmp_i]].empty()) running.erase(ins_pos[tmp_i]);
        }
        else ins_remain[disk_index[app_apply[instance_apps[ins]]]]--;
        
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
            u_score -= m_ins[ins_pos[moving_ins_id]].compute_score();
            bool del_successfully = m_ins[ins_pos[moving_ins_id]].del_instance(moving_ins_id);
            assert(del_successfully);
            u_score += m_ins[ins_pos[moving_ins_id]].compute_score();
            if (m_ins[ins_pos[moving_ins_id]].empty()) running.erase(ins_pos[moving_ins_id]);
            
            if (moving_machine_id) {
                u_score -= m_ins[moving_machine_id].compute_score();
                bool add_successfully =  m_ins[moving_machine_id].add_instance(moving_ins_id);
                //cerr << moving_ins_id << " " << instance_apps[moving_ins_id] << endl;
                //m_ins[moving_machine_id].print();
                assert(add_successfully);
                u_score += m_ins[moving_machine_id].compute_score();
                running.insert(moving_machine_id);
            }
            else ins_remain[disk_index[app_apply[instance_apps[moving_ins_id]]]]++;
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
        int counter = 0;
        cout.precision(20);
        for (auto &t :m_ins) 
            if (!t.empty())
        {
            instance_num += t.ins_ids.size();
            if (!t.empty())min_machine_score = min(min_machine_score,t.compute_score());
            double max_c = 0;
            double cpu_over=0;
            for (auto value : t.cpu ) {all_cpu+= value;max_c=max(max_c,value);cpu_over+=cst::a*exp(max(0.0,value/cpu_spec[t.m_ids]-cst::b))-10;}
            if (max_c / cpu_spec[t.m_ids]>0.5) ++overload_num;
            max_cpu+= max_c;
            double max_m = 0;
            for (auto value : t.mem ) {all_mem+= value;max_m=max(max_m,value);}
            max_mem+= max_m;
            all_disk+= t.disk;
            cpu_limit+= cpu_spec[t.m_ids];
            mem_limit+= mem_spec[t.m_ids];
            disk_limit+= disk_spec[t.m_ids];
            
            //* 
            set<int> inter_limit;
            for (auto l : t.apps ) {
                int ins_app = l.first;
                if (app_inter_set.count(ins_app))                           //查找干扰和反向干扰 
                for (auto &lt:app_inter_set[ins_app])
                    if ( t.apps.count(lt.first) && t.apps[ins_app]==lt.second ) 
                        inter_limit.insert(ins_app);
                    else if ( lt.second < t.apps[ins_app] )
                        inter_limit.insert(lt.first);
                if (app_rvs_inter_set.count(ins_app)) 
                for (auto &lt:app_rvs_inter_set[ins_app])
                    if ( !lt.second || (t.apps.count(lt.first) && lt.second==t.apps[lt.first]) ) {
                            //cout << "INTERFERENCE" << lt.first << " " << ins_app << endl;
                        inter_limit.insert(lt.first);
                    }
            }
            
            //if (t.disk-disk_spec[t.m_ids]<=-60) {
            if ((double)max_c / cpu_spec[t.m_ids]>0.5) {
            //if (t.compute_score() > 100) {
            //if ((double)max_m / mem_spec[t.m_ids]>0.8) {
            //if (inter_limit.size()>700) {
            //if (t.m_ids >= 730 && t.m_ids <=740 ){
                ++counter;
                /*
                cout << endl << "machine " << t.m_ids << " : " << t.disk << "-" << disk_spec[t.m_ids] 
                    << "\tcpu: " << max_c / cpu_spec[t.m_ids] << " " << cpu_over << endl
                    << "\tmem: " << max_m / mem_spec[t.m_ids] 
                    << "\tsize: " << t.ins_ids.size() 
                    << "\tinter limit: " << inter_limit.size() << endl;
                    for (auto ins : t.ins_ids) cout << instance_apps[ins] << " ";
                    cout << endl;
                    */
                
            }
            //*/
        }
        //cout << endl << "machines in cond: " << counter;
        cout << "machines_num :" << running.size() << " u_score:" << u_score
            << " delta_score:" << ave_score() << " min_score:" << min_machine_score 
            << " time:" << (double)clock()/CLOCKS_PER_SEC //<< endl;
            << " max_cpu:" << max_cpu/cpu_limit << " all_cpu:" << all_cpu/time_len/cpu_limit 
            << " max_mem:" << max_mem/mem_limit << " all_mem:" << all_mem/time_len/mem_limit
            << " all_disk:" << (double)all_disk/disk_limit << " overload_num:" << overload_num << endl;
        if ( instance_num < 68219 ) {
            cout << moving_machine_id << " " << moving_ins_id << " " << instance_apps[moving_ins_id] << " " << ins_pos[moving_ins_id] << endl;
            m_ins[moving_machine_id].print();
            m_ins[ins_pos[moving_ins_id]].print();
            exit(0);
        }
        //cout << max_ins_pair <<endl;
    }
    
    //打印各类硬盘剩余情况 
    void Code::show_extra_info () {
        
    	map<int,int> disk_space; 
    	int space_remain = 0;
    	for (auto m : m_ins) {
    	   disk_space[m.disk]++;
    	   space_remain += disk_spec[m.m_ids]-m.disk; 
    	   
        }
    	for (auto d:disk_space) 
    	   cout << d.first << ":" << d.second <<endl;
    }
    
    double Code:: ave_score() {
        //u_score = recalculate_score();
        return u_score / time_len;
    }
    
    double Code:: recalculate_score() {
        u_score = 0;
        for(auto mch:m_ins) u_score+= mch.compute_score();
        return u_score;
    }
    
    void Code::restore_pos( map <int,int> &r_pos) {
        for (auto t:r_pos) {
            assert(move(t.first,t.second));
        }
        recalculate_score();
    }
    
