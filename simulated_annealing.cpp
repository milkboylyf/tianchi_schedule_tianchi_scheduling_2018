#include "simulated_annealing.h"

bool judge( double score, double new_score ,double temper) {
    if (score >new_score) return true;
    double dE = score-new_score;
    return exp( dE /temper )*10000 > rand()%10000;;
    return false;
}


void simulated_annealing (double end_time) {
    
	double starttime = (double)clock()/CLOCKS_PER_SEC  , endtime1 = 0.3, endtime2 = 0 ;
	
	Code a(machine_resources_num);
	a.init();
	a.show_status();
	double score = a.ave_score(), temper = 1e-3;
	
	int counter = 0, failed_times= 0, change_times = 0;
	while ((double)clock()/CLOCKS_PER_SEC   - starttime < end_time ) {
        int tmp_i = rand()%instance_deploy_num+1;
        if (a.move(tmp_i)) {
            int tmp_j = rand()%instance_deploy_num+1;
            //while (tmp_j==tmp_i) tmp_j = rand()%instance_deploy_num+1;
            if (a.move(tmp_j)) {
                double new_score = a.ave_score();
                if (judge(score,new_score,temper)) {
                    score = new_score;
                    a.accept();
                }
                else a.recover();
                if(counter%10000==0) {
                    cout << "temper:" << temper << " change times:" << change_times << "failed times:" << failed_times << " "; 
                    a.show_status();
                }
                change_times++;
                temper*=0.999999;
            }
            else a.recover();
        }
        else failed_times++;
        //cout << "fail to move:" << tmp_i <<endl;
        counter++;
	}
    global:: final_output = a.ins_pos;
}
