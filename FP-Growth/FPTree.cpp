#include "stdafx.h"
#include "FPTree.hpp"

FPTree::FPTree() {

}

const vector<Node*> & FPTree::getHeaderTable() {
    return headerTable;
}

//od obecnego wska�nika idzie w g�r�, dop�ki nie napotka nulla (korzenia), i dodaje wska�niki na wierzcho�ki
void FPTree::setPath(Node *&n, Node *&wsk) {
    while(n->getUp() != NULL) {
        wsk->addPath(n->getUp()->getItem());
        n = n->getUp();
    }
}

// dodaje wierzcho�ek do drzewa oraz do header table
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
    Node *wsk = new Node(); // tymczasowy wska�nik

    for(int i = 0; !items.empty(); i++) {
        Node *n = new Node(); // nowy wierzcho�ek (przedmiot)
        n->setItem(items.front()); // ustawia wska�nik na przedmiot
        items.pop();

        if(i == 0) { // je�eli podaje pierwszy przedmiot z transakcji
            if(nodes.size() == 0) { // je�eli korze� (null) nie ma �adnych syn�w
                addNodeToTree(n, wsk);
            } else { // je�eli ma syn�w
                bool is = false; // flaga czy jest taki sam syn
                for(int i = 0; i < nodes.size(); i++) { // p�tla po synach korzenia
                    if(n->isTheSameNode(nodes[i])) { // je�eli jest taki sam
                        nodes[i]->incCounter(); // zwiekszenie licznika o 1
                        wsk = nodes[i];
                        is = true;
                        break;
                    }
                }
                if(!is) addNodeToTree(n, wsk); // je�eli nie ma takiego syna
            }
        } else { // je�eli podaje nie pierwszy przedmiot transakcji
            if(wsk->getDown().size() > 1) { // je�eli wierzcho�ek ma syn�w
                bool is = false; // flaga czy jest taki sam syn
                for(int i = 1; i < wsk->getDown().size(); i++) {// p�tla po synach wierzcho�ka
                    if(n->isTheSameNode(wsk->getDown()[i])) { // je�eli jest taki sam
                        wsk->getDown()[i]->incCounter(); // zwi�kszenie licznika o 1
                        wsk = wsk->getDown()[i];
                        is = true;
                        break;
                    }
                }
                if(!is) { // je�eli nie ma takiego syna
                    addNodeToTreeAndHT(n, wsk); // dodaje
                    setPath(n, wsk); // ustala �cie�k�
                }
            } else { // je�eli wierzcho�ek nie ma syn�w
                addNodeToTreeAndHT(n, wsk); // dodaje
                setPath(n, wsk); // ustala �cie�k�
            }
        }
    }
}

