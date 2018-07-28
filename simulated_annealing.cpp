#include "simulated_annealing.h"

bool judge( double score, double new_score ,double temper) {
    if (score > new_score) return true;
    double dE = score-new_score;
    //return exp( dE /temper )*10000 > rand()%10000;;
    return false;
}


void simulated_annealing (double end_time) {
    
	double starttime = (double)clock()/CLOCKS_PER_SEC  , endtime1 = 0.3, endtime2 = 0 ;
	
	//Code a(machine_resources_num);
	MergeWorker a(machine_resources_num);
	
	//*
	map<int, int > ip;
	
	read_output_file("../submit_b_6095_4816.csv", ip );
	
	a.restore_pos(ip);
	
	//生成初始解，当前算法核心2333 
	/*/
	a.init();                                     
	a.show_status();
	
	//有用了 
	double score = a.ave_score(), temper = 1000000000, min_score = score;
	int counter = 0, failed_times= 0, change_times = 0;
	//disk_spec[1] = 10000; 
	while ((double)clock()/CLOCKS_PER_SEC   - starttime < 20 && a.ave_score() > 5507.179) {             
        int tmp_i = rand()%instance_deploy_num+1;
        if (a.move_ins(tmp_i)) {
                double new_score = a.ave_score();
                if (judge(score,new_score,temper)) {
                    score = new_score;
                    a.accept();
                    if (min_score>score) min_score = score;
                }
                else a.recover();
                if(counter%100==0) {
                    a.show_status();
                    cout << "min_score: " << min_score << " temper: " << temper << " change times: " << change_times << " failed times: " << failed_times << " "; 
                }
                change_times++;
                temper*=0.99999;
        }
        else {
            a.recover();
            failed_times++;
            //cout << "fail to move:" << tmp_i <<endl;
        }
        counter++;
	}
	//*/
	for (int i=0;i<4000000&&a.ave_score()>5506+1e-4;i++) {
	    a.merge();
	    if (i%10==0) {
            a.show_status();
            if (kbhit()) {
                if (getch()=='a') break;
            }
        }
    }
    
    //a.make_integer_result(5600);
    
    a.show_status();
    
	//输出当前各服务器的disk状态 
	map<int,int> disk_space; 
	int space_remain = 0;
	for (auto m :a.m_ins) {
	   disk_space[m.disk]++;
	   space_remain += disk_spec[m.m_ids]-m.disk; 
    }
	for (auto d:disk_space) 
	   cout << d.first << ":" << d.second <<endl;
    global:: final_output = a.ins_pos;
}
