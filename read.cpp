#include "csv.h"
#include "global.h"
#include "read.h"
#include <iostream>

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
    cerr << "input instance_deploy_num = " << (global:: instance_deploy_num = global:: instance_ids.size()) << endl;

    io::CSVReader<7> in1(app_resources_file);
    while(in1.read_row(
            string_buffers[0], string_buffers[1], string_buffers[2], 
            int_buffers[3], int_buffers[4], int_buffers[5], int_buffers[6]))  {
        global:: app_ids.push_back(stoi(string_buffers[0].substr(4)));
        global:: app_cpu_line.push_back(parse_float_list(string_buffers[1]));
        global:: app_mem_line.push_back(parse_float_list(string_buffers[2]));
        global:: app_apply.push_back(int_buffers[3]);
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
    cerr << "input app_resource_num = " << (global:: app_resources_num = global:: app_ids.size()) << endl;

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
    cerr << "input machine_resources_num = " << (global:: machine_resources_num = global:: machine_ids.size()) << endl;

    vector<int> app_inter1, app_inter2, app_inter_max;
    io::CSVReader<3> in3(app_interference_file);
    while(in3.read_row(string_buffers[0], string_buffers[1], int_buffers[2])) {
        global:: app_inter1.push_back(stoi(string_buffers[0].substr(4)));
        global:: app_inter2.push_back(stoi(string_buffers[1].substr(4)));
        global:: app_inter_max.push_back(int_buffers[2]);
    }
    /*cerr << global:: app_inter1[3] << " " 
         << global:: app_inter2[3] << " " 
         << global:: app_inter_max[3] << endl;
    */
    cerr << "input app_interference_num = " << (global:: app_interference_num = global:: app_inter_max.size()) << endl;
}