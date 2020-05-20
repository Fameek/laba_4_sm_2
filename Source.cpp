#include "mythreadpool.h"


void foo(int f) {
	cout << "foo - " <<this_thread::get_id() <<"  "<< f << endl;
	this_thread::sleep_for(chrono::milliseconds(100));
}
void rrr(int t) {
	for (int i = 0;i<10 ;i++ ) {
		this_thread::sleep_for(chrono::milliseconds(200));
		cout << "rrr - " << this_thread::get_id() << "  " << t << endl;	
	}
}



int main() {
	
		Th_pool pool;
		//////////////////////////////////
		vector<TaskInfo> mon;
		for (int i = 0; i < 10; i++) {
			mon.push_back(pool.add_task_inf(rrr, 2));
			mon.push_back(pool.add_task_inf(rrr, 5));
			
			pool.statys();
		}	
		pool.add_thread();
		pool.add_thread();
		pool.add_thread();
		//mon[mon.size()-1].wait_to_complet(); //дождется завершения последней задачи 
		int is_completed = 0;
		while (is_completed != mon.size()) {
			is_completed = 0;
			int is_queue = 0;
			int is_perf = 0;
			for (int i = 0; i < mon.size(); i++) {
				string stat = mon[i].statys();
				if (stat == "completed") {
					is_completed += 1;
				}
				else if (stat == "performed") {
					is_perf += 1;

				}
				else if (stat == "in the queue") {
					is_queue += 1;
				}
			}
			this_thread::sleep_for(chrono::milliseconds(500));
			cout << "comp: " << is_completed << endl;
			cout << "perf: " << is_perf << endl;
			cout << "queue: " << is_queue << endl;
			pool.statys();


		}
		///////////////////////////////////////////


		

		pool.wait_to_complet(); // это тип перед деструктором вызывается чтоб не выскочил аборт (если это функция идет вечно, значит недождетесь завержения из за бесконечных функцмй что вы засунули в очередь задач)
	return 0;
}
