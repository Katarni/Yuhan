#include "Trie.h"

bool Trie::Node::isTerminal() const {
    return terminal_;
}

Trie::Node *Trie::Node::next(char let) {
    return go_[let];
}

void Trie::Node::addChild(char let) {
    if (go_[let] != nullptr) return;
    go_[let] = new Node();
}

void Trie::Node::setTerminal() {
    terminal_ = true;
}

Trie::Node::~Node() {
    for (auto& [let, to] : go_) {
        delete to;
        to = nullptr;
    }
}

Trie::Node::Node() : terminal_(false) {}

void Trie::goNext(char let) {
    if (cur_ptr_ == nullptr) return;
    cur_ptr_ = cur_ptr_->next(let);
}

bool Trie::isTerminal() {
    if (cur_ptr_ == nullptr) return false;
    return cur_ptr_->isTerminal();
}

void Trie::goToRoot() {
    cur_ptr_ = root_;
}

Trie::Trie() {
    root_ = new Node();
    cur_ptr_ = root_;
}

Trie::~Trie() {
    delete root_;
}

void Trie::insert(char *word, size_t length) {
    Node *ptr = root_;
    for (int i = 0; i < length; ++i, ++word) {
        char let = *word;
        ptr->addChild(let);
        ptr = ptr->next(let);
    }
    ptr->setTerminal();
}

bool Trie::isInTrie(std::string &word) {
    Node *ptr = root_;
    for (auto& let : word) {
        ptr = ptr->next(let);
        if (ptr == nullptr) return false;
    }
    return ptr->isTerminal();
}
