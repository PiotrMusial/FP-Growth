#ifndef FPTree_hpp
#define FPTree_hpp

#include <vector>
#include <queue>
#include <algorithm>
#include "Node.hpp"

class FPTree {
private:
    Node *root; // null
    vector<Node*> nodes; // wierzcho³ki od nulla
    vector<Node*> headerTable; // wektor wszystkich wierzcholkow o innych adresach
public:
	FPTree();
    const vector<Node*> & getHeaderTable();
    //od obecnego wskaŸnika idzie w górê, dopóki nie napotka nulla (korzenia), i dodaje wskaŸniki na wierzcho³ki
    void setPath(Node *&n, Node *&wsk);
    // dodaje wierzcho³ek do drzewa oraz do header table
    void addNodeToTreeAndHT(Node *&n, Node *&wsk);
    // dodaje wierzcholek tylko do drzewa
    void addNodeToTree(Node *&n, Node *&wsk);
    // sortuje header table po przedmiotach
    void sortHT();
    void insertNode(queue<string> &items);
};

#endif // FPTree_hpp
