#include "global.h"
#include "read.h"
#include "code.h"
#include "simulated_annealing.h"
#include <iostream>

int main() {
    read_data(
            "../data_preliminary/scheduling_preliminary_instance_deploy_20180606.csv",
            "../data_preliminary/scheduling_preliminary_app_resources_20180606.csv",
            "../data_preliminary/scheduling_preliminary_machine_resources_20180606.csv",
            "../data_preliminary/scheduling_preliminary_app_interference_20180606.csv");
    process_data();
    simulated_annealing (20000);
    return 0;
}
