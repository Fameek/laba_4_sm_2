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
		
		pool.statys();
		//стандартное добавление в очередь
		pool.add_task(foo, 8);
		pool.add_task(rrr, 1);
		pool.statys();
		// добовление новых потоков
		this_thread::sleep_for(chrono::milliseconds(1000));
		pool.add_thread();		
		pool.add_thread();
		pool.add_thread();
		this_thread::sleep_for(chrono::milliseconds(100));
		pool.statys();
		
		
		
		
		//переполнение очереди задач, потоки выполняют задачу и переходят на новую
		this_thread::sleep_for(chrono::milliseconds(2000));
		for (int i = 0; i < 5; i++) {
			pool.add_task(foo, i);
		}
		pool.statys();
		this_thread::sleep_for(chrono::milliseconds(500));
		
		
		pool.remove_thread(); // удаление свободного потока 


		//добавление новой задачи, которую можно мониторить 
		TaskInfo inf = pool.add_task_inf(rrr, 9);
		this_thread::sleep_for(chrono::milliseconds(500));
		string info = inf.statys();
		cout << info << endl;
		
		pool.wait_to_complet(); // это тип перед деструктором вызывается чтоб не выскочил аборт (если это функция идет вечно, значит недождетесь завержения из за бесконечных функцмй что вы засунули в очередь задач)
	return 0;
}
