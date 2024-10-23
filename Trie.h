#pragma once
#include "includes.h"


class Trie {
public:
    Trie();

    ~Trie();

    void insert(char* word, size_t length);

    bool isInTrie(std::string& word);

    void goNext(char let);

    bool isTerminal();

    void goToRoot();

private:
    class Node {
    public:
        Node();

        ~Node();

        bool isTerminal() const;

        Node* next(char let);

        void addChild(char let);

        void setTerminal();

    private:
        std::map<char, Node*> go_;
        bool terminal_;
    };

    Node *root_, *cur_ptr_;
};
