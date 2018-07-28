#include "csv.h"
#include "global.h"
#include "read.h"

using namespace std;

vector<double> parse_float_list(const string& s) {
    vector<double> res;
    int i = 0;
    while(i < int(s.size())) {
        int p = i + 1;
        while(p < int(s.size()) && s[p] != '|') {
            p++;
        }
        res.push_back(stod(s.substr(i, p - i)));
        i = p + 1;
    }
    return res;
}

void read_data(
        string instance_deploy_file,
        string app_resources_file,
        string machine_resources_file,
        string app_interference_file) {

    io::CSVReader<3> in0(instance_deploy_file);
    std::string string_buffers[10];
    int int_buffers[10];
    double double_buffers[10];
    while(in0.read_row(string_buffers[0], string_buffers[1], string_buffers[2]))  {
        global:: instance_ids.push_back(stoi(string_buffers[0].substr(5)));
        global:: instance_apps.push_back(stoi(string_buffers[1].substr(4)));
        if(string_buffers[2] != "") {
            global:: instance_machines.push_back(stoi(string_buffers[2].substr(8)));
        }
        else {
            global:: instance_machines.push_back(-1);
        }
        
    }
    //cerr << global:: instance_ids[0] << " " << global:: instance_apps[0] << " " << global:: instance_machines[2] << endl;
    cerr << "input instance_deploy_num = " << (global:: instance_deploy_num = global:: instance_ids.size() -1) << endl;

    io::CSVReader<7> in1(app_resources_file);
    while(in1.read_row(
            string_buffers[0], string_buffers[1], string_buffers[2], 
            double_buffers[3], int_buffers[4], int_buffers[5], int_buffers[6]))  {
        global:: app_ids.push_back(stoi(string_buffers[0].substr(4)));
        global:: app_cpu_line.push_back(parse_float_list(string_buffers[1]));
        global:: app_mem_line.push_back(parse_float_list(string_buffers[2]));
        global:: app_apply.push_back((int)double_buffers[3]);
        global:: app_p.push_back(int_buffers[4]);
        global:: app_m.push_back(int_buffers[5]);
        global:: app_pm.push_back(int_buffers[6]);
    }
    /*
    cerr << global:: app_ids[3] << " " << global:: app_cpu_line[3][3] << " " 
         << global:: app_mem_line[3][3] << " " 
         << global:: app_apply[0] << " " 
         << global:: app_p[3] << " " 
         << global:: app_m[3] << " " 
         << global:: app_pm[3] << endl;
    */
    cerr << "input app_resource_num = " << (global:: app_resources_num = global:: app_ids.size() -1) << endl;

    io::CSVReader<7> in2(machine_resources_file);
    while(in2.read_row(
            string_buffers[0], int_buffers[1], int_buffers[2], 
            int_buffers[3], int_buffers[4], int_buffers[5], int_buffers[6]))  {
        global:: machine_ids.push_back(stoi(string_buffers[0].substr(8)));
        global:: cpu_spec.push_back(int_buffers[1]);
        global:: mem_spec.push_back(int_buffers[2]);
        global:: disk_spec.push_back(int_buffers[3]);
        global:: p_lim.push_back(int_buffers[4]);
        global:: m_lim.push_back(int_buffers[5]);
        global:: pm_lim.push_back(int_buffers[6]);
    }
    /*
    cerr << global:: machine_ids[3] << " " 
         << global:: cpu_spec[3] << " " 
         << global:: mem_spec[3] << " " 
         << global:: disk_spec[3] << " " 
         << global:: p_lim[3] << " " 
         << global:: m_lim[3] << " " 
         << global:: pm_lim[3] << endl;
    */
    cerr << "input machine_resources_num = " << (global:: machine_resources_num = global:: machine_ids.size() -1) << endl;

    vector<int> app_inter1, app_inter2, app_inter_max;
    io::CSVReader<3> in3(app_interference_file);
    while(in3.read_row(string_buffers[0], string_buffers[1], int_buffers[2])) {
        int app1 = stoi(string_buffers[0].substr(4)), app2 = stoi(string_buffers[1].substr(4));
        if(global:: app_inter_list.find(app1) == global:: app_inter_list.end()) {
            global:: app_inter_list[app1] = vector<pair<int, int> >();
        }
        global:: app_inter_list[app1].push_back(make_pair(app2, int_buffers[2]+(app1==app2)));
        global:: app_inter1.push_back(app1);
        global:: app_inter2.push_back(app2);
        global:: app_inter_max.push_back(int_buffers[2]+(app1==app2));
    }
    /*cerr << global:: app_inter1[3] << " " 
         << global:: app_inter2[3] << " " 
         << global:: app_inter_max[3] << endl;
    */
    cerr << "input app_interference_num = " << (global:: app_interference_num = global:: app_inter_max.size()) << endl;
}

