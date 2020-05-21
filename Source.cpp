#include "mythreadpool.h"

void rrr(int t) {
	this_thread::sleep_for(chrono::milliseconds(500));
}

int main() {
	Th_pool pool;
	vector<TaskInfo> mon;
	for (int i = 0; i < 10; i++)
		mon.push_back(pool.add_task_inf(rrr, i));
	pool.statys();
	for (int i = 0; i < 3; i++)
		pool.add_thread();

	std::thread thr([&]() {
		int is_completed = 0;
		int is_queue = 0;
		int is_perf = 0;
		while (is_completed != mon.size()) {
			is_completed = 0;
			is_queue = 0;
			is_perf = 0;
			for (int i = 0; i < mon.size(); i++) {
				string stat = mon[i].statys();
				if (stat == "completed")
					is_completed += 1;
				else if (stat == "performed")
					is_perf += 1;
				else if (stat == "in the queue")
					is_queue += 1;				
			}
			/*cout << "kok\n";*/
		}
		cout << "comp: " << is_completed << endl;
		cout << "perf: " << is_perf << endl;
		cout << "queue: " << is_queue << endl;
		pool.statys();
		});
	thr.detach();

	this_thread::sleep_for(chrono::milliseconds(500));
	for (int i = 0; i < 10; i++)
	{
		
		mon.push_back(pool.add_task_inf(rrr, 10 + i));
	}
	pool.statys();
	
	cout << "fff\n";
	pool.wait_to_complet(); // это тип перед деструктором вызывается чтоб не выскочил аборт (если это функция идет вечно, значит недождетесь завержения из за бесконечных функцмй что вы засунули в очередь задач)
	//this_thread::sleep_for(chrono::milliseconds(1000));
	pool.statys();
	this_thread::sleep_for(chrono::milliseconds(100));
	return 0;
}