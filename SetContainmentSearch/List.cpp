#pragma once
#include<string>
#include"Node.cpp"
struct List {
	std::string meno = "";
	int len = 0;
	Node* Head;
	Node* Tail =nullptr;
};