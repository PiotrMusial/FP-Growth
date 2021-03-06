#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <queue>
#include <time.h>
#include <cstdlib>

#include "StringTokenizer.hpp"
#include "Node.hpp"
#include "FPTree.hpp"

using namespace std;

#define MAP_SF map<string, float>
#define MAP_SF_IT map<string, float>::iterator
#define VEC_SF vector<pair<string, float > >
#define VEC_S vector<string>
#define VEC_S_2D vector<vector<string> >

clock_t start, stop;
double exeTime;

const float MIN_SUPP = 0.0144;
VEC_S dataBaseVector; // wektor zawieraj¹cy bazê danych, gdzie jednym elementem jest cała transakcja
VEC_SF frequentPatterns; // wszystkie zbiory częste
int numberOfTransactions = 0; // liczba transakcji w pliku

void display_map_sf(MAP_SF m) {
	MAP_SF_IT it;
	for (it = m.begin(); it != m.end(); it++)
		cout << it->first << "   " << it->second << endl;
	cout << endl;
}

void display_vec_sf(VEC_SF v) {
	for (int i = 0; i < v.size(); i++)
		cout << v[i].first << "      " << v[i].second << endl;
	cout << endl;
}

void display_vec_s(VEC_S v) {
	for (int i = 0; i < v.size(); i++)
		cout << v[i] << endl;
	cout << endl;
}

/*****
***** 1 krok - kompresja bazy danych
*****/

VEC_SF count_items_in_DB(ifstream &dataBaseFile) {
	MAP_SF itemCounted;
	MAP_SF_IT it;
	string item;
	string transaction;

	dataBaseFile.clear();
	dataBaseFile.seekg(0, ios::beg);

	while (getline(dataBaseFile, transaction)) {
		dataBaseVector.push_back(transaction); // dodanie transakcji do wektora bazy danych

		StringTokenizer st(transaction);
		VEC_S vecTransaction = st.getTokens();

		//dodawanie przedmiotow do mapy
		for (int i = 0; i < vecTransaction.size(); i++) {
			it = itemCounted.find(vecTransaction[i]);
			if (it != itemCounted.end())
				it->second++; // jeœli wystêpuje w mapie
			else
				itemCounted.insert(make_pair(vecTransaction[i], 1)); // jeœli nie wystêpuje w mapie
		}
		numberOfTransactions++;
	}

	//skopiowanie zawartoœci mapy do wektora par, aby by³o mo¿liwe posortowanie wed³ug wsparcia (supp)
	VEC_SF vecItemsCounted;
	copy(itemCounted.begin(), itemCounted.end(), back_inserter(vecItemsCounted));

	//cout << "WEKTOR Z BAZA DANYCH" << endl;
	//display_vec_s(dataBaseVector);

	return vecItemsCounted;
}

bool cmp(const pair<string, int>  &p1, const pair<string, int> &p2) {
	return p1.second > p2.second;
}

void sort_items_desc_by_support_and_erase(VEC_SF &itemCounted) {
	sort(itemCounted.begin(), itemCounted.end(), cmp); // posortowanie wektora po drugiej zmiennej (supp)

	for (int i = 0; i < itemCounted.size();)
		if (MIN_SUPP > (float)itemCounted[i].second / numberOfTransactions) // jeœli supp < MINSUPP
			itemCounted.erase(itemCounted.begin() + i); // usuniêcie przedmiotu z wektora
		else
			i++;
}

