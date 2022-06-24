#pragma comment(lib, "winmm.lib")
#include<iostream>
#include "InputSet.hpp"
#include <math.h>
#include <algorithm>
#include"InvertedList.hpp"
#include<fstream>
#include<windows.h>
#include<thread>
#include<chrono>
#include<string>
#include<sstream>
#include<future>
#include <stdlib.h> 
#include <time.h>
#include <array>
#include <queue>
#include <atomic>
#include <cassert>
#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
//#include"thread_safe_queue.h"

struct test {
	int type;
	std::pair<std::string, int> insert;
	std::vector<std::string> intersect;
};
std::vector<test> fronta;
//#include <Python.h>
//#include <pybind11/embed.h>

template<typename Job>
void start_thread(std::vector<std::thread>& threads, Job&& job)
{
	// find an ended thread
	for (auto&& thread : threads)
	{
		if (thread.joinable()) // still running or waiting to join
			continue;

		thread = std::thread(job);
		return;
	}

	// if not wait for one
	for (auto&& thread : threads)
	{
		if (!thread.joinable()) // dead thread (not run or already joined)
			continue;

		thread.join();
		thread = std::thread(job);
		return;
	}
}

class ThreadPool
{
public:
	std::chrono::steady_clock::time_point start2;
	std::chrono::steady_clock::time_point end2; //= std::chrono::steady_clock::now();
	ThreadPool(int threads) : shutdown_(false)
	{
		start2 = std::chrono::steady_clock::now();
		// Create the specified number of threads
		threads_.reserve(threads);
		for (int i = 0; i < threads; ++i)
			threads_.emplace_back(std::bind(&ThreadPool::threadEntry, this, i));
	}

	~ThreadPool()
	{
		{
			// Unblock any threads and tell them to stop
			std::unique_lock <std::mutex> l(lock_);

			shutdown_ = true;
			condVar_.notify_all();
		}

		// Wait for all threads to stop
		std::cerr << "Joining threads" << std::endl;
		for (auto& thread : threads_)
			thread.join();
	}

	void doJob(std::function <void(void)> func)
	{
		// Place a job on the queu and unblock a thread
		std::unique_lock <std::mutex> l(lock_);
		jobs_.emplace(std::move(func));
		condVar_.notify_one();
	}
protected:

	void threadEntry(int i)
	{
		std::function <void(void)> job;

		while (1)
		{
			{
				std::unique_lock <std::mutex> l(lock_);

				while (!shutdown_ && jobs_.empty())
					condVar_.wait(l);

				if (jobs_.empty())
				{
					// No jobs to do and we are shutting down
					std::cerr << "Thread " << i << " terminates" << std::endl;
					end2 = std::chrono::steady_clock::now();
					std::cerr << "Elapsed time in milliseconds THREAD: "
						<< std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count()
						<< " ms" << std::endl;
					return;
				}

				//std::cerr << "Thread " << i << " does a job" << std::endl;
				job = std::move(jobs_.front());
				jobs_.pop();
			}

			// Do the job without holding any locks
			job();
		}

	}

	std::mutex lock_;
	std::condition_variable condVar_;
	bool shutdown_;
	std::queue <std::function <void(void)>> jobs_;
	std::vector <std::thread> threads_;
};

void dosmth(InvertedList* inv, std::vector<std::string> k, int th) {
	std::promise<std::vector<int>> intersectOutputT;
	std::future<std::vector<int>> futureObjT = intersectOutputT.get_future();
	inv->intersect(std::move(intersectOutputT), k, th);
	auto b = futureObjT.get();
	//std::cerr << b[0];
}
using namespace std::chrono;

std::mutex locking_mutex;
std::queue <std::function <void(void)>> jobs_;
std::condition_variable condVar_;
std::vector <std::thread> threads_;
InvertedList* inv = new InvertedList();

