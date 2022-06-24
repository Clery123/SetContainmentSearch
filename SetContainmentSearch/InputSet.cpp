#include <string>
#include<vector>
#include<iostream>
#include"InputSet.hpp"
	void InputSet::add(int a) {
		this->set.push_back(a);
	}
	void InputSet::setTo(std::vector<int> a) {
		this->set = a;
	}
	void InputSet::print() {
		for (int i : set) {
			std::cout << i << " ";
		}
		std::cout << std::endl;
	}
	std::vector<int> InputSet::getVec() {
		return this->set;
	}
	std::string InputSet::getMeno() {
		return this->meno;
	}