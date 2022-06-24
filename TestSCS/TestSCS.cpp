#include "pch.h"
#include "CppUnitTest.h"
#include "../SetContainmentSearch/InvertedList.hpp"
#include"Hash.hpp"
//#include "../SetContainmentSearch/InvertedList.cpp"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestSCS
{
	TEST_CLASS(InvertedList)
	{
	private:
		const int limit = 5;
		std::vector<std::pair<Node*, int>> bin;
		std::vector<List*> L;
		Hash hs;
		List* Ll;
		int cur_epoch;
		int startedEpoch[3];
		std::atomic<int> epoch[49][3];
		int positionToInsert = 0;
		std::vector<Node*> recycled_node;
		//Node* recycled_node[49];
	public:
		std::mutex mtx;
		Node* InvertedList::getFreeNode() {
			for (auto n : recycled_node) {
				if (n == nullptr || NULL || recycled_node.capacity() == 0) {
					return new Node;
				}
				else {
					//int en = sizeof(recycled_node) / sizeof(recycled_node[0]);
					//auto itr = std::find(recycled_node, recycled_node+en, n);
					recycled_node.erase(recycled_node.begin());
					return n;
				}
			}
			if (recycled_node.size() == 0) {
				return new Node;
			}
		}
		void InvertedList::init(std::string id, const int& val) {
			int store = 0;
			for (int k = 0; k < L.size(); k++) {
				if (L[k]->meno == id) {
					store = k;
					break;
				}
			}
			Node* node = new Node;
			L[store]->Head = L[store]->Tail = node;
			L[store]->Head->Value.push_back(val);
			L[store]->Head->Next = nullptr;
		}
		List* InvertedList::findID1(std::string skill) {
			return hs.getList(skill);
		}
		//bool apply(int th = -1, STATES insert = True);
		bool InvertedList::apply(int thread = -1, STATES insert = True) {
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
					cur_epoch = (cur_epoch + 1) % 49;
					memcpy(epoch[cur_epoch], last_epoch, sizeof(epoch[cur_epoch]));
				}
				else if (insert == NULL_STATE) {
					for (std::atomic<int> ii = startedEpoch[thread]; ii <= cur_epoch; ii++) {

						epoch[ii][thread] = 0;

					}
				}
			}

			return gotLock;
		}
		void InvertedList::insert(std::string id, const int& val) {
			apply();
			int counter = 0;
			for (std::pair<Node*, int> par : bin) {
				int cnt = 0;
				for (int k = 0; k < 4;k++) {
					if (epoch[par.second][k] == 0) {
						cnt++;
						if (cnt == 4) {
							par.first->Value.clear();
							recycled_node.push_back(par.first);
							bin.erase(bin.begin() + counter);
							std::vector<Node*>::iterator it = std::find(recycled_node.begin(), recycled_node.end(), par.first);
							int index = std::distance(recycled_node.begin(), it);
							recycled_node[index]->Value.clear();
							for (Node* nod : recycled_node) {
								nod->Value.clear();
							}
						}
					}
				}
				counter++;
			}
			std::cout << "size: " << recycled_node.size();
			List* lss = findID1(id);
			lss->len++;
			Node* Prev = lss->Tail;
			if (isEmpty(id))
				init(id, val);
			else {
				for (Node* p = lss->Tail; p != nullptr; p = p->Next)
				{
					for (int i = 0; i < p->Value.size(); i++) {
						if (p->Next == nullptr && i == p->Value.size() - 1 && p->Value[i] > val) {
							if (p->Value.size() < limit)
							{
								Node* nd1 = getFreeNode();
								//Node* nd1 = new Node;
								nd1->Value = p->Value;
								nd1->Value.insert(nd1->Value.end(), val);
								nd1->Next = nullptr;
								//bin.push_back(p);
								bin.push_back(std::make_pair(p, cur_epoch));
								Prev->Next = nd1;
								return;
							}
							else {
								Node* nd = getFreeNode();
								Node* nd2 = getFreeNode();
								//Node* nd = new Node;
								//Node* nd2 = new Node;


								copy(p->Value.begin(), p->Value.end() - (limit >> 1), back_inserter(nd->Value));
								copy(p->Value.begin() + (limit >> 1) + 1, p->Value.end(), back_inserter(nd2->Value));
								nd2->Value.insert(nd2->Value.end(), val);
								nd->Next = nd2;
								nd2->Next = nullptr;
								Prev->Next = nd;
								return;
							}

						}
						else if (p->Value[i] < val) {
							if (p->Value.size() < limit)
							{
								Node* nd1 = getFreeNode();
								//Node* nd1 = new Node;
								nd1->Value = p->Value;
								auto it = find(nd1->Value.begin(), nd1->Value.end(), p->Value[i]);
								int index = it - nd1->Value.begin();
								nd1->Value.insert(nd1->Value.begin() + index, val);
								nd1->Next = p->Next;
								bin.push_back(std::make_pair(p, cur_epoch));
								if (p == lss->Tail) {
									lss->Tail = nd1;
									//L[store]->Tail = nd1;
									return;
								}
								else {
									Prev->Next = nd1;
									return;
								}
							}
							else
							{
								Node* nd = getFreeNode();
								Node* nd2 = getFreeNode();
								//Node* nd = new Node;
								//Node* nd2 = new Node;


								copy(p->Value.begin(), p->Value.end() - (limit >> 1), back_inserter(nd->Value));
								copy(p->Value.begin() + (limit >> 1) + 1, p->Value.end(), back_inserter(nd2->Value));
								if (i <= (limit >> 1)) {
									nd->Value.insert(nd->Value.end() - ((limit >> 1) + 1 - i), val);
								}
								else {
									nd2->Value.insert(nd2->Value.begin() + (i - (limit >> 1) - 1), val);
								}
								nd->Next = nd2;
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

								}//5
								break;
							}
						}


					}
					if (Prev->Next != nullptr) {
						Prev = p;
					}
				}
			}
		}
		void InvertedList::init2(std::string id, const std::vector<int>& val) {
			int store = 0;
			for (int k = 0; k < L.size(); k++) {
				if (L[k]->meno == id) {
					store = k;
					break;
				}
			}
			Node* node = new Node;
			L[store]->Head = L[store]->Tail = node;
			L[store]->Head->Value = val;
			L[store]->Head->Next = nullptr;
		}
		std::vector<int> InvertedList::sortV(std::string id, std::vector<int> val) {
			std::sort(val.begin(), val.end());
			return val;
		}
		bool InvertedList::isEmpty(std::string id) {
			int store = 0;
			for (int k = 0; k < L.size(); k++) {
				if (L[k]->meno == id) {
					store = k;
					break;
				}
			}
			int counter = 0;
			for (Node* p = L[store]->Tail; p != nullptr; p = p->Next)
				counter += 1;
			if (counter >= 1)
				return false;
			else
				return true;
		}
		void InvertedList::setList(std::string id, InputSet lst) {

			List* LS = new List;
			LS->meno = id;
			this->L.push_back(LS);
			hs.insertItem(id, LS);
			set(id, lst.getVec());
		}
		void InvertedList::set(std::string id, const std::vector<int>& value) {
			std::vector<int> val;
			int store = 0;
			for (int k = 0; k < L.size(); k++) {
				if (L[k]->meno == id) {
					store = k;
					break;
				}
			}
			L[store]->len += value.size();
			val = sortV(id, value);
			if (isEmpty(id) && val.size() <= limit)
				init2(id, val);
			else if (val.size() > limit) {
				int cnt = 0;
				int j1 = 0;
				for (int i = 0; i < ceil(double(val.size()) / double(limit)); i++) {
					Node* node = new Node;

					for (int j = 0; j < 5; j++) {
						if (j >= 0 && j < val.size() - cnt * limit) {
							node->Value.insert(node->Value.begin(), val[j1]);
						}
						j1++;
					}
					if (cnt == 0)
						node->Next = nullptr;
					else
						node->Next = L[store]->Tail;
					L[store]->Tail = node;
					cnt += 1;
					if (i == 0) {
						L[store]->Head = node;
					}
				}
			}

		}
		void InvertedList::reportH(std::string id)
		{
			List* lss = findID1(id);
			for (Node* p = lss->Tail; p != nullptr; p = p->Next)
			{
				int cnt = 0;
				for (int i = 0; i < p->Value.size(); i++) {
					if (i >= 0 && i < p->Value.size() - cnt * limit) {
						std::cout << p->Value[i] << "\n";
					}
				}
				cnt += 1;
			}
			std::cout << std::endl;
		}
		TEST_METHOD(reportH)
		{
			InputSet i;
			i.meno = "skil1";
			i.add(4);
			InvertedList* inv = new InvertedList();
			inv->setList("skil1", i);
			inv->insert("skil1", 12);
			inv->reportH("skil1");
			//List* lss = findID1("skil1");

		}
	};
}
