#ifndef BEST_FIT
#define BEST_FIT


#include "code.h"
#include <algorithm>

using namespace std;
using namespace global;

struct BestFit:Code {
    
    BestFit(int _len);
    
    int get_level( int ins );
    
    virtual void init();
    
    void show_extra_info () ;
};

#endif
