#include "global.h"
#include "read.h"
#include "code.h"
#include <iostream>

int main() {
    read_data(
            "../data_preliminary/scheduling_preliminary_instance_deploy_20180606.csv",
            "../data_preliminary/scheduling_preliminary_app_resources_20180606.csv",
            "../data_preliminary/scheduling_preliminary_machine_resources_20180606.csv",
            "../data_preliminary/scheduling_preliminary_app_interference_20180606.csv");
    process_data();
    Code a(machine_resources_num);
    a.init();
    a.show_status();
    return 0;
}