void sort_transaction_in_new_file(VEC_SF itemCounted) {
	fstream outputDB("newdb.txt", ios::out);
	if (!outputDB.good()) {
		cout << "Failure with opening newdb.txt";
		exit(0);
	}

	// posortowane przedmioty
	VEC_S itemsInOrder;
	for (int i = 0; i < itemCounted.size(); i++) {
		itemsInOrder.push_back(itemCounted[i].first);
	}

	// "sortowanie" przedmiotow w transakcjach
	for (int i = 0; i < dataBaseVector.size(); i++) {
		// podzielenie transakcji
		StringTokenizer st(dataBaseVector[i]);
		VEC_S vecTransaction = st.getTokens();

		VEC_S itemsInOrderTmp = itemsInOrder; // przedmioty, które będą usuwane ze wszystkich posortowanych przedmiotów

		// usuwa przedmiot z tymczasowego wektora posortowanych przedmiotów jeżeli nie występuje w transakcji
		for (int j = 0; j < itemsInOrderTmp.size();) {
			if (find(vecTransaction.begin(), vecTransaction.end(), itemsInOrderTmp[j]) != vecTransaction.end())
				j++;
			else
				itemsInOrderTmp.erase(itemsInOrderTmp.begin() + j);
		}

		//kopiowanie wektora do pliku
		copy(itemsInOrderTmp.begin(), itemsInOrderTmp.end(), ostream_iterator<string>(outputDB, " "));
		outputDB << endl;
	}

	dataBaseVector.erase(dataBaseVector.begin(), dataBaseVector.end());

	//cout << "Nowa baza danych: newdb.txt" << endl;
}

void compress_data_base(ifstream &dataBaseFile) {
	VEC_SF itemCounted = count_items_in_DB(dataBaseFile);
	//cout << "zliczone przedmioty" << endl;
	//cout << "ID    SUPP" << endl;
	//display_vec_sf(itemCounted); // zliczone przedmioty

	sort_items_desc_by_support_and_erase(itemCounted);
	//cout << "posortowane i usuniete przedmioty" << endl;
	//cout << "ID    SUPP" << endl;
	//display_vec_sf(itemCounted); // posortowane i usuniête przedmioty

	sort_transaction_in_new_file(itemCounted);
}

/*****
***** 2 krok - budowa FP-Tree
*****/

FPTree create_fp_tree_from_file(ifstream &dataBaseFile2) {
	//cout << "Tworzenie drzewa z pliku..." << endl;

	FPTree tree;
	string transaction;

	while (getline(dataBaseFile2, transaction)) {
		// każda transakcja jest dzielona na pojedyncze przedmioty
		StringTokenizer st(transaction);
		VEC_S vecTransaction = st.getTokens();
		queue<string> qTransactions;

		// i wrzucana do kolejki
		for (int i = 0; i < vecTransaction.size(); i++)
			qTransactions.push(vecTransaction[i]);

		tree.insertNode(qTransactions); // dodanie kolejki
	}

	tree.sortHT(); // posortowanie header table

				   //cout << "Drzewo zostalo utworzone!" << endl << endl;
	return tree;
}

FPTree create_fp_tree_from_vector(VEC_S dataBaseVector2) {
	//cout << "Tworzenie drzewa z wektora..." << endl;
	//display_vec_s(dataBaseVector2);

	FPTree tree;

	for (int i = 0; i < dataBaseVector2.size(); i++) {
		StringTokenizer st(dataBaseVector2[i]);
		VEC_S vecTransaction = st.getTokens();
		queue<string> qTransaction;

		for (int j = 0; j < vecTransaction.size(); j++)
			qTransaction.push(vecTransaction[j]);

		tree.insertNode(qTransaction);
	}

	//cout << "Drzewo zostalo utworzone!" << endl;
	tree.sortHT();
	return tree;
}

/*****
***** 3 krok - generacja wszystkich zbiorów częstych
*****/

// dodaje do wektora te same przedmioty, usuwa je z header table i zwraca wektor
vector<Node*> get_same_items(vector<Node*> &headerTable) {
	vector<Node*> nodes;

	if (headerTable.size() == 1) {
		nodes.push_back(headerTable[0]);
		headerTable.erase(headerTable.begin());
	}
	else {
		nodes.push_back(headerTable[0]);
		int i;
		for (i = 1; i < headerTable.size(); i++) {
			if (headerTable[i - 1]->getItem() == headerTable[i]->getItem())
				nodes.push_back(headerTable[i]);
			else
				break;
		}
		headerTable.erase(headerTable.begin(), headerTable.begin() + i);
	}

	//for(int i = 0; i < nodes.size(); i++)
	//cout << nodes[i]->getItem() << " ";
	//cout << endl;
	return nodes;
}

