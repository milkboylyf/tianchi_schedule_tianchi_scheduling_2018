#include "global.h"
#include "read.h"
#include "code.h"
#include "misc.h"
#include "simulated_annealing.h"
#include "offline.h" 
#include "move.h"
#include <iostream>

int main() {
    /*
    read_data(
            "../data_preliminary/scheduling_preliminary_instance_deploy_20180606.csv",
            "../data_preliminary/scheduling_preliminary_app_resources_20180606.csv",
            "../data_preliminary/scheduling_preliminary_machine_resources_20180606.csv",
            "../data_preliminary/scheduling_preliminary_app_interference_20180606.csv",
            "../dataset/job_info.e.csv");
            */
    /*
    read_data(
            "../data_preliminary/scheduling_preliminary_b_instance_deploy_20180726.csv",
            "../data_preliminary/scheduling_preliminary_b_app_resources_20180726.csv",
            "../data_preliminary/scheduling_preliminary_b_machine_resources_20180726.csv",
            "../data_preliminary/scheduling_preliminary_b_app_interference_20180726.csv",
            "../dataset/job_info.a.csv");
            //*/
       
       
    const int online = 0;
    const int offline = 1;
    const int move_ins = 2;
    
    //***************all parameters*****************
    
    int mode = 0;                   //模式 
    string input_num = "a";         //数据集 
    
    //offline
    int empty_mch_use = 0;         //离线使用的空服务器数量
    
    //online
    int thread_num = 1;         //线程数 
    double max_cpu_rate = 1.91; //用来控制服务器数，通常区间在1.6-2.2之间 
    int stop_time = 1000;       //运行时间，秒
    int not_used_large = 0;     //由于c和d的数据，需要为jobs空出一些大的服务器，大致在几十吧 
    
    //各种文件名在下面 
    
    //***************end parameters*****************
        
    //*
    read_data(
            "../dataset/instance_deploy."+input_num+".csv",
            "../dataset/app_resources.csv",
            "../dataset/machine_resources."+input_num+".csv",
            "../dataset/app_interference.csv",
            "../dataset/job_info."+input_num+".csv");
            //*/
    process_data();
    //check_output_file("../../submit20180629215200.csv");
    //check_output_file("../../submit20180626111523.csv");
    
    
    
    
    if (mode == offline)
    {
    	map<int, int > ip;
    	read_output_file_turn("../submit_final_a_tmp1_s.csv", ip);
    	vector<tuple<int, int, int> > job_pt;
    	offline_scheduling(ip, job_pt, empty_mch_use);
    	write_offline_result("../submit_final_a_tmp1_o.csv",job_pt);
        return 0;
    }
    else if (mode == move_ins) {
    	map<int, int > ip;
    	vector<int> ins_mch = instance_machines;
    	
    	read_output_file("../submit_final_a_m4478_4482.csv", ip );
    	vector<vector<pair<int,int> > > results = test_move(ip, ins_mch);
    	write_output_turn(results, "../submit_final_a_tmp1_s.csv");
    	return 0;
    }
    else {
    
        simulated_annealing (thread_num,max_cpu_rate,stop_time,not_used_large);
        /*
        DP dp(machine_resources_num);
        dp.init();
        dp.dp_plan();
        global:: final_output = dp.ins_pos;
        /*//* 
        Deployer dp(machine_resources_num);
        dp.init();
        dp.package_up();
        global:: final_output = dp.ins_pos;
        //*/
        check_output(global:: final_output);
        //write_output(process_output2(global:: final_output), "../submit_final_"+input_num+"_.csv");
        write_output_origin(global:: final_output, "../submit_final_"+input_num+"_.csv");
    }
    return 0;
}
