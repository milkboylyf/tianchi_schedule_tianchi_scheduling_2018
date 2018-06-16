#include "simulated_annealing.h"

bool judge( double score, double new_score ) {
    if (score >new_score) return true;
    return false;
}


void simulated_annealing (double end_time) {
    
	double starttime = (double)clock()/CLOCKS_PER_SEC  , endtime1 = 0.3, endtime2 = 0 ;
	
	Code a(machine_resources_num);
	a.init();
	a.show_status();
	double score = a.ave_score();
	
	int counter = 0, failed_times= 0, change_times = 0;
	while ((double)clock()/CLOCKS_PER_SEC   - starttime < end_time ) {
        int tmp_i = rand()%instance_deploy_num+1;
        if (a.move(tmp_i)) {
            double new_score = a.ave_score();
            if (judge(score,new_score)) {
                score = new_score;
            }
            else a.recover();
            if(counter%10000==0) {
                cout <<"change times:" << change_times << "failed times:" << failed_times ; 
                a.show_status();
            }
            change_times++;
        }
        else failed_times++;
        //cout << "fail to move:" << tmp_i <<endl;
        counter++;
	}
    
}
