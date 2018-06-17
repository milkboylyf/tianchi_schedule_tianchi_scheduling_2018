#include "dp.h"

bool InstanceData:: operator< ( const InstanceData &t ) const {
    if ( app_apply[ins_id] == app_apply[t.ins_id] ) {
        //if ( app_cpus[ins_id][0] >= 16 || 
        return app_cpus[ins_id][0] > app_cpus[t.ins_id][0];
    }
    return app_apply[ins_id] > app_apply[t.ins_id];
}
InstanceData::InstanceData(int id, bool deployed)
    :ins_id(id),is_deployed(deployed),app_id(instance_apps[id]){}

DP::DP():disk_spec({0,40,60,80,100,120,150,167,180,200,250,300,500,600,650,1000,1024}),u_score(0) {
    for (int i=0;i<=machine_resources_num;i++) m_ins.push_back(Machine(i));
}

void DP::init() {
    for (int i=0;i<disk_spec.size();i++) {
        disk_index[disk_spec[i]]=i;
        ins_remain.push_back(0);
        data.push_back(vector<InstanceData>({}));
    }
    for (int i=1;i<=instance_deploy_num;i++ ) {
        int tmp_spec = app_apply[instance_apps[i]];
        //disk_ins_set[tmp_spec].insert(i);
        ins_remain[disk_index[tmp_spec]]++;
        data[disk_index[tmp_spec]].push_back(InstanceData(i,0));
    }
    for (int i=1;i<disk_spec.size();i++) 
        sort(data[i].begin(),data[i].end());
}


void DP::dynamic_programming() {
    memset(f,0,sizeof(f));
    memset(tr,0,sizeof(tr));
    f[0]=1;
    for (int i=disk_spec.size()-1;i>0;i--) {
        memset(v,0,sizeof(v));
        for (int j=disk_spec[i];j<1200;j++) 
            if (!f[j]&&f[j-disk_spec[i]]&&v[j-disk_spec[i]]<ins_remain[i]) 
                f[j]=1,v[j]=v[j-disk_spec[i]]+1,tr[j]=i;
    }
}

void DP::package_up( int m ) {
    //for (int i=0;i<n;i++) {
        
    //}
}

void DP::dp_plan() {
    for (int i=1;i<machine_resources_num;i++) {
        dynamic_programming();
        package_up(i);
    }
}
