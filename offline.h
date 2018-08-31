#ifndef OFFLINE_SCHEDULING
#define OFFLINE_SCHEDULING

#include "global.h"
#include <cstring>
#include <queue>
#include <algorithm>
#include <tuple>
#include <cassert>
#include <iostream>

bool offline_scheduling (
        const map<int, int> &ins_pos, 
        vector<tuple<int, int, int> > &job_pos_and_time,
        int num);


#endif
