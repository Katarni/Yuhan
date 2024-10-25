#pragma once
#include "includes.h"


class Trie {
public:
    Trie();

    ~Trie();

    void insert(char* word, size_t length);

    bool isInTrie(std::string& word);

private:
    class Node {
    public:
        Node();

        ~Node();

        [[nodiscard]]
        bool isTerminal() const;

        Node* next(char let);

        void addChild(char let);

        void setTerminal();

    private:
        std::map<char, Node*> go_;
        bool terminal_;
    };

    Node *root_;
};
