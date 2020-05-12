#include <iostream>
#include <thread>
#include <mutex>
#include <Windows.h>
#include <functional>
#include <vector>



using namespace std;


class Th_pool {
private:
	int numCPU;
	int numUsindCPU;
	int numfreeCPU=0;
	
	
	struct task_elem
	{
		function<void(int)> funk;
		int info;
		int stat = 0;
	};

	vector<task_elem>tasks;
	mutex mtx;
	
	vector<thread> Thread_list;
	vector<bool> activ_th;
	vector<bool> exit;
	void th_func_using() { // +
		int st = exit.size() - 1;
		thread::id id_th = this_thread::get_id();
		bool ex = exit[st];
		while (ex) {
			bool trriger = false;
			int indx_t;
			int indx_th;

			
			
			for (int i = 0; i < tasks.size(); i++) {
				mtx.lock();
				if (tasks[i].stat == 0) {
					
					indx_t = i;
					trriger = true;
					tasks[i].stat = 1;
					for (int j = 0; j < activ_th.size(); j++) {
						if (Thread_list[j].get_id() == id_th) {
							activ_th[j] = true;
							break;
						}

					}
					mtx.unlock();
					break;
				}
				mtx.unlock();
			}
			
			if (trriger) {
				function<void(int)> new_func = tasks[indx_t].funk;
				new_func(tasks[indx_t].info);
				tasks[indx_t].stat = 2;
				mtx.lock();
				for (int j = 0; j < activ_th.size(); j++) {
					if (Thread_list[j].get_id() == id_th) {
						activ_th[j] = false;
						break;
					}
				}
				mtx.unlock();
			}			



			bool tr = false;
			for (int i = 0; i < Thread_list.size(); i++) {
				if (Thread_list[i].get_id() == id_th) {
					ex = exit[i];
					tr = true;
					break;
					
				}
			}
			if (tr == false) {
				ex = false;
			}
		}
	numUsindCPU--;	
	}
	
public:
	Th_pool() { //+
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		numCPU = sysinfo.dwNumberOfProcessors;
		numUsindCPU = 1;			
	}

	void add_thread() { //+
		if(numCPU == numUsindCPU){
			mtx.lock();
			cout << "ERROR : Exceeded the thread limit (" << numCPU << "/" << numCPU <<")" << endl;
			mtx.unlock();
		}
		else {
			thread new_th(&Th_pool::th_func_using, this);
			mtx.lock();
			Thread_list.push_back(move(new_th));
			activ_th.push_back(false);
			numUsindCPU++;
			exit.push_back(true);
			mtx.unlock();
		}
	}
	void remove_thread() { //+

		bool trriger = false;
		
		mtx.lock();
		for (int i = 0; i < Thread_list.size(); i++) {
			if (activ_th[i] == false) {
				trriger = true;
				exit[i] = false;
				numUsindCPU--;
				Thread_list[i].detach();				
				exit.erase(exit.begin() + i);
				Thread_list.erase(Thread_list.begin() + i);
				activ_th.erase(activ_th.begin() + i);
				break;
			}

		}
		if (trriger == false) {
			cout << "ERROR: There are no free threads" << endl;
		}
		mtx.unlock();

	}

	void add_task(function<void(int)>func, int data) {
		task_elem r;
		r.funk = func;
		r.info = data;
		tasks.push_back(r);
	}

	void statys() {

		int wait = 0;
		int end = 0;
		int activ = 0;
		for (int i = 0; i < tasks.size(); i++) {
			if (tasks[i].stat == 0) {
				wait++;
			}
			else if (tasks[i].stat == 1) {
				activ++;
			}
			else if (tasks[i].stat == 2) {
				end++;
			}
		}

		mtx.lock();
		cout << "wait: " << wait << " | performed: " << activ << " | completed: " << end << endl;
		mtx.unlock();
	}

	void wait_to_complet() {
		while (1) {
			int s = exit.size();
			for (int i = 0; i < s; i++) {
				exit[i] = false;
			}
			for (int i = 0; i < Thread_list.size(); i++) {
				Thread_list[i].join();
			}
			for (int i = 0; i < s; i++) {
				exit.pop_back();
			}
			for (int i = 0; i < s; i++) {
				Thread_list.pop_back();
			}
			for (int i = 0; i < s; i++) {
				activ_th.pop_back();
			}
			for (int i = 0; i < s; i++) {
				this->add_thread();
			}

			bool trriger = false;
			for (int i = 0; i < tasks.size(); i++) {
				if (tasks[i].stat != 2) {
					trriger = true;
					break;
				}

			}
			if (trriger == false) {
				break;
			}


		}
	}

	~Th_pool(){ //+
		int s = exit.size();
		for (int i = 0; i < s; i++) {
			exit[i] = false;
		}

		for (int i = 0; i < Thread_list.size(); i++) {
			if (activ_th[i] == true) {
				mtx.lock();
				cout << "ERROR : you cannot call the destructor if functions in other threads are not completed" << endl;
				mtx.unlock();

				terminate();
			}
			Thread_list[i].join();
		}
		for (int i = 0; i < s; i++) {
			exit.pop_back();
		}
		numfreeCPU = 0;
		s = tasks.size();
		for (int i = 0; i < s; i++) {
			tasks.pop_back();
		}
		s = activ_th.size();
		for (int i = 0; i < s; i++) {
			activ_th.pop_back();
		}
		
		
		s = Thread_list.size();
		for (int i = 0; i < s; i++) {
			Thread_list.pop_back();
		}
		
		
	}

};


