#define EXPORT_TEST_FUNCTIONS
#include"InvertedList.hpp"
//#include<pybind11/pybind11.h>
//#include <pybind11/stl.h>
//#include <pybind11/complex.h>
//#include <pybind11/functional.h>
//#include <pybind11/chrono.h>
//namespace py = pybind11;

	void InvertedList::setList(std::string id, std::vector<int> lst) {
		List* LS = new List();
		LS->meno = id;
		ID.push_back(id);
		this->L.push_back(LS);
		insertHashItem(id, LS);
		if (!lst.empty()) {
			set(id, lst);
			//init(id, lst[0]);
		}
	}

	bool InvertedList::save(const std::string file_name)
	{
		std::ofstream file(file_name, std::ios::out | std::ios::binary);
		if (!file.is_open()) {
			std::cout << "Cannot open save file!\n";
			return false;
		}
		int LS;
		std::vector<std::pair<std::string,int>> test;
		for (std::string s : ID) {
			List* oldList = new List;
			oldList = findID1(s);
			std::vector<int> vals;
			for (Node* p = oldList->Tail; p != nullptr; p = p->Next) {
				for (int i = 0; i < p->Value.size();i++) {
					test.push_back(std::make_pair(oldList->meno, p->Value[i]));
				}
			}
		}
		for (std::pair<std::string, int> inf : test) {
			unsigned int size2 = inf.first.size();// get the size
			file.write(reinterpret_cast<char*>(&size2), sizeof(size2));  // write it to the file
			file.write(inf.first.c_str(), inf.first.size());// write the actual text
			file.write(reinterpret_cast<const char*>(&inf.second), sizeof(inf.second));
		}
		file.close();
		return true;
	}
	bool InvertedList::load(const std::string file_name)
	{
		std::ifstream f(file_name, std::ios::in | std::ios::binary);
		if (!f.is_open()) {
			std::cout << "Cannot open save file!\n" << file_name << "\n";
			return false;
		}
		if (f.peek() == std::ifstream::traits_type::eof()) {
			std::cout << "File is empty\n";
			return false;
		}

		// read the size
		 int size = 0;
		 std::vector<std::pair<std::string,int>> vec;
		 f.read(reinterpret_cast<char*>(&size), sizeof(size));
		
		 std::pair<std::string,int> buffer; // Allocate a string, make it large enough to hold the input
		 buffer.first.resize(size);
		while (f.read(&buffer.first[0], buffer.first.size())) {
			std::string bf = buffer.first;
			f.read((char*)&buffer.second, sizeof(buffer.second));
			int fa = buffer.second;
			vec.push_back(std::make_pair(bf,fa));
			f.read(reinterpret_cast<char*>(&size), sizeof(size));
			buffer.first.resize(size);
		}
		f.close();
		this->L.clear();
		std::vector<int> inty;
		std::string helpS;
		helpS = vec[0].first;
		for (std::pair<std::string, int> p : vec) {
			if (helpS == p.first) {
				inty.push_back(p.second);
				helpS = p.first;
			}
			else
			{
				List* LSO = new List;
				LSO->meno = helpS;
				ID.push_back(helpS);
				this->L.push_back(LSO);
				insertHashItem(helpS, LSO);
				set(helpS, inty);

				inty.clear();
				inty.push_back(p.second);
				helpS = p.first;
			}
		}
		List* LSO = new List;
		LSO->meno = helpS;
		ID.push_back(helpS);
		this->L.push_back(LSO);
		insertHashItem(helpS, LSO);
		set(helpS, inty);
		std::cout << "a";
		return true;
}
	void InvertedList::insertHashItem(std::string key, List* L)
	{
		table.emplace(std::make_pair(key, L));
	}
	List* InvertedList::getHashList(std::string key)
	{
		if (table.find(key) == table.end()) {
			return nullptr;
		}
		return table[key];
	}
	std::vector<std::string>InvertedList::getNames() {
		std::vector<std::string> names;
		for (List* m : this->L) {
			names.push_back(m->meno);
		}
		return names;
	}
	InvertedList::InvertedList()
	{
		cur_epoch = 0;
		epoch[49][3] = { 0 };
	}
	std::vector<int> InvertedList::sortV(std::string id, std::vector<int> val) {
		std::sort(val.begin(), val.end(),std::less<int>());
		return val;
	}
	std::vector<int> InvertedList::sortV2(std::string id, std::vector<int> val) {
		std::sort(val.begin(), val.end(), std::greater<>());
		return val;
	}
	Node* InvertedList::SearchValue(std::string id, int value)
	{
		int store = 0;
		for (int k = 0; k < L.size(); k++) {
			if (L[k]->meno == id) {
				store = k;
				break;
			}
		}
		for (Node* p = L[store]->Tail; p != nullptr; p = p->Next)
		{
			if (p->Value[0] == value) {
				std::cout << p->Value[0] << "\t";
				return p;
			}
		}
		std::cout << std::endl;
	}
	 bool InvertedList::isEmpty(std::string id) {
		int store = 0;
		List* ll = getHashList(id); 
		int counter = 0;
		
		if (ll->Tail == nullptr)
			return true;
		return false;
	}
	void InvertedList::init2(std::string id, const std::vector<int>& val) {
		//int store = 0;
		List* ll = getHashList(id);
		
		Node* node = new Node;
		ll->Head = ll->Tail = node;
		ll->Head->Value = val;
		ll->Head->Next = nullptr;
	}
	std::mutex mtx;
	//////////////////////////
	Node* InvertedList::getFreeNode() {
		for (auto n : recycled_node) {
			if (n == nullptr || NULL || recycled_node.capacity() == 0) {
				return new Node;
			}
			else {
				recycled_node.erase(recycled_node.begin());
				return n;
			}
		}
		if (recycled_node.size() == 0) {
			return new Node;
		}
	}
	void setNext(Node* p, Node* node1, Node* previous, List* li) {
		if (p != li->Tail)
			previous->Next = node1;
		else {
			li->Tail = node1;
		}
	}
	void InvertedList::emptyUnusedNodes() {
		for (std::pair<Node*, int> par : bin) {
			int cnt = 0;
			for (int k = 0; k < 4;k++) {
				if (epoch[par.second][k] == 0) {
					cnt++;
					if (cnt == 4) {
						//counter--;
						par.first->Value.clear();
						recycled_node.push_back(par.first);
						std::vector<std::pair<Node*, int>>::iterator itt = std::find(bin.begin(), bin.end(), par);
						int indexx = std::distance(bin.begin(), itt);
						bin.erase(bin.begin() + indexx);
						std::vector<Node*>::iterator it = std::find(recycled_node.begin(), recycled_node.end(), par.first);
						int index = std::distance(recycled_node.begin(), it);
						recycled_node[index]->Value.clear();
						for (Node* nod : recycled_node) {
							nod->Value.clear();
						}
					}
				}
			}
		}
	}



	void InvertedList::insertIntoEmptyNode(Node* p,Node* Prev, List* lss, const int& val, int state) {
		std::vector<int>::iterator iterator = std::upper_bound(p->Value.begin(), p->Value.end(), val, std::greater<int>());
		int pos = iterator - p->Value.begin();
		Node* nd1 = getFreeNode();
		nd1->Value = p->Value;
		nd1->Value.insert(nd1->Value.begin() + pos, val);
		if (state == 0) {
			nd1->Next = nullptr;
		}
		else
		{
			nd1->Next = p->Next;
		}
		setNext(p, nd1, Prev, lss);
	}
	void InvertedList::splitNodeInsert(Node* p, Node* Prev, List* lss, const int& val, int state) {
		Node* nd = getFreeNode();
		Node* nd2 = getFreeNode();
		///Rozdeli na dva nody
		copy(p->Value.begin(), p->Value.end() - (limit >> 1), back_inserter(nd->Value));
		copy(p->Value.begin() + (limit >> 1) + 1, p->Value.end(), back_inserter(nd2->Value));
		///Zistit, do ktoreho nodu vlozit hodnotu
		if (val >= nd->Value[nd->Value.size() - 1]) {
			std::vector<int>::iterator iterator = std::upper_bound(nd->Value.begin(), nd->Value.end(), val, std::greater<int>());
			int pos = iterator - nd->Value.begin();
			nd->Value.insert(nd->Value.begin() + pos, val);
		}
		else {
			std::vector<int>::iterator iterator = std::upper_bound(nd2->Value.begin(), nd2->Value.end(), val, std::greater<int>());
			int pos = iterator - nd2->Value.begin();
			nd2->Value.insert(nd2->Value.begin() + pos, val);
		}

		nd->Next = nd2;
		if (state == 0) {
			nd2->Next = nullptr;
			bin.push_back(std::make_pair(p, cur_epoch));
			if (p == lss->Tail) {
				lss->Tail = nd;
				lss->Head = nd2;
			}
			else {
				Prev->Next = nd;
			}
		}
		else if (state == 1) {
			if (p == lss->Tail) {
				nd2->Next = lss->Tail->Next;
				lss->Tail = nd;
				bin.push_back(std::make_pair(p, cur_epoch));
				return;
			}
			if (p != lss->Tail) {//p->Next != nullptr &&
				nd2->Next = p->Next;
				Prev->Next = nd;
				bin.push_back(std::make_pair(p, cur_epoch));
				return;

			}
		}
	}
	void InvertedList::insert(std::string id, const int& val) {
		if (getHashList(id) == nullptr) {
			this->setList(id, { val });
			return;
		}
		apply();
		int counter = 0;
		emptyUnusedNodes();
		List* lss = getHashList(id);
		lss->len++;
		Node* Prev = lss->Tail;
		if (isEmpty(id))
			init2(id, { val });
		else {
			for (Node* p = lss->Tail; p != nullptr; p = p->Next) { //Pre kazdy Node
				if (p->Next == nullptr) { // Ak je node posledny v liste
					if (p->Value.size() < limit) { //Ak je Node prazdny
						insertIntoEmptyNode(p, Prev, lss, val, 0);
						bin.push_back(std::make_pair(p, cur_epoch));
						return;
					}
					else if(p->Value.size() == limit) { //Ak je Node plny
						splitNodeInsert(p, Prev, lss, val, 0);
						return;
					}
				}
				else if (p->Next != nullptr) { //Ak node nieje posledny v liste
					if (p->Value.size() < limit && val >= p->Value[p->Value.size()-1]) //Ak je node prazdny
					{
						insertIntoEmptyNode(p, Prev, lss, val, 1);
						bin.push_back(std::make_pair(p, cur_epoch));
						return;
					}
					else if(p->Value.size() == limit && val >= p->Value[p->Value.size() - 1])
					{
						splitNodeInsert(p, Prev, lss, val, 1);
						break;
					}
				}
				if (Prev->Next != nullptr) {
					Prev = p;
				}
			}
		}
	}


	List* InvertedList::findID1(std::string skill) {
		return getHashList(skill);
	}
	
	void InvertedList::print(int v) {
		std::cout << v << "\n";
	}
	
	std::condition_variable cv;
	std::atomic_bool done_{ false }, process_{ false };
	bool InvertedList::apply(int thread, STATES insert) {
		// making use of std::unique_lock so if the code throws an
	// exception, the std::mutex will still get unlocked correctly...

		std::unique_lock<std::mutex> lck(mtx, std::try_to_lock);
		bool gotLock = lck.owns_lock();

		if (gotLock)
		{
			if (insert == False) {
				startedEpoch[thread] = cur_epoch;
				epoch[cur_epoch][thread] = 1;
			}
			else if (insert == True) {
				int last_epoch[3];
				memcpy(last_epoch, epoch[cur_epoch], sizeof(last_epoch));
				cur_epoch = (cur_epoch + 1) % 99999;
				memcpy(epoch[cur_epoch], last_epoch, sizeof(epoch[cur_epoch]));
			}
			else if (insert == NULL_STATE) {
				std::atomic<int> ii(0);
				for (ii = startedEpoch[thread]; ii <= cur_epoch; ii++) {

					epoch[ii][thread] = 0;

				}
			}
		}

			return gotLock;
	}
	bool InvertedList::isEmpty() {
		if (this->L.empty()) {
			std::cout << "Inverted List is empty";
			return true;
		}
		return false;
	}

	void InvertedList::intersect(std::promise<std::vector<int>>&& output, std::vector<std::string> id, int th) { //prienik
		//std::cout << "start INT\n";
		//std::vector<int> output;
		if (isEmpty()) {
			return;
		}
		if (id.size() < 2) {
			return;
		}
		apply(th ,False);

	/*	std::thread::id this_id = std::this_thread::get_id();
		std::cout << "thread " << this_id << " ...\n";*/

		int posA = 0; int posB = 0;
		std::vector<List*> vct;
		std::vector<int> store;
		std::vector<int> str;
		for (auto& idcka : id)
		{
			vct.push_back(findID1(idcka));
		}
		
		Node* LA = vct[0]->Tail;
		Node* LB = vct[1]->Tail;
		posA = 0;
		posB = 0;
		while (posA < LA->Value.size() && posB < LB->Value.size())
		{
			if (LA->Value[posA] == LB->Value[posB])
			{
				str.push_back(LA->Value[posA]);
				posA++;
				posB++;
			}
			else if (LA->Value[posA] > LB->Value[posB]) posA++;
			else posB++;
			if (posA == LA->Value.size() && LA->Next != nullptr) {
				posA = 0;
				LA = LA->Next;
			}
			if (posB == LB->Value.size() && LB->Next != nullptr) {
				posB = 0;
				LB = LB->Next;
			}
		}
		for (int i = 2; i < vct.size(); i++)
		{
			Node* LA = vct[i]->Tail;
			posA = 0;
			posB = 0;

			while (posA < LA->Value.size() && posB < str.size())
			{
				if (LA->Value[posA] == str[posB])
				{
					store.push_back(LA->Value[posA]);
					posA++;
					posB++;
				}
				else if (LA->Value[posA] > str[posB]) posA++;
				else posB++;
				if (posA == LA->Value.size() && LA->Next != nullptr) {
					posA = 0;
					LA = LA->Next;
				}
				else if (posB == str.size()) {
					continue;
				}
			}
			str.clear();
			std::copy(store.begin(), store.end(), std::back_inserter(str));
			store.clear();
		}
		//output=str;
		output.set_value(str);
		apply(th,NULL_STATE);
		//return output;
	}


	void InvertedList::set(std::string id, const std::vector<int>& value) {
		std::vector<int> val;
		//int store = 0;
		List* ll = getHashList(id);
		ll->len += value.size();
		//L[store]->len += value.size();
		if (value.size() <= limit) {
			val = sortV2(id, value);
		}
		else
			val = sortV(id,value);
		if (isEmpty(id) && val.size() <= limit) {
			init2(id, val);
			return;
		}
		else if (val.size() > limit) {
			int cnt = 0;
			int j1 = 0;
			for (int i = 0; i < ceil(double(val.size()) / double(limit)); i++) {
				Node* node = new Node;
				
				for (int j = 0; j < limit; j++) {
					if (j >= 0 && j < val.size() - cnt * limit) {
						node->Value.insert(node->Value.begin(), val[j1]);
					}
					j1++;
				}
				if (cnt == 0)
					node->Next = nullptr;
				else
					node->Next = ll->Tail;
				ll->Tail = node;
				cnt += 1;
				if (i == 0) {
					ll->Head = node;
				}
			}
		}
		
	}
	/////////////////////////

	std::vector<int> InvertedList::reportList(std::string id)
	{
		std::vector<int> vals;
		List* lss = findID1(id);
		int cnt = 0;
		for (Node* p = lss->Tail; p != nullptr; p = p->Next)
		{
			
			for (int i = 0; i < p->Value.size(); i++) {
				if (i >= 0 && i < p->Value.size() - cnt * limit) {
					vals.push_back(p->Value[i]);
					cnt += 1;
					std::cout << p->Value[i] << "\n";
				}
			}
			
		}
		//std::cout << "celkovo prvkov: " << cnt;
		return vals;
		std::cout << std::endl;
	}

	//namespace py = pybind11;
	//void hello() {
	//	std::cout << "helloS";
	//}
	//PYBIND11_MODULE(SetContainmentSearch, module) {
	//	py::class_<InvertedList>(module, "InvertedList")
	//	.def(py::init<>())
	//	.def("reportList", &InvertedList::reportList)
	//	.def("getNames", &InvertedList::getNames)
	//	.def("setList", &InvertedList::setList)
	//	.def("insert", &InvertedList::insert)
	//	//.def("intersect", &InvertedList::intersect)
	//	.def("save", &InvertedList::save)
	//	.def("load", &InvertedList::load);
	//}