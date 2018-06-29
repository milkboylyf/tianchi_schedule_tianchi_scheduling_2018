#pragma once
#include <string>
void read_data(
        string instance_deploy_file,
        string app_resources_file,
        string machine_resources_file,
        string app_interference_file);
        
        
void process_data();

void read_output_file(
        string output_file_name,
        map<int,int> &result ); 
