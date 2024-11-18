#pragma once
#include "includes.h"
#include "ReservedMemory.h"


class not_found_error : public std::runtime_error {
public:
    not_found_error() : std::runtime_error("Identifier not found") {}
};


class Node {
public:
    Node();

    ~Node();

    [[nodiscard]]
    bool isTerminal() const;

    Node* next(char let);

    virtual void addChild(char let);

    void setTerminal();

protected:
    std::map<char, Node*> go_;
    bool terminal_;
};

class VariableNode : public Node {
public:
    VariableNode() : var_(nullptr) {}

    [[nodiscard]]
    Type getType() const;
    ReservedMemory* getVar() const;

    void setType(Type& type);

    void addChild(char let) override;

private:
    ReservedMemory *var_;
};

template <typename T>
class Trie {
public:
    Trie() {
        root_ = new T();
    }

    ~Trie() {
        delete root_;
    }

    void insert(char *word, size_t length) {
        T *ptr = root_;
        for (int i = 0; i < length; ++i, ++word) {
            char let = *word;
            ptr->addChild(let);
            ptr = ptr->next(let);
        }
        ptr->setTerminal();
    }

    T* insert(std::string &word) {
        T *ptr = root_;
        size_t length = word.size();
        for (int i = 0; i < length; ++i) {
            char let = word[i];
            ptr->addChild(let);
            ptr = static_cast<T *>(ptr->next(let));
        }
        ptr->setTerminal();
        return ptr;
    }

    bool isInTrie(std::string &word) {
        T *ptr = root_;
        for (auto& let : word) {
            ptr = static_cast<T*>(ptr->next(let));
            if (ptr == nullptr) return false;
        }
        return ptr->isTerminal();
    }

    T *getNode(std::string& word) {
        T *ptr = root_;
        for (auto& let : word) {
            ptr = static_cast<T*>(ptr->next(let));
            if (ptr == nullptr) throw not_found_error();
        }
        return ptr;
    }

protected:
    T *root_;
};


class TIDVariable : public Trie<VariableNode> {
public:
    ReservedMemory* checkID(std::string& name);

    void pushID(std::string& name, Type& type);
};

class StructureNode : public Node {
public:
    ReservedMemory* checkIDField(std::string& name);

    void pushIDField(std::string& name, Type& type);

    void addChild(char let);

private:
    TIDVariable fields_;
};


class TIDStructure : public Trie<StructureNode> {
public:
    bool checkId(std::string& name);

    void pushID(std::string& name);

    ReservedMemory* checkField(std::string& name, std::string& name_field);

    void pushField(std::string& name, std::string& name_field, Type& type_field);
};

class Variable {
public:
    Variable(std::string name, Type type);

    void setName(std::string& name);

    void setType(Type& type);

    Type getType();

    std::string getName();

private:
    std::string name_;
    Type type_;
};

class FunctionNode : public Node {
public:
    Type getType();

    void setType(Type& type);

    void setArgs(std::vector<Variable>& args);

    void checkArgs(std::vector<Type>& args);

    void addChild(char let);

private:
    Type type_;
    std::vector<Variable> args_;
};

class TIDFunction : public Trie<FunctionNode> {
public:
    Type checkID(std::string& name, std::vector<Type>& args);

    Type checkID(std::string& name);

    void pushID(std::string& name, Type& type, std::vector<Variable>& args);
};
