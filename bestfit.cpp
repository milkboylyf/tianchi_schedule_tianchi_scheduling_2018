#include "bestfit.h"

inline bool cmp(int a, int b ) {
    //if (app_apply[instance_apps[b]]>500)
    if (app_inter_counter[instance_apps[b]]>=100&&app_inter_counter[instance_apps[a]]<100) return false;
    if (app_inter_counter[instance_apps[a]]>=100&&app_inter_counter[instance_apps[b]]<100) return true;
    return app_max_cpu[instance_apps[a]] > app_max_cpu[instance_apps[b]];
}

double m_cpu_score[7000] ={};
double cpu_left[7000][98] ={};

inline bool cmp2(int a, int b ) {
    return m_cpu_score[a]<m_cpu_score[b];
}

inline double cpu_similarities( vector<double> &icpu , double mcpu[] ) {
    double sumi = 0, summ = 0;
    double df = 0;
    for (int i=0;i<time_len;i++) {sumi+=icpu[i];summ+=max(mcpu[i]-icpu[i],0.0);}
    for (int i=0;i<time_len;i++) {
        if (icpu[i]>mcpu[i]) df += exp(icpu[i]-mcpu[i])*10000;
        else df += pow((mcpu[i]-icpu[i])/summ -sum_cpu_line[i]/sum_cpu,2);
        //df += pow((mcpu[i]-icpu[i]),2);
    }
    return df;
}

BestFit::BestFit(int _len):Code(_len){}


void BestFit::init() {
    int ct = 3001;
    int index[instance_deploy_num+2];
    index[0]=0;
    
    for (int i=1;i<=instance_deploy_num;i++) index[i]=i;
    sort(index+1,index+instance_deploy_num+1,cmp);
    
    int machine_use = 1;
    
    for (int i=machine_use;i<=machine_resources_num;i++) {
        for (int j=0;j<time_len;j++) cpu_left[i][j] = disk_spec[i]/2;
    }
    
    for (int i=1;i<=machine_resources_num;i++) m_cpu_score[i]=1e9;
    //machine_resources_num = 10;
    for (int ii=1;ii<=instance_deploy_num;ii++) {
        int id = index[ii];
        if (ii%100==0) 
            cout << "&" << ii << ":" << endl;
        int mid[machine_resources_num+2];
        for (int j=machine_use;j<=machine_resources_num;j++) {
            //cout << id << endl;
            m_cpu_score[j] = cpu_similarities( app_cpu_line[instance_apps[id]], cpu_left[j] );
            //cout << " " << m_cpu_score[j];
        }
        
        for (int i=1;i<=machine_resources_num;i++) mid[i]=i;
        sort(mid+1,mid+machine_resources_num+1,cmp2);
        int tmp_m = 0;
        
        for (int i = 1;i<=machine_resources_num;i++) {
            tmp_m = mid[i];
            //if ( ii == 15 )
            //    cout << tmp_m <<endl;
            if (m_ins[tmp_m].add_instance(id) )
            {
                running.insert(tmp_m);
                ins_pos[id] = tmp_m;
                ins_remain[disk_index[app_apply[instance_apps[id]]]]--;
                for (int j=0;j<time_len;j++) cpu_left[tmp_m][j]-=app_cpu_line[instance_apps[id]][j];
                //cout << tmp_m << endl;
                break;
            }
            else {
                if (i == machine_resources_num) {
                    cout << id << " " << instance_apps[id] <<endl;
                    m_ins[mid[1]].print();
                    system("pause");
                }
            }
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
