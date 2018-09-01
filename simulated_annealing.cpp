#include "simulated_annealing.h"

#ifdef _WIN32

#define sleep(a) Sleep(a)

#else

int kbhit(void)  
{  
struct termios oldt, newt;  
int ch;  
int oldf;  
tcgetattr(STDIN_FILENO, &oldt);  
newt = oldt;  
newt.c_lflag &= ~(ICANON | ECHO);  
tcsetattr(STDIN_FILENO, TCSANOW, &newt);  
oldf = fcntl(STDIN_FILENO, F_GETFL, 0);  
fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);  
ch = getchar();  
tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  
fcntl(STDIN_FILENO, F_SETFL, oldf);  
if(ch != EOF)  
{  
ungetc(ch, stdin);  
return 1;  
}  
return 0;  
}  

void sleep(int t) {
    std::this_thread::sleep_for(std::chrono::milliseconds(t));
}

#endif


bool judge( double score, double new_score ,double temper) {
    if (score > new_score) return true;
    double dE = score-new_score;
    //return exp( dE /temper )*10000 > rand()%10000;;
    return false;
}


int merge_a[1000], merge_b[1000];
set<int> moving_machines;

mutex coder_mutex;

bool terminal;
Code *cd;

//void run_thread( ParallelMergeWorker *mw ) {
//    mw->dfs_m_divide(0);
//}

void run_thread( ParallelMergeWorker *mw ) {
	int count = 0;
    sleep(100);
	while (true) {
		//count++;
		//cout << mw->worker_id << " " << count++ <<endl;
		coder_mutex.lock(); 
		//cout << mw->worker_id << "####" <<endl;
		
	    int m_a, m_b , times = 0;
	    do {
	        m_a = rand()%machine_resources_num+1;
	        times ++;
	    }
	    while ( (moving_machines.count(m_a)||cd->m_ins[m_a].empty()) && times <20000) ;
	    if (times >=20000) continue;
	    do {
	        m_b = rand()%machine_resources_num+1;
	        times ++;
	    }
	    while ( (m_a==m_b||moving_machines.count(m_b)||cd->m_ins[m_b].empty()) && times < 200000 );
	    if (times >=200000) continue;
	    merge_a[mw->worker_id] = m_a;
	    merge_b[mw->worker_id] = m_b;
	    moving_machines.insert(m_a);
	    moving_machines.insert(m_b);
	    
	    
	   	mw->before_merge(*cd,cd->m_ins[m_a],cd->m_ins[m_b] );
	    
		coder_mutex.unlock();
		//cout << mw->worker_id << "$$$$" <<endl;
		
		mw->dfs_m_divide(0);
		
		//cout << mw->worker_id << "%%%%" <<endl;
		coder_mutex.lock();
	    
		//cout << mw->worker_id << "&&&&" <<endl;
		mw->after_merge(*cd,cd->m_ins[m_a],cd->m_ins[m_b] );
	    moving_machines.erase(m_a);
	    moving_machines.erase(m_b);
	    if (terminal) {
			coder_mutex.unlock();
			return ;
		}
	        
	    count ++;
	    //cout << mw->worker_id <<endl;
	    //if (!mw->worker_id&&count%10==0) cout << cd->ave_score() <<endl;
	    
		coder_mutex.unlock();
		//cout << mw->worker_id << "****" <<endl;
	}
}

bool simulated_annealing (int thread_num, double cpu_threshod, int sleep_times, int not_used_large) {
    
	double starttime = (double)clock()/CLOCKS_PER_SEC  , endtime1 = 0.3, endtime2 = 0 ;
	
	Code coder(machine_resources_num);
	int workers_num = thread_num;
	vector<ParallelMergeWorker*> workers;
	for ( int i=0;i<workers_num;i++) workers.push_back(new ParallelMergeWorker(machine_resources_num,i));
	//ParallelMergeWorker mw(machine_resources_num);
	
	//MergeWorker a(machine_resources_num);
	
	/*
	map<int, int > ip;
	
	read_output_file("../submit_final_e_8800.csv", ip );
	
	coder.restore_pos(ip);
	coder.show_status();
	
	//生成初始解，当前算法核心2333 
	/*/
	coder.set_threshod(cpu_threshod, not_used_large);
	coder.init();                                     
	coder.show_status();
	
	//有用了 
	double score = coder.ave_score(), temper = 1000000000, min_score = score;
	int counter = 0, failed_times= 0, change_times = 0;
	//disk_spec[1] = 10000; 
	while ((double)clock()/CLOCKS_PER_SEC   - starttime < 0 && coder.ave_score() > 5507.179) {             
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
	
	bool flag = 0;
	
	
    vector<thread> mwthreads;
    for ( int j=0;j<workers_num;j++) {
        mwthreads.push_back(thread(run_thread,workers[j]));
    }
    
    terminal = 0;
	cd = &coder;
	
	/*
    while ( true ) {
        char ch = getchar();
        //cout << "character: " << ch <<endl;
        if (ch=='a') {
        	terminal = 1;
		    for ( int j=0;j<workers_num;j++) {
		        mwthreads[j].join();
	        }
			break;
		}
    }
    
    //*/
	for (int i=0;i<sleep_times;i++) {
		
        
        sleep(1000);
	    //if (i%10==0) {
            //coder.show_status();
        if (kbhit()) {
        	char ch = getchar();
            if (ch=='a') {
            	terminal = 1;
			    for ( int j=0;j<workers_num;j++) {
			        mwthreads[j].join();
		        }
				break;
			}
        }
       // }
    }
    //*/
    
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
    
    return flag;
}
