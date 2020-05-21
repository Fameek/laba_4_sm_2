#include <iostream>
#include <thread>
#include <mutex>
#include <Windows.h>
#include <functional>
#include <vector>



using namespace std;

class TaskInfo {
private:
	vector<string>&staty;
	int stat_el;
	
	
public:

	TaskInfo(vector<string>& st, int rr):staty(st) {
		
		stat_el = rr;
	}
	
	

	string statys(){
		string res;
		if (&staty == NULL|| staty[stat_el] != "completed" && staty[stat_el] != "in the queue" && staty[stat_el] != "performed") {
			res = "completed";
			
		}
		else {
			res = staty[stat_el];
		}

		return res;
	}
	void wait_to_complet() {
		if (&staty != NULL) {
			while (staty[stat_el] != "completed") {

			}
		}

	}

};

class Th_pool {
private:
	int numCPU;
	int numUsindCPU;
	int numfreeCPU=0;
	
	
	struct task_elem
	{
		function<void(int)> funk;
		int info;
		
	};
	vector<string>stat;
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
				if (stat[i] == "in the queue") {
					
					indx_t = i;
					trriger = true;
					stat[i] = "performed";
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
				
				mtx.lock();
				for (int j = 0; j < activ_th.size(); j++) {
					if (Thread_list[j].get_id() == id_th) {
						activ_th[j] = false;
						break;
					}
				}
				stat[indx_t] = "completed";
				mtx.unlock();
			}			

			bool trrr = true;
			while (trrr) {
				for (int i = 0; i < tasks.size(); i++){
					if (stat[i] == "in the queue") {
						trrr = false;
					}
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
					break;
				}
				if (!ex) {
					break;
				}
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
			exit.push_back(true);
			activ_th.push_back(false);
			numUsindCPU++;
			mtx.unlock();
			Thread_list.push_back(move(new_th));
			
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
	//friend TaskInfo add_t() {
	//	TaskInfo inf(&tasks[tasks.size() - 1].stat);
	//	return inf;
	//}


	TaskInfo add_task_inf(function<void(int)>func, int data) {
		task_elem r;
		r.funk = func;
		r.info = data;
		mtx.lock();
		stat.push_back("in the queue");
		tasks.push_back(r);
		int rr = stat.size() - 1;
		TaskInfo inf(stat,rr);
		//inf.save_pointer(&stat[stat.size() - 1]);
		mtx.unlock();
		return inf;

	}


	void add_task(function<void(int)>func, int data) {
		task_elem r;
		r.funk = func;
		r.info = data;
		mtx.lock();
		stat.push_back("in the queue");
		tasks.push_back(r);
		mtx.unlock();
	}


	void statys() {

		int wait = 0;
		int end = 0;
		int activ = 0;
		mtx.lock();
		for (int i = 0; i < tasks.size(); i++) {
			if (stat[i] == "in the queue") {
				wait+=1;
			}
			else if (stat[i] == "performed") {
				activ+=1;
			}
			else if (stat[i] == "completed") {
				end+=1;
			}
		}

		
		cout << "wait: " << wait << " | performed: " << activ << " | completed: " << end << endl;
		mtx.unlock();
	}


	void wait_to_complet() {
		for (int i = 0; i < stat.size(); i++) {
			while (stat[i] != "completed") {

			}


		}



		//while (1) {
			//int s = exit.size();
			//for (int i = 0; i < s; i++) {
			//	exit[i] = false;
			//}

			//for (int i = 0; i < Thread_list.size(); i++) {
			//	Thread_list[i].join();
			//}

			//for (int i = 0; i < s; i++) {
			//	exit.pop_back();
			//}

			//for (int i = 0; i < s; i++) {
			//	Thread_list.pop_back();
			//}

			//for (int i = 0; i < s; i++) {
			//	activ_th.pop_back();
			//}

			//for (int i = 0; i < s; i++) {
			//	this->add_thread();
			//}

			//bool trriger = false;
			//for (int i = 0; i < tasks.size(); i++) {
			//	if (stat[i] != "completed") {
			//		trriger = true;
			//		break;
			//	}

			//}

			//if (trriger == false) {
			//	break;
			//}


		//}
	}

	~Th_pool(){ //+
		int s = exit.size();
		
		for (int i = 0; i < s; i++) {
			exit[i] = false;
		}

		for (int i = 0; i < Thread_list.size(); i++) {
			if (activ_th[i] == true) {
				//mtx.lock();
				cout << "ERROR : you cannot call the destructor if functions in other threads are not completed" << endl;
				//mtx.unlock();

				terminate();
			}
			Thread_list[i].join();
		}

		
	}

};


