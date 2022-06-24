#pragma once
#include<vector>
struct InputSet {
public:
	std::string meno = "";
	std::vector<int> set;
	void add(int a);
	void setTo(std::vector<int> a);
	void print();
	std::vector<int> getVec();
	std::string getMeno();
};