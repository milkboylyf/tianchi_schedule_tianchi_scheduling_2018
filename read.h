#pragma once
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>

void read_data(
        string instance_deploy_file,
        string app_resources_file,
        string machine_resources_file,
        string app_interference_file,
        string job_resources_file);
        
        
void process_data();

void read_output_file(
        string output_file_name,
        map<int,int> &result ); 

void compute_cpu_sup();
