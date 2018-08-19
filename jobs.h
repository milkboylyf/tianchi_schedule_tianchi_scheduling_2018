#ifndef OFFLINE_JOBS
#define OFFLINE_JOBS

#include <vector>
#include <map>
#include <string>

using namespace std;

struct Jobs {
    int id;
    double cpu;
    double mem;
    int ins_size;
    int time;
    int fn;
    int f[100];
};


#endif 