void process_data() {
    
    for (int i =0 ;i<global::time_len; i++) {
        global:: sum_cpu_line.push_back(0.0);
        global:: sum_mem_line.push_back(0.0);
    }
    int flag = 0; 
    for (auto &lines : global:: app_cpu_line ) {
        vector<int> tmp ;
        global:: app_ins_num.push_back(0);
        if (flag == 0 ) {flag = 1;continue;}
        //cout<< lines.size() << endl;
        assert( lines.size()== global:: time_len ) ;
        double tmp_max_cpu = 0;
        for (int t=0;t<global::time_len;t++) {
            //tmp.push_back(t*1000);
            tmp_max_cpu = max(tmp_max_cpu,lines[t]);
            //global::sum_cpu_line[t] += lines[t];
        }
        global:: app_max_cpu.push_back(tmp_max_cpu);
        //global:: app_cpu_line.push_back(tmp);
    }
    flag = 0; 
    for (auto &lines : global:: app_mem_line ) {
        vector<int> tmp ;
        if (flag == 0 ) {flag = 1;continue;}
        //cout<< lines.size() << endl;
        assert( lines.size()== global:: time_len ) ;
        for (int t=0;t<global::time_len;t++) {
            //tmp.push_back(t*1000);
            //global::sum_mem_line[t] += lines[t];
        }
        //global:: app_mem_line.push_back(tmp);
    }
    
    int counter = 0;
    double ave_cpu_p_disk = 0;
    for (int i=1;i<=global:: instance_deploy_num;i++) {
        global:: instance_index[global:: instance_ids[i]] = i;
        global:: app_ins_num[global:: instance_apps[i]]++;
        double max_cpu =0;
        for (double t : global:: app_cpu_line[global:: instance_apps[i]]) max_cpu=max(max_cpu,t);
        for (int t=0;t<global::time_len;t++) {
            global::sum_cpu_line[t] += global::app_cpu_line[global:: instance_apps[i]][t];
            global::sum_mem_line[t] += global::app_mem_line[global:: instance_apps[i]][t];
        }
        
        //cout << max_cpu << "\t" << global:: app_apply[global:: instance_apps[i]]
        //    << "\t" << max_cpu/global:: app_apply[global:: instance_apps[i]] << endl;
        if (max_cpu <16 ) {
            max_cpu = max_cpu/global:: app_apply[global:: instance_apps[i]];
            ave_cpu_p_disk += max_cpu;
            ++counter;
        }    
    }
    //cout << "ave_cpu_p_disk: "<< ave_cpu_p_disk/global:: instance_deploy_num << " num: " << counter << endl;
    counter = 0;
    
    global::sum_cpu = 0;
    for (int i=0;i<global::time_len;i++) global::sum_cpu+=global::sum_cpu_line[i];
    for (int i=0;i<global::time_len;i++) cout << global::sum_cpu_line[i] << " " ;
    cout << endl;
    for (int i=0;i<global::time_len;i++) cout << global::sum_mem_line[i] << " " ;
    cout << endl;
    
    
    for (int i =1; i<= global:: app_resources_num ;i ++ ) 
        global:: self_inter_num.push_back(0);
        
    //for (auto &t : global:: cpu_spec ) t*= 1000;
    //for (auto &t : global:: mem_spec ) t*= 1000;
    for (int i =0; i< global:: app_interference_num ;i ++ ) {
        if ( global:: app_inter1[i] == global:: app_inter2[i] ) 
            global:: self_inter_num[global:: app_inter1[i]]=global:: app_inter_max[i];
            
            /*
        if ( global:: app_inter1[i] == global:: app_inter2[i] && global:: app_inter_max[i]==0 ) { 
            cout << global:: app_inter1[i] << ":\t" << global:: app_inter_max[i] 
            << "\t" << global:: app_ins_num[global:: app_inter1[i]] << endl;
            counter += global:: app_ins_num[global:: app_inter1[i]]; 
        }
        */
        
        global:: app_inter_map[global:: app_inter2[i]][global:: app_inter1[i]]=global:: app_inter_max[i] ;
        //if (global:: app_inter_max[i]==0) {
        global:: app_inter_set[global:: app_inter2[i]].insert(make_pair(global:: app_inter1[i],global:: app_inter_max[i])) ;
        global:: app_rvs_inter_set[global:: app_inter1[i]].insert(make_pair(global:: app_inter2[i],global:: app_inter_max[i])) ;
        //}
    }
    
    //cout << "counter:" << counter <<endl; 
    
    counter = 0;
    for (int i =1; i<= global:: app_resources_num ;i ++ ) {
        global:: app_inter_counter[i] = (global:: app_inter_set.count(i) ? global:: app_inter_set[i].size() : 0) 
            + (global:: app_rvs_inter_set.count(i) ? global:: app_rvs_inter_set[i].size() : 0) ;
            
        /*
        if ( global:: app_inter_map[i].count(i) && global:: app_inter_map[i][i] == 1) 
            cout << i << "\t" << global:: app_inter_counter[i] << "\t" <<  global:: app_ins_num[i] 
            << "\t" << global:: app_apply[i] << "\t" << global:: app_inter_map[i][i] << endl;
            */
    }
    //compute_cpu_sup();
}

