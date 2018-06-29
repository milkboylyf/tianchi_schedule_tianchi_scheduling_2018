#include "deployer.h"



AppData::AppData(int id):app_id(id){
    for (int i=0;i<10;i++) a[i]=0;
}

bool AppData::operator< ( const AppData &t ) const {
    for (int i=0;i<5;i++) if (a[i]!=t.a[i]) return a[i]>t.a[i];
    return false;
}

Deployer::Deployer(int _len):Code(_len),sim_disk_spec({0,40,60,80,100,120,150,167,180,200,250,300,500,600,650,1000,1024}),u_score(0) {}

void Deployer::init() {
    for (int i=0;i<sim_disk_spec.size();i++) {
        disk_index[sim_disk_spec[i]]=i;
        ins_remain.push_back(0);
        disk_ins_set.push_back(set<int>());
    }
    int ct = 3000;
    for (int i=0;i<=app_resources_num;i++) 
        app_ins_set.push_back(set<int>());
    for (int i=1;i<=instance_deploy_num;i++ ) {
        int tmp_spec = app_apply[instance_apps[i]];
        ins_remain[disk_index[tmp_spec]]++;
        disk_ins_set[disk_index[tmp_spec]].insert(i);
        app_ins_set[instance_apps[i]].insert(i);
        if (tmp_spec>=500) 
            set_ins(i,++ct);        //初始化大硬盘占用的实例 
    }
    dynamic_programming(1024);
}


int Deployer::dynamic_programming( int space) {
    memset(f,0,sizeof(f));
    int max_space =0;
    f[0]=1;
    for (int i=sim_disk_spec.size()-1;i>0;i--) {
        for (int j=sim_disk_spec[i];j<=space;j++) 
            if (!f[j]&&f[j-sim_disk_spec[i]]) 
                f[j]=1, max_space = max(max_space,j);
    }
    for (int i=sim_disk_spec.size()-1;i>0;i--) {
        for (int j=sim_disk_spec[i];j<=space;j++) 
            if (!f[j]&&f[j-sim_disk_spec[i]]) 
                f[j]=1, max_space = max(max_space,j);
    }
    /*
    int now = max_space;
    while (now) {
        //cout << sim_disk_spec[tr[now]] << ":" << tr[now] << " " ;
        now -=  sim_disk_spec[tr[now]];
    }
    cout << endl;
    */
    return max_space;
}


void Deployer::package_up() {
    for (int i=1;i<=machine_resources_num;i++) {
        imposible_apps.clear();
        if (i%100==0||i>7000) {
            for (int t :ins_remain ) cout << t << " ";
            cout << endl << recalculate_score() << endl; 
        }
        while (m_ins[i].disk<disk_spec[i]) {
            make_view(m_ins[i]);
            if (i>30000) {
                cout << m_ins[i].disk << " " << disk_spec[i] << endl;
                for (int t : m_ins[i].ins_ids ) cout << instance_apps[t] << " ";
                cout << endl; 
                cout << "view:"; 
            //for (AppData &d: app_view ) cout << d.app_id << " ";
            //cout << endl; 
            }
            int flag = 0;
            for (AppData &d: app_view) {
                int tmp_ins = *(app_ins_set[d.app_id].begin());
                if (set_ins(tmp_ins,i)) {
                    flag = 1;
                    break;
                }
                else 
                    cout << "failed to put app " << instance_apps[tmp_ins] << endl;
            }
            if (flag == 0) break;
        }
    }
        
    
    accept();
}

int Deployer::get_next_ins( Machine &m ) {
    if (m.empty()) {
        //return get_first_ins( m ) ;
    }
    //if (m.disk 
}

void Deployer::make_view (Machine &m) {
    app_view.clear();
    for (int i=1;i<app_resources_num;i++) {
        if (app_ins_set[i].empty()) continue;
        if (imposible_apps.count(i)
            &&imposible_apps[i]
                <= ( m.apps.count(i) ?m.apps[i]:0) ) continue;
        if (!f[disk_spec[m.m_ids]-m.disk-app_apply[i]] && !f[disk_spec[m.m_ids]-m.disk-app_apply[i]-4]) continue;
        if (!m.spec_eval(i)) continue;
        AppData tmp_a(i);
        //if (m.empty()) {
            tmp_a.a[0]=self_inter_num[i]? (app_inter_counter[i]*app_ins_set[i].size()/self_inter_num[i]):-app_inter_counter[i];
        /*}
        else {
            int overlap= 0, distinct = 0;
            for (auto &inter_i : app_inter_set[i]) {
                if ( (m.apps.count(i)?m.apps[i]:0)>=inter_i.second ) {
                    if (imposible_apps.count(inter_i.first))
                        overlap += 4;
                    else 
                        distinct += 4;
                }
                //else distinct += 4-min(inter_i.second,3);
            }
            for (auto &inter_i : app_rvs_inter_set[i]) {
                if ( imposible_apps.count(inter_i.first) ) {
                    if (imposible_apps[inter_i.first]<=inter_i.second)
                        overlap += 4-min( inter_i.second,3);
                    else 
                        distinct += imposible_apps[inter_i.first]-inter_i.second;
                }
                else distinct += 4-min(inter_i.second,3);
            }
            tmp_a.a[0]=overlap-distinct/4;
        }*/ 
        app_view.push_back(tmp_a);
    }
    sort(app_view.begin(),app_view.end());
}

void Deployer::update_imposible_apps( int t, int max_app) {
    if (imposible_apps.count(t)) 
        imposible_apps[t] = min(imposible_apps[t],max_app);
    else imposible_apps[t] = max_app;
}

bool Deployer::set_ins( int ins, int m , bool constant) {
    if (!move(ins,m)) return false;
    int ins_app = instance_apps[ins];
    ins_remain[disk_index[app_apply[ins_app]]]--;
    app_ins_set[ins_app].erase(ins);
    
    if (app_inter_set.count(ins_app))                           //查找干扰和反向干扰 
    for (auto &t:app_inter_set[ins_app])
        if ( m_ins[m].apps.count(t.first) ) 
            update_imposible_apps(ins_app,t.second);
        else if ( t.second < m_ins[m].apps[ins_app] )
            update_imposible_apps(t.first,0);
    if (app_rvs_inter_set.count(ins_app)) 
    for (auto &t:app_rvs_inter_set[ins_app])
        //if ( !t.second || (m_ins[m].apps.count(t.first) && t.second==m_ins[m].apps[t.first]) ) {
                //cout << "INTERFERENCE" << t.first << " " << ins_app << endl;
            update_imposible_apps(t.first,t.second);
        //}
    if (constant) 
        m_ins[m].set_constant(ins);
    return true;
}

