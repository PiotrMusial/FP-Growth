#ifndef Node_hpp
#define Node_hpp

#include <vector>
#include <string>
using namespace std;

class Node {
private:
    Node *up; // wskaŸnik na ojca
    vector<Node*> down; // wektor synów wierzcho³ka
    vector<string> path; // œcie¿ka wierzcho³ka
    int counter; // licznik
    string item; // id przedmiotu (przedmiot)
public:
    Node();
    bool isTheSameNode(Node *n);
    Node* getUp();
    void setUp(Node *up);
    vector<string> getPath();
    void addPath(string item);
    string getItem();
    void setItem(string item);
    vector<Node*> getDown();
    void addToDown(Node *n);
    void incCounter();
    int getCounter();
};

bool cmp2(Node *& n1, Node *& n2);

#endif // Node_hpp
