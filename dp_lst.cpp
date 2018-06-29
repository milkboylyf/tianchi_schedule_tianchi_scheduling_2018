#include "dp_lst.h"

bool InstanceData:: operator< ( const InstanceData &t ) const {
    if ( app_apply[app_id] == app_apply[t.app_id] ) {
        //if ( app_cpu_line[app_id][0] >= 16 || 
        if ( app_inter_map[app_id].count(app_id) && app_inter_map[t.app_id].count(t.app_id)) 
            return app_inter_map[app_id][app_id] < app_inter_map[t.app_id][t.app_id];
        if ( app_inter_map[app_id].count(app_id) ) return true;
        if ( app_inter_map[t.app_id].count(t.app_id)) return false;
        if (app_inter_counter[app_id]>50) return app_inter_counter[app_id] > app_inter_counter[t.app_id];
        if (app_inter_counter[t.app_id]>50) return false;
        return app_cpu_line[app_id][0] > app_cpu_line[t.app_id][0];
    }
    return app_apply[app_id] > app_apply[t.app_id];
}
InstanceData::InstanceData(int id, bool deployed)
    :ins_id(id),is_deployed(deployed),app_id(instance_apps[id]){}

DP::DP(int _len):Code(_len),u_score(0) {}

void DP::init() {
    for (int i=0;i<sim_disk_spec.size();i++) {
        disk_index[sim_disk_spec[i]]=i;
        ins_remain.push_back(0);
        data.push_back(vector<InstanceData>());
        disk_ins_set.push_back(set<int>());
        imposible_apps.push_back(set<int>());
    }
    for (int i=1;i<=instance_deploy_num;i++ ) {
        int tmp_spec = app_apply[instance_apps[i]];
        ins_remain[disk_index[tmp_spec]]++;
        data[disk_index[tmp_spec]].push_back(InstanceData(i,0));
    }
    for (int i=1;i<sim_disk_spec.size();i++) 
        sort(data[i].begin(),data[i].end());
}

int DP::dfs_disk_plans( int k, int space ) {
    //cout << k << " " << space <<endl;
    if (disk_plan_num[k][space]>=0) return disk_plan_num[k][space];
    if (k==1) return disk_plan_num[k][space]=1;
    disk_plan_num[k][space]=0;
    for (int i=0;i*sim_disk_spec[k]<=space;i++) disk_plan_num[k][space]+= dfs_disk_plans(k-1,space-i*sim_disk_spec[k]);
    return disk_plan_num[k][space];
}


int DP::dynamic_programming( int space) {
    memset(f,0,sizeof(f));
    memset(tr,0,sizeof(tr));
    int max_space =0;
    f[0]=1;
    for (int i=sim_disk_spec.size()-1;i>0;i--) {
        memset(v,0,sizeof(v));
        for (int j=sim_disk_spec[i];j<=space;j++) 
            if (!f[j]&&f[j-sim_disk_spec[i]]&&v[j-sim_disk_spec[i]]<ins_remain[i]&&i==2) 
                f[j]=1,v[j]=v[j-sim_disk_spec[i]]+1,tr[j]=i, max_space = max(max_space,j);
    }
    int now = max_space;
    while (now) {
        //cout << sim_disk_spec[tr[now]] << ":" << tr[now] << " " ;
        now -=  sim_disk_spec[tr[now]];
    }
    cout << endl;
    return max_space;
}

bool DP::dfs_deploy ( int m, int spec , int j) {
    if (j == 0 ) return true;
    for (int i=0;i<data[spec].size();i++) 
        if (!data[spec][i].is_deployed && !imposible_apps[spec].count(data[spec][i].app_id)) {
            if (move(data[spec][i].ins_id,m)) {
                //cout << data[spec][i].ins_id << " " << m << " " << j << endl;
                data[spec][i].is_deployed=1;
                disk_ins_set[spec].insert(data[spec][i].ins_id);
                ins_remain[spec]--;
                //if ( tr[j-sim_disk_spec[spec]]!= spec ) imposible_apps[tr[j-sim_disk_spec[spec]]].clear();
                if ( dfs_deploy ( m, tr[j-sim_disk_spec[spec]] , j-sim_disk_spec[spec]) ) return true;
                ins_remain[spec]++;
                imposible_apps[spec].insert(data[spec][i].app_id); 
                disk_ins_set[spec].erase(data[spec][i].ins_id);
                data[spec][i].is_deployed=0;
                recover(1);
            }
            //else recover(1);
        }
    return false;
}

void DP::package_up( int m, int max_space ) {
    cout << m << " " << max_space << endl;
    for (int i :ins_remain ) cout << i << " ";
    cout << endl; 
    imposible_apps.clear();
    if (!dfs_deploy( m, tr[max_space], max_space) ) {
        cout << "imposible to deploy instance" <<endl;
        throw 1;
    }
    accept();
}

void DP::dp_plan() {
    for (int i=0;i<=sim_disk_spec.size();i++)
    for (int j=0;j<2000;j++) disk_plan_num[i][j]=-1;
    cout << dfs_disk_plans(sim_disk_spec.size()-1,1024) << endl;
    for (int i=machine_resources_num;i>0;i--) {
        package_up(i,dynamic_programming(disk_spec[i]));
    }
}

int DP::get_next_ins( Machine &m ) {
    if (m.empty()) {
        //return get_first_ins( m ) ;
    }
    //if (m.disk 
}
