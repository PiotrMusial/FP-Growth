#include "FPTree.hpp"

FPTree::FPTree() {

}

const vector<Node*> & FPTree::getHeaderTable() {
    return headerTable;
}

//od obecnego wskaŸnika idzie w górê, dopóki nie napotka nulla (korzenia), i dodaje wskaŸniki na wierzcho³ki
void FPTree::setPath(Node *&n, Node *&wsk) {
    while(n->getUp() != NULL) {
        wsk->addPath(n->getUp()->getItem());
        n = n->getUp();
    }
}

// dodaje wierzcho³ek do drzewa oraz do header table
void FPTree::addNodeToTreeAndHT(Node *&n, Node *&wsk) {
    n->setUp(wsk);
    n->addToDown(NULL);
    wsk->addToDown(n);
    wsk = n;
    headerTable.push_back(wsk);
}

// dodaje wierzcholek tylko do drzewa
void FPTree::addNodeToTree(Node *&n, Node *&wsk) {
    nodes.push_back(n);
    n->setUp(NULL);
    n->addToDown(NULL);
    wsk = n;
}

// sortuje header table po przedmiotach
void FPTree::sortHT() {
    sort(headerTable.begin(), headerTable.end(), cmp2);
}

void FPTree::insertNode(queue<string> &items) {
    Node *wsk = new Node(); // tymczasowy wskaŸnik

    for(int i = 0; !items.empty(); i++) {
        Node *n = new Node(); // nowy wierzcho³ek (przedmiot)
        n->setItem(items.front()); // ustawia wskaŸnik na przedmiot
        items.pop();

        if(i == 0) { // je¿eli podaje pierwszy przedmiot z transakcji
            if(nodes.size() == 0) { // je¿eli korzeñ (null) nie ma ¿adnych synów
                addNodeToTree(n, wsk);
            } else { // je¿eli ma synów
                bool is = false; // flaga czy jest taki sam syn
                for(int i = 0; i < nodes.size(); i++) { // pêtla po synach korzenia
                    if(n->isTheSameNode(nodes[i])) { // je¿eli jest taki sam
                        nodes[i]->incCounter(); // zwiekszenie licznika o 1
                        wsk = nodes[i];
                        is = true;
                        break;
                    }
                }
                if(!is) addNodeToTree(n, wsk); // je¿eli nie ma takiego syna
            }
        } else { // je¿eli podaje nie pierwszy przedmiot transakcji
            if(wsk->getDown().size() > 1) { // je¿eli wierzcho³ek ma synów
                bool is = false; // flaga czy jest taki sam syn
                for(int i = 1; i < wsk->getDown().size(); i++) {// pêtla po synach wierzcho³ka
                    if(n->isTheSameNode(wsk->getDown()[i])) { // je¿eli jest taki sam
                        wsk->getDown()[i]->incCounter(); // zwiêkszenie licznika o 1
                        wsk = wsk->getDown()[i];
                        is = true;
                        break;
                    }
                }
                if(!is) { // je¿eli nie ma takiego syna
                    addNodeToTreeAndHT(n, wsk); // dodaje
                    setPath(n, wsk); // ustala œcie¿kê
                }
            } else { // je¿eli wierzcho³ek nie ma synów
                addNodeToTreeAndHT(n, wsk); // dodaje
                setPath(n, wsk); // ustala œcie¿kê
            }
        }
    }
}