void testing(InvertedList* inv, int numberOfWords, int pocetVlakien) {

	srand((unsigned int)time(NULL));
	int random; //Sl˙ûi na generovanie n·hodneho ËÌsla od 0 po 9
	int randomInsertId; //Generuje n·hodnÈ ËÌslo od 1 po max poËet slov v invertovanom zozname.
	int randomInsertValue; //Generuje n·hodnÈ ËÌslo, ktorÈ sa bude vkladaù do invertovanÈho zoznamu
	std::vector<test> zoznam;
	///////////// Generovnie D·t do structu test
	for (int i = numberOfWords; i > 0;i--) {
		test T;
		random = rand() % 9; // Generuje n·hodnÈ ËÌslo od 1 po 9
		randomInsertId = rand() % numberOfWords + 1;
		std::string s = "skil" + std::to_string(randomInsertId);
		if (random < 2) { //Ak je ËÌslo 0, 1, tak sa vklad· do InvertovanÈho zozanamu
			randomInsertValue = rand() % 50 + 1;;
			std::pair<std::string, int> par;
			par.first = s;
			par.second = randomInsertValue;
			T.type = 0;
			T.insert = par;
		}
		else if (random >= 2) { //Ak je ËÌslo v‰Ëöie ako 1, tak sa robÌ prienik. V priemere by teda malo byù 20% vkladanie 80% prienik
			std::vector<std::string> input;
			input.push_back(s);
			randomInsertId = rand() % numberOfWords + 1;
			std::string s = "skil" + std::to_string(randomInsertId);
			input.push_back(s);
			T.type = 1;
			T.intersect = input;
			input.clear();
		}
		zoznam.push_back(T);
	}
	std::cout << "Zacina meranie\n";
	/////////////////////
	/////////////////////Insert a Intersect z·roveÚ, meranie
	std::array<std::thread, 4> threads;
	int ch = 0;
	auto start2 = std::chrono::steady_clock::now();
	ThreadPool p(pocetVlakien);
	for (int k = 0;k < zoznam.size();k++) {
		if (zoznam[k].type == 0) {
			//inv->insert(zoznam[k].insert.first, zoznam[k].insert.second);
		}
		else if (zoznam[k].type == 1) {
			fronta.push_back(zoznam[k]);

			if (pocetVlakien == 0) { // main thread
				std::promise<std::vector<int>> intersectOutputT;
				std::future<std::vector<int>> futureObjT = intersectOutputT.get_future();
				inv->intersect(std::move(intersectOutputT), zoznam[k].intersect, 0);
				auto b = futureObjT.get();
			}
			else {
				p.doJob(std::bind(dosmth, inv, zoznam[k].intersect, ch%pocetVlakien));
				ch++;
				
			}
		}
	}
	//for (int i = 0;i < fronta.size();i++) {
	//	p.doJob(std::bind(dosmth, inv, i, 0));
	//}
	
	auto end2 = std::chrono::steady_clock::now();
	std::cout << "INTERSECT Elapsed time in milliseconds: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count()
		<< " ms" << std::endl;
}
void insert(InvertedList* inv,double numberOfDocuments, int numberOfWords) {
	srand(time(NULL));
	int random;
	
	auto start2 = std::chrono::steady_clock::now();
	for (int j = numberOfDocuments; j > 1;j--) {
		for (int i = numberOfWords; i > -1;i--) {
			std::string s = "skil" + std::to_string(i);
			random = rand() % 250 + 1;
			inv->insert(s, random);
			numberOfDocuments = numberOfDocuments * 0.99;
		}
	}
	auto end2 = std::chrono::steady_clock::now();
	std::cout << "Insert Elapsed time in milliseconds: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count()
		<< " ms" << std::endl;
	
}


void intersect(InvertedList* inv, int numberOfWords) { //nepouûÌva sa teraz
	srand((unsigned int)time(NULL));
	int random;
	std::vector<int> output;
	std::vector<std::string> inputstr;
	auto start3 = std::chrono::steady_clock::now();


	for (int i = 0; i < 5000;i++) {
		for (int l = 0; l < 2;l++) {
			random = rand() % numberOfWords+1;
			inputstr.push_back("skil" + std::to_string(l));
		}
		std::promise<std::vector<int>> common;
		std::future<std::vector<int>> futureObj = common.get_future();
		inv->intersect(std::move(common), inputstr, 0);
		//thr1.join();
		inputstr.empty();
	}
	auto end3 = std::chrono::steady_clock::now(); 
	std::cout << "Intersect Elapsed time in milliseconds: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(end3 - start3).count()
		<< " ms" << std::endl;
}


