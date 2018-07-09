#include "Node.hpp"

Node::Node() {
    this->counter = 1;
}
bool Node::isTheSameNode(Node *n) {
    return n->item == this->item;
}
Node* Node::getUp() {
    return up;
}
void Node::setUp(Node *up) {
    this->up = up;
}
vector<string> Node::getPath() {
    return path;
}
void Node::addPath(string item) {
    path.push_back(item);
}
string Node::getItem() {
    return item;
}
void Node::setItem(string item) {
    this->item = item;
}
vector<Node*> Node::getDown() {
    return down;
}
void Node::addToDown(Node *n) {
    down.push_back(n);
}
void Node::incCounter() {
    this->counter++;
}
int Node::getCounter() {
    return counter;
}
bool cmp2(Node *& n1, Node *& n2) {
    return n1->getItem() < n2->getItem();
}
