#pragma once
#include "includes.h"


class Node {
public:
    Node();

    ~Node();

    [[nodiscard]]
    bool isTerminal() const;

    Node* next(char let);

    void addChild(char let);

    void setTerminal();

protected:
    std::map<char, Node*> go_;
    bool terminal_;
};


class Type {
public:
    bool operator==(const Type& other) const;

    bool operator!=(const Type& other) const;

private:
    std::string name_;
    size_t size_array_;
    Type* array_type_;
    bool is_lvalue_;
};

class VariableNode : public Node {
public:
    [[nodiscard]]
    Type getType() const;

    void setType(Type& type);

private:
    std::string name_;
    Type type_;
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
            if (ptr == nullptr) throw std::runtime_error("Нет имени в боре");
        }
        return ptr;
    }

protected:
    T *root_;
};


class TIDVariable : public Trie<VariableNode> {
public:
    Type checkID(std::string& name);

    void pushID(std::string& name, Type& type);
};

class StructureNode : public Node {
public:
    Type checkIDField(std::string& name);

    void pushIDField(std::string& name, Type& type);

private:
    TIDVariable fields_;
};


class TIDStructure : public Trie<StructureNode> {
public:
    bool checkId(std::string& name);

    void pushID(std::string& name);

    Type checkField(std::string& name, std::string& name_field);

    void pushField(std::string& name, std::string& name_field, Type& type_field);
};

class Variable {
public:
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

    void checkArgs(std::vector<Variable>& args);

private:
    Type type_;
    std::vector<Variable> args_;
};

class TIDFunction : public Trie<FunctionNode> {
public:
    Type checkID(std::string& name, std::vector<Variable>& args);

    void pushID(std::string& name, Type& type, std::vector<Variable>& args);
};
