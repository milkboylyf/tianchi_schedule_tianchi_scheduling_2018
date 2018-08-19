#include "global.h"
#include "read.h"
#include "code.h"
#include "misc.h"
#include "simulated_annealing.h"
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
            "../dataset/job_info.e.csv");
            //*/
            
    //*
    string input_num = "e";
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
    simulated_annealing (10);
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
    write_output(process_output2(global:: final_output), "../submit_b.csv");
    //write_output_origin(global:: final_output, "../submit_b.csv");
    return 0;
}
