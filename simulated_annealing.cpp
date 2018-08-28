#include "simulated_annealing.h"

bool judge( double score, double new_score ,double temper) {
    if (score > new_score) return true;
    double dE = score-new_score;
    //return exp( dE /temper )*10000 > rand()%10000;;
    return false;
}

void run_thread( ParallelMergeWorker *mw ) {
    mw->dfs_m_divide(mw->constant_ins_num);
}

void simulated_annealing (double end_time) {
    
	double starttime = (double)clock()/CLOCKS_PER_SEC  , endtime1 = 0.3, endtime2 = 0 ;
	
	Code coder(machine_resources_num);
	int workers_num = 1;
	vector<ParallelMergeWorker*> workers;
	for ( int i=0;i<workers_num;i++) 
        workers.push_back(new ParallelMergeWorker(machine_resources_num));
	//ParallelMergeWorker mw(machine_resources_num);
	
	//MergeWorker a(machine_resources_num);
	
	/*
	map<int, int > ip;
	
	read_output_file("../submit_final_e_8330.csv", ip );
	
	coder.restore_pos(ip);
	
	//生成初始解，当前算法核心2333 
	/*/
	coder.init();                                     
	coder.show_status();
	
	//有用了 
	double score = coder.ave_score(), temper = 1000000000, min_score = score;
	int counter = 0, failed_times= 0, change_times = 0;
	//disk_spec[1] = 10000; 
	while ((double)clock()/CLOCKS_PER_SEC   - starttime < 2 && coder.ave_score() > 5507.179) {             
        int tmp_i = rand()%instance_deploy_num+1;
        if (coder.move_ins(tmp_i)) {
                double new_score = coder.ave_score();
                if (judge(score,new_score,temper)) {
                    score = new_score;
                    coder.accept();
                    if (min_score>score) min_score = score;
                }
                else coder.recover();
                if(counter%100==0) {
                    coder.show_status();
                    cout << "min_score: " << min_score << " temper: " << temper << " change times: " << change_times << " failed times: " << failed_times << " "; 
                }
                change_times++;
                temper*=0.99999;
        }
        else {
            coder.recover();
            failed_times++;
            //cout << "fail to move:" << tmp_i <<endl;
        }
        counter++;
	}
	//*/
	
	
	
	
	for (int i=0;i<4000;i++) {
	    vector<int> merge_a, merge_b;
	    set<int> moving_machines;
	    for ( int j=0;j<workers_num;j++) {
            int m_a, m_b , times = 0;
            do {
                m_a = rand()%machine_resources_num+1;
                times ++;
            }
            while ( (moving_machines.count(m_a)||coder.m_ins[m_a].empty()) && times <20000) ;
            if (times >=20000) return;
            do {
                m_b = rand()%machine_resources_num+1;
                times ++;
            }
            while ( (m_a == m_b || moving_machines.count(m_b)||coder.m_ins[m_b].empty()) && times < 200000 );
            merge_a.push_back(m_a);
            merge_b.push_back(m_b);
            moving_machines.insert(m_a);
            moving_machines.insert(m_b);
        }
	    for ( int j=0;j<workers_num;j++) {
	        workers[j]->before_merge(coder,coder.m_ins[merge_a[j]],coder.m_ins[merge_b[j]] );
        }
        vector<thread> mwthreads;
	    for ( int j=0;j<workers_num;j++) {
	        mwthreads.push_back(thread(run_thread,workers[j]));
        }
	    for ( int j=0;j<workers_num;j++) {
	        mwthreads[j].join();
        }
	    for ( int j=0;j<workers_num;j++) {
	        workers[j]->after_merge(coder,coder.m_ins[merge_a[j]],coder.m_ins[merge_b[j]] );
        }
        
	    if (i%10==0) {
            coder.show_status();
            
#ifdef _WIN32
            if (kbhit()) {
                if (getch()=='a') break;
            }
#endif
        }
    }
    
    //a.make_integer_result(5600);
    
    coder.show_status();
    
	//输出当前各服务器的disk状态 
	map<int,int> disk_space; 
	int space_remain = 0;
	for (auto m :coder.m_ins) {
	   disk_space[m.disk]++;
	   space_remain += disk_spec[m.m_ids]-m.disk; 
    }
	for (auto d:disk_space) 
	   cout << d.first << ":" << d.second <<endl;
    global:: final_output = coder.ins_pos;
}