void read_output_file( string output_file_name, map<int,int> &result) {
    io::CSVReader<2> in0(output_file_name);
    std::string string_buffers[10];
    int int_buffers[10];
    result.clear();
    for (int i=1;i<=global::instance_deploy_num;i++) if (global::instance_machines[i]!=-1) {
        result[i] = global::instance_machines[i];
    }
    while(in0.read_row(string_buffers[0], string_buffers[1]))  {
        result[global:: instance_index[stoi(string_buffers[0].substr(5))]] = stoi(string_buffers[1].substr(8));
    }
}

using namespace global;
double cpu_score_in_mnum ( int k ) {
    int space = 3000*92+(k-3000)*32;
    double score = 0;
    for (int i=0;i<global::time_len;i++) {
        double l= 0.5, r= sum_cpu_line[i]/space;
        while (r-l>1e-6) {
            double score_a = 0, score_b = 0, mid = (l+r)/2;
            score_a = (exp(mid-0.5)*10-9)*(k-3000)+(exp((sum_cpu_line[i]-mid*32*(k-3000))/(3000*92)-0.5)*10-9)*3000;
            mid+=1e-7;
            score_b = (exp(mid-0.5)*10-9)*(k-3000)+(exp((sum_cpu_line[i]-mid*32*(k-3000))/(3000*92)-0.5)*10-9)*3000;
            if (score_a>score_b) l= mid;
            else r= mid;
            //cout << mid << " $ " << score_a << " & " << score_b <<" " << (sum_cpu_line[i]-mid*32*(k-3000))/(3000*92) <<endl;
        }
        //cout << endl;
        if (global::sum_cpu_line[i]/space<=0.5) score+= k;
        else 
        score += (exp(l-0.5)*10-9)*(k-3000)+(exp((sum_cpu_line[i]-l*32*(k-3000))/(3000*92)-0.5)*10-9)*3000;
        //score += (exp(max(sum_cpu_line[i]/space-0.5,0.0))*10-9)*k;
    }
    //cout <<  space << " " << score << endl;
    return score/time_len;
}

void compute_cpu_sup() {
    int l = 3000, r = 6000;
    for (int i=4500;i<=5500;i++) 
         cout << i << ":" << cpu_score_in_mnum(i) <<endl;
    /*
    while ( r-l>2) {
        int mid = (l+r)/2;
        if (cpu_score_in_mnum(mid)>cpu_score_in_mnum(mid+1)) l = mid;
        else r= mid+1;
        cout << mid << "\t" << cpu_score_in_mnum(mid) << endl;
    }
    cout << "###############" << endl;
    cout << l << ":" << cpu_score_in_mnum(l) <<endl
         << r << ":" << cpu_score_in_mnum(r) <<endl;
         */
    //system("pause");
    exit(0);
}
