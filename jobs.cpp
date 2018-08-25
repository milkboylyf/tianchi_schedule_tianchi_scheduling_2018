#include "jobs.h"


namespace global{
vector<Jobs> job_res;

map<string,int> str_to_job_id;

//总任务CPU占用*分钟 
double total_jobs_cpu;
}

bool Jobs_cmp(const Jobs& a, const Jobs& b) {
    return a.name < b.name;
}


