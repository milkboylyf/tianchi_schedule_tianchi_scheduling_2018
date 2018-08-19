#include "csv.h"
#include "global.h"
#include "read.h"

using namespace std;
using namespace global;

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

vector<string> split( const string& s) {
    string sb = s;
    vector<string> results;
    while (sb.size()>0) {
        int index = sb.find(',');
        if (index == -1) {
            results.push_back(sb);
            return results;
        }
        results.push_back(sb.substr(0,index));
        sb = sb.substr(index+1);
    }
}

void read_data(
        string instance_deploy_file,
        string app_resources_file,
        string machine_resources_file,
        string app_interference_file,
        string job_resources_file) {

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

    ifstream job_input(job_resources_file);
    string line ;
    while(!job_input.eof()) {
        job_input >> line ;
        if (line.size()<2) break;
        cout << line <<endl;
        vector<string> splits = split(line);
        Jobs a;
        if (!str_to_job_id.count(splits[0])) {
            a.id = global::job_res.size();
            global::job_id_to_str[a.id]= splits[0];
            global::str_to_job_id[splits[0]] = a.id;
        }
        else a.id = global::str_to_job_id[splits[0]];
        a.cpu = stod(splits[1]);
        a.mem = stod(splits[2]);
        a.ins_size = stol(splits[3]);
        a.time = stol(splits[4]);
        global::job_res.push_back(a);
        a.fn = splits[5].size()?splits.size()-5:0;
        for (int i=0;i<a.fn;i++) {
            if (str_to_job_id.count(splits[i+5]))
                a.f[i] = global::str_to_job_id[splits[i+5]];
            else {
                a.f[i] = global::job_res.size();
                global::job_id_to_str[a.f[i]]= splits[0];
                global::str_to_job_id[splits[0]] = a.f[i];
            }
        }
    }
    /*cerr << global:: app_inter1[3] << " " 
         << global:: app_inter2[3] << " " 
         << global:: app_inter_max[3] << endl;
    */
    cerr << "input app_interference_num = " << (global:: app_interference_num = global:: app_inter_max.size()) << endl;
}

void process_data() {
    
    large_num=small_num=0;
    for (int i =1;i<=machine_resources_num;i++) {
        if (disk_spec[i]>2000) large_num ++;
        else small_num++;
    }
    
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
    
    total_jobs_cpu = 0;
    for (Jobs &a : job_res ) {
        total_jobs_cpu += a.time*a.cpu*a.ins_size/15.0;
    }
    compute_cpu_sup();
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

inline double compute_mch_score( double rate, double apps_per_mch, int mch_num) {
    return (1+(exp(max(0.0,rate-0.5))-1)*apps_per_mch)*mch_num;
}

double cpu_score_with_mnum ( int k ) {
    int space = large_num*92+(k-large_num)*32;
    double score = 0;
    double apps_per_mch = (double)instance_deploy_num / k;
    for (int i=0;i<global::time_len;i++) {
        double l= 0.5, r= sum_cpu_line[i]/space;
        while (r-l>1e-6) {
            double score_a = 0, score_b = 0, mid = (l+r)/2;
            double rate_large = (sum_cpu_line[i]-mid*32*(k-large_num))/(large_num*92);
            //double apps_large = , apps_small = mid*32*(k-large_num)/ ;
            score_a = compute_mch_score(mid, apps_per_mch-1, small_num)
                        + compute_mch_score(rate_large, apps_per_mch+1, large_num);
            mid+=1e-7;
            rate_large = (sum_cpu_line[i]-mid*32*(k-large_num))/(large_num*92);
            score_b = compute_mch_score(mid, apps_per_mch-1, small_num)
                        + compute_mch_score(rate_large, apps_per_mch+1, large_num);
            if (score_a>score_b) l= mid;
            else r= mid;
            //cout << mid << " $ " << score_a << " & " << score_b <<" " << (sum_cpu_line[i]-mid*32*(k-3000))/(3000*92) <<endl;
        }
        //cout << endl;
        double rate_large = (sum_cpu_line[i]-l*32*(k-large_num))/(large_num*92);
        if (global::sum_cpu_line[i]/space<=0.5) {
            //score -= (space/2-sum_cpu_line[i])/(small_num?32:92)*1.64;
            score+= k;
        }
        else 
        score += compute_mch_score(l, apps_per_mch-1, small_num)
                        + compute_mch_score(rate_large, apps_per_mch+1, large_num);
        //score += (exp(max(sum_cpu_line[i]/space-0.5,0.0))*10-9)*k;
    }
    //cout <<  space << " " << score << endl;
    return score/time_len;
}

void compute_cpu_sup() {
    int l = 3000, r = 6000;
    double job_cpu_lines[100];
    double ins_cpu_lines_pre[100]={};
    vector<double> cpu_lines_t = sum_cpu_line;
    sort(cpu_lines_t.begin(),cpu_lines_t.end());
    double cpu_th = 0;
    for (int i=0;i<time_len;i++) {
        ins_cpu_lines_pre[i] = cpu_lines_t[i] + (i?ins_cpu_lines_pre[i-1]:0);
        if (i+1==time_len||total_jobs_cpu<cpu_lines_t[i]*(i+1)-ins_cpu_lines_pre[i]) {
            cpu_th = (total_jobs_cpu+ins_cpu_lines_pre[i])/(i+1);
        }
    }
    
    for (int i=1000;i<=8000;i+=10) 
         cout << i << ":" << cpu_score_with_mnum(i) <<endl;
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
