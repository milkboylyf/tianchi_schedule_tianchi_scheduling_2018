#include "jobs.h"


namespace global{
vector<Jobs> job_res;

map<int,string> job_id_to_str;
map<string,int> str_to_job_id;


//总任务CPU占用*分钟 
double total_jobs_cpu;
}