void Normal_testing(int pocetDocs, int pocetSlov) {
	srand(time(NULL));
	int random;
	std::vector<std::pair<std::string, std::vector<int>>> inter;
	std::vector<int> ins;
	/*insert*/
	for (int i = 0;i < pocetSlov;i++) {
		for (int j = 0; j < pocetDocs; j++) {
			random = rand() % 200 + 1;
			ins.push_back(random);
		}
		sort(ins.begin(), ins.end());
		std::string s = "skil" + std::to_string(i);
		inter.push_back(std::make_pair(s, ins));
		ins.clear();
	}
	/*intersect + insert*/
	int randomInsertId; //Generuje n·hodnÈ ËÌslo od 1 po max poËet slov v invertovanom zozname.
	int randomInsertValue; //Generuje n·hodnÈ ËÌslo, ktorÈ sa bude vkladaù do invertovanÈho zoznamu
	
	std::vector<test>zoznam;
	for (int i = pocetSlov; i > 0;i--) {
		test T;
		random = rand() % 9; // Generuje n·hodnÈ ËÌslo od 1 po 9
		randomInsertId = rand() % pocetSlov + 1;
		std::string s = "skil" + std::to_string(randomInsertId);
		if (random < 2) { //Ak je ËÌslo 0, 1, tak sa vklad· do InvertovanÈho zozanamu
			randomInsertValue = rand() % 50 + 1;;
			std::pair<std::string, int> par;
			par.first = s;
			par.second = randomInsertValue;
			T.type = 0;
			T.insert = par;
		}
		else if (random >= 2) { //Ak je ËÌslo v‰Ëöie ako 1, tak sa robÌ prienik. V priemere by teda malo byù 20% vkladanie 80% prienik
			std::vector<std::string> input;
			input.push_back(s);
			randomInsertId = rand() % pocetSlov + 1;
			std::string s = "skil" + std::to_string(randomInsertId);
			input.push_back(s);
			T.type = 1;
			T.intersect = input;
			input.clear();
		}
		zoznam.push_back(T);
	}
	//////////////////Meranie
	std::cout << "zacina meranie";
	auto start2 = std::chrono::steady_clock::now();
	for (int k = 0;k < zoznam.size();k++) {
		if (zoznam[k].type == 0) {
			for (auto p : inter) {
				if (p.first == zoznam[k].insert.first) {
					//std::cout << p.first << " : " << zoznam[k].insert.first <<std::endl;
					p.second.push_back(zoznam[k].insert.second);
					sort(p.second.begin(), p.second.end());
					break;
				}
			}
			//inv->insert(zoznam[k].insert.first, zoznam[k].insert.second);
		}
		else if (zoznam[k].type == 1) {
			int cnt = 0;
			std::string vec1;
			std::string vec2;
			vec1 = zoznam[k].intersect[0];
			vec2 = zoznam[k].intersect[1];
			std::vector<int> intersected1;
			std::vector<int> intersected2;
			for (auto p : inter) {
				if (p.first == vec1) {
					cnt++;
					intersected1 = p.second;
				}
				else if (p.first == vec2) {
					cnt++;
					intersected2 = p.second;
				}
				if (cnt == 2) {
					cnt = 0;
					break;
				}
			}
			std::vector<int> intersection;
			std::set_intersection(intersected1.begin(), intersected1.end(), intersected2.begin(), intersected2.end(), std::back_inserter(intersection));
		}

	}
	auto end2 = std::chrono::steady_clock::now();
	std::cout << "\nINTERSECT Elapsed time in milliseconds: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count()
		<< " ms" << std::endl;
}
int main(void) {

	//insert(inv, 100, 100000);
	//testing(inv, 100000,1); // 0 - 4 vlakna
	
	Normal_testing(100, 10000);
}