// dodaje ścieżki do wektora
VEC_S add_paths_to_vector(vector<Node*> nodes) {
	VEC_S paths;

	for (int i = 0; i < nodes.size(); i++) {
		VEC_S path = nodes[i]->getPath();
		string str = "";

		//dodaje przedmioty do stringa
		for (int j = 0; j < path.size(); j++)
			str += path[j] + " ";

		//dodaj tyle razy ile wynosi counter ścieżki
		for (int j = 0; j < nodes[i]->getCounter(); j++)
			paths.push_back(str);
	}
	return paths;
}

// zlicza przedmioty w wektorze
VEC_SF count_items_in_vector(VEC_S paths) {
	MAP_SF itemCounted; // zliczone przedmioty
	MAP_SF_IT it;
	string item; // przedmiot

				 // petla po wszystkich ścieżkach
	for (int i = 0; i < paths.size(); i++) {
		// podzielenie ścieżki
		StringTokenizer st(paths[i]);
		VEC_S vecTransaction = st.getTokens();

		//dodawanie przedmiotow do mapy
		for (int i = 0; i < vecTransaction.size(); i++) {
			it = itemCounted.find(vecTransaction[i]);
			if (it != itemCounted.end())
				it->second++; // jeœli wystêpuje w mapie
			else
				itemCounted.insert(make_pair(vecTransaction[i], 1)); // jeœli nie wystêpuje w mapie
		}
	}

	//skopiowanie zawartoœci mapy do wektora par, aby było możliwe posortowanie według wsparcia (supp)
	VEC_SF vecItemsCounted;
	copy(itemCounted.begin(), itemCounted.end(), back_inserter(vecItemsCounted));

	return vecItemsCounted;
}

// sortuje ścieżki (transakcje) w wektorze
VEC_S sort_transaction_in_new_vector(VEC_S paths, VEC_SF itemCounted) {
	// posortowane przedmioty
	VEC_S itemsInOrder;
	for (int i = 0; i < itemCounted.size(); i++)
		itemsInOrder.push_back(itemCounted[i].first);

	VEC_S sortedPaths; // posortowane ścieżki (transakcje)

					   // "sortowanie" przedmiotow w ściezkach (transakcjach)
	for (int i = 0; i < paths.size(); i++) {
		// podzielenie ścieżki (transakcji)
		StringTokenizer st(paths[i]);
		VEC_S vecTransaction = st.getTokens();

		VEC_S itemsInOrderTmp = itemsInOrder; // przedmioty, które będą usuwane ze wszystkich posortowanych przedmiotów

											  // usuwa przedmiot z tymczasowego wektora posortowanych przedmiotów jeżeli nie występuje w ścieżce (transakcji)
		for (int j = 0; j < itemsInOrderTmp.size();) {
			if (find(vecTransaction.begin(), vecTransaction.end(), itemsInOrderTmp[j]) != vecTransaction.end())
				j++;
			else
				itemsInOrderTmp.erase(itemsInOrderTmp.begin() + j);
		}

		// dodanie nowych ścieżek (transakcji) do nowego wektora
		string tmp = "";
		for (int j = 0; j < itemsInOrderTmp.size(); j++)
			tmp += itemsInOrderTmp[j] + " ";

		sortedPaths.push_back(tmp);
	}
	return sortedPaths;
}

// tworzy dwuelementowe zbiory częste
void add_frequent_patterns(string prefix, VEC_SF itemsCounted) {
	for (int i = 0; i < itemsCounted.size(); i++) {
		string frequentPattern = prefix + itemsCounted[i].first;
		frequentPatterns.push_back(make_pair(frequentPattern, itemsCounted[i].second / numberOfTransactions));
	}
}

void delete_last_prefix(string &prefix) {
	StringTokenizer st(prefix);
	VEC_S vecPrefix = st.getTokens();
	vecPrefix.pop_back();

	stringstream ss;
	for (int i = 0; i < vecPrefix.size(); i++) {
		if (i != 0)
			ss << " ";
		ss << vecPrefix[i];
	}
	prefix = ss.str();
	prefix += " ";

}

