#pragma once
#include<iostream>
#include <algorithm>
#include<vector>
#include"List.cpp"
#include<iterator>
#include"InputSet.hpp"
//#include"Hash.hpp"
#include<future>
#include<Windows.h>
#include <mutex>
#include<fstream>
#include<stack>
#include <unordered_map>
#include <condition_variable>


enum STATES
{
	NULL_STATE = -1,
	False =0,            
	True =1                
};

class InvertedList {
private:
	std::unordered_map<std::string, List*> table;
	std::vector<std::string> ID;
	const int limit = 1225;
	std::vector<std::pair<Node*,int>> bin;
	std::vector<List*> L;
	List* Ll;
	int cur_epoch;
	int startedEpoch[3];
	int epoch[99999][3];
	int positionToInsert = 0;
	std::vector<Node*> recycled_node;
	static std::ofstream subor;
	const int maxThreads = 4;
public:
	void insertHashItem(std::string key, List* L);
	List* getHashList(std::string key);
	std::vector<std::string>getNames();
	bool save(const std::string file_name);
	bool load(const std::string file_name);
	InvertedList();
	std::vector<std::thread::id> threadID;
	void setList(std::string id, std::vector<int> lst);
	bool isEmpty();
	std::vector<int> sortV(std::string id, std::vector<int> val);
	Node* getFreeNode();
	std::vector<int> sortV2(std::string id, std::vector<int> val);
	bool apply(int th = -1, STATES insert = True);
	Node* SearchValue(std::string id, int value);
	bool isEmpty(std::string id);
	void init(std::string id, const int& val);
	void init2(std::string id, const std::vector<int>& val);
	void emptyUnusedNodes();
	//////////////////////////
	void insert(std::string id, const int& val);
	void insertIntoEmptyNode(Node* p,Node* Prev, List* lss, const int& val, int state);
	void splitNodeInsert(Node* p, Node* Prev, List* lss, const int& val, int state);
	int findID(std::string skill);
	List* findID1(std::string skill);
	void intersectN(std::vector<int>& output, std::vector<std::string>id);
	void intersect(std::promise<std::vector<int>>&& output, std::vector<std::string> id, int th);//std::promise<std::vector<int>>&& output
	static void print(int v);
	void set(std::string id, const std::vector<int>& value);
	/////////////////////////
	void report(std::string id);
	std::vector<int> reportList(std::string id);
	std::string findName2(std::string id, int tar[], int num);
};