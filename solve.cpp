#include "global.h"
#include "read.h"
#include "code.h"
#include "misc.h"
#include "simulated_annealing.h"
#include <iostream>

int main() {
    read_data(
            "../data_preliminary/scheduling_preliminary_instance_deploy_20180606.csv",
            "../data_preliminary/scheduling_preliminary_app_resources_20180606.csv",
            "../data_preliminary/scheduling_preliminary_machine_resources_20180606.csv",
            "../data_preliminary/scheduling_preliminary_app_interference_20180606.csv");
    process_data();
    simulated_annealing (20);
    check_output(global:: final_output);
    write_output(process_output(global:: final_output), "../submit.csv");
    return 0;
}