// główna funckja tworzenia zbiorów częstych
void generate_frequent_patterns(vector<Node*> &headerTable, string prefix, bool recursion) {
	for (int i = 0; !headerTable.empty(); i++) {
		vector<Node*> nodes = get_same_items(headerTable);
		VEC_S paths = add_paths_to_vector(nodes);

		if (recursion)
			prefix += nodes[0]->getItem() + " ";
		else
			prefix = nodes[0]->getItem() + " ";

		//cout << "PREFIX: " << prefix << endl;
		//cout << "HEADER = ITEM: " << nodes[0]->getItem() << endl;
		//cout << "PATHS" << endl;
		//display_vec_s(paths);

		VEC_SF itemsCounted = count_items_in_vector(paths);
		sort_items_desc_by_support_and_erase(itemsCounted);

		if (itemsCounted.size() > 1) {
			VEC_S sortedItems = sort_transaction_in_new_vector(paths, itemsCounted);

			//cout << "DONE" << endl;
			//display_vec_s(sortedItems);

			FPTree tree2 = create_fp_tree_from_vector(sortedItems);
			vector<Node*> headerTable2 = tree2.getHeaderTable();
			bool recursion = true;
			generate_frequent_patterns(headerTable2, prefix, recursion);

		}
		add_frequent_patterns(prefix, itemsCounted);
		delete_last_prefix(prefix);

		//cout << "FREQUENT PATTERNS" << endl;
		//display_vec_sf(frequentPatterns);
	}
}

/*****
***** 4 krok - generacja wszystkich reguł asocjacyjnych
*****/

// zliczanie przedmiotów w zbiorze
int count_number_of_items_in_pattern(string pattern) {
	int counter = 0;
	istringstream ss(pattern);
	int num;
	while (ss >> num)
		counter++;
	return counter;
}

// generazja wszystkich podzbiorów zbioru
VEC_S_2D generate_all_subsets(string oneSet) {
	StringTokenizer st(oneSet);
	VEC_S pattern = st.getTokens();
	int t = pattern.size();
	int n = t;
	int numberOfItemsInSet = count(oneSet.begin(), oneSet.end(), ' ') + 1;
	VEC_S_2D subsets(numberOfItemsInSet - 1);
	string str;

	for (int i = 1; i < (1 << n) - 1; i++) {
		str = "";
		for (int j = 0; j < n; j++) {
			if (i & (1 << j))
				str = str + pattern[j] + " ";
		}
		int length = count_number_of_items_in_pattern(str);
		subsets[length - 1].push_back(str);
	}
	return subsets;
}

// sprawdza czy dany pozdzbiór występuje w drugim podzbiorze
bool is_present(string str1, string str2) {
	StringTokenizer st1(str1);
	StringTokenizer st2(str2);
	VEC_S tmp1 = st1.getTokens();
	VEC_S tmp2 = st2.getTokens();
	bool found;

	for (int i = 0; i < tmp1.size(); i++)
		if (find(tmp2.begin(), tmp2.end(), tmp1[i]) != tmp2.end())
			return true;
		else
			found = false;

	return found;
}

// generuje reguly asocjacyjne z dwuelementowych zbiorów
void generate_of_two(string pattern, float support, int &counter, ofstream &rules) {
	StringTokenizer st(pattern);
	VEC_S vecPattern = st.getTokens();

	cout << vecPattern[0] << " => " << vecPattern[1] << "  supp: " << support << endl;
	cout << vecPattern[1] << " => " << vecPattern[0] << "  supp: " << support << endl;
	rules << vecPattern[0] << " => " << vecPattern[1] << endl;// << "  supp: " << support << endl;
	rules << vecPattern[1] << " => " << vecPattern[0] << endl;// << "  supp: " << support << endl;

	counter += 2;
}

// generuje reguły asocjacyjne ze zbiorów o parzystej liczbie przedmiotów (oprócz ziorów dwuelementowych)
void generate_of_even(string pattern, float support, int &counter, ofstream &rules) {
	VEC_S_2D allSubsets = generate_all_subsets(pattern);

	for (int i = 0; i < allSubsets.size(); i++) {
		if (i == allSubsets.size() - 1)
			break;

		if (i != allSubsets.size() / 2) {
			VEC_S first = allSubsets[i];
			VEC_S last = allSubsets[allSubsets.size() - 1 - i];

			for (int j = 0; j < first.size(); j++) {
				for (int k = 0; k < last.size(); k++) {
					if (!is_present(first[j], last[k])) {
						cout << first[j] << " => " << last[k] << " supp: " << support << endl;
						cout << last[k] << " => " << first[j] << " supp: " << support << endl;
						rules << first[j] << " => " << last[k] << endl;// << " supp: " << support << endl;
						rules << last[k] << " => " << first[j] << endl;// << " supp: " << support << endl;
						counter += 2;
					}
				}
			}
		}
		else {
			VEC_S mid = allSubsets[i];

			for (int i = 0; i < mid.size() - 1; i++) {
				for (int j = i + 1; j < mid.size(); j++) {
					if (!is_present(mid[i], mid[j])) {
						cout << mid[i] << " => " << mid[j] << " supp: " << support << endl;
						cout << mid[j] << " => " << mid[i] << " supp: " << support << endl;
						rules << mid[i] << " => " << mid[j] << endl;// << " supp: " << support << endl;
						rules << mid[j] << " => " << mid[i] << endl;// << " supp: " << support << endl;
						counter += 2;
					}
				}
			}
		}
	}
}

// generuje reguły asocjacyjne ze zbiorów o nieparzystej liczbie przedmiotów
void generate_of_odd(string pattern, float support, int &counter, ofstream &rules) {
	VEC_S_2D allSubsets = generate_all_subsets(pattern);

	for (int i = 0; i < allSubsets.size(); i++) {
		VEC_S first = allSubsets[i];
		VEC_S last = allSubsets[allSubsets.size() - 1 - i];

		if (i == allSubsets.size() - i)
			break;

		for (int j = 0; j < first.size(); j++) {
			for (int k = 0; k < last.size(); k++) {
				if (!is_present(first[j], last[k])) {
					cout << first[j] << " => " << last[k] << " supp: " << support << endl;
					cout << last[k] << " => " << first[j] << " supp: " << support << endl;
					rules << first[j] << " => " << last[k] << endl;// << " supp: " << support << endl;
					rules << last[k] << " => " << first[j] << endl;// << " supp: " << support << endl;
					counter += 2;
				}
			}
		}
	}
}

// głowna funkcja do generowania reguł asocjacyjnych
void generate_all_association_rules() {
	ofstream rules;
	rules.open("rules.txt");
	if (!rules.good()) {
		cout << "bad rules.txt" << endl;
		exit(1);
	}

	int counter = 0;

	for (int i = 0; i < frequentPatterns.size(); i++) {
		string pattern = frequentPatterns[i].first;
		float support = frequentPatterns[i].second;
		int numberOfItems = count_number_of_items_in_pattern(pattern);

		if (numberOfItems == 2) {
			generate_of_two(pattern, support, counter, rules);
		}
		else if (numberOfItems % 2 == 0) {
			generate_of_even(pattern, support, counter, rules);
		}
		else if (numberOfItems % 2 == 1) {
			generate_of_odd(pattern, support, counter, rules);
		}
	}

	cout << "Lczba wszystkich zbiorow czestych: " << frequentPatterns.size() << endl;
	cout << "Liczba wszystkich regul asocjacyjnych: " << counter << endl;

}

void fp_growth(ifstream &dataBaseFile, ifstream &dataBaseFile2) {
	start = clock();

	compress_data_base(dataBaseFile);
	FPTree tree = create_fp_tree_from_file(dataBaseFile2);
	vector<Node*> headerTable = tree.getHeaderTable();
	generate_frequent_patterns(headerTable, "", false);
	display_vec_sf(frequentPatterns);
	generate_all_association_rules();

	stop = clock();
	exeTime = (double)(stop - start) / CLOCKS_PER_SEC;
	cout << "Execution time: " << exeTime << " seconds." << endl;
}

int main() {
	ifstream dataBaseFile;
	dataBaseFile.open("F:/Moje Dokumenty/E/Praca licencjacka/BAZY/TABELA/db3.txt", ios::in);
	if (!dataBaseFile.good()) {
		cout << "Failure while opening file";
		exit(1);
	}

	ifstream dataBaseFile2;
	dataBaseFile2.open("newdb.txt", ios::in);
	if (!dataBaseFile2.good()) {
		cout << "Failure while opening fileeeee";
		exit(1);
	}

	fp_growth(dataBaseFile, dataBaseFile2);

	dataBaseFile.close();
	dataBaseFile2.close();
}
