#pragma once

/*!
    \file
    \brief Описание нод и боров
    \author Алена Иванова

    Здесь описываются базовая и дочерние ноды (Node, VariableNode, StructNode, FunctionNode),
    базовый бор и его дочерние классы
*/

#include "includes.h"
#include "ReservedMemory.h"


class not_found_error : public std::runtime_error {
public:
    not_found_error() : std::runtime_error("Identifier not found") {}
};


/*!
    \brief Базовая нода
    \author Алена Иванова

    Базовая нода для боров
*/
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
    std::map<char, Node*> go_; ///< КЧД доступных переходов
    bool terminal_;
};


/*!
    \brief Нода для переменной
    \author Алена Иванова

    Хранит данные переменной, используется в TIDVariable
*/
class VariableNode : public Node {
public:
    VariableNode() {}

    [[nodiscard]]
    Type getType();
    Identifier getVar() const;

    void setType(Type& type);
    void setId(std::string id);
    void setFields(const std::vector<std::pair<std::string, Type>>& fields);

    void addChild(char let) override;

private:
    Identifier var_;
};


/*!
    \brief Базовый бор
    \author Алена Иванова

    Бор с шаблонной нодой, от него наследуются все кастомные боры
*/
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

    bool isInTrie(const std::string &word) {
        T *ptr = root_;
        for (auto& let : word) {
            ptr = static_cast<T*>(ptr->next(let));
            if (ptr == nullptr) return false;
        }
        return ptr->isTerminal();
    }

    T *getNode(const std::string& word) {
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


/*!
    \brief Бор для переменных
    \author Алена Иванова

    Наследник Trie, построен от VariableNode, хранит в себе переменные
*/
class TIDVariable : public Trie<VariableNode> {
public:
    Identifier checkID(std::string& name);

    void pushID(std::string& name, Type& type, const std::vector<std::pair<std::string, Type>>& fields = {},
                std::string id = "");
};


/*!
    \brief Нода для структур
    \author Алена Иванова

    Хранит данные структур, используется в TIDStructure
*/
class StructureNode : public Node {
public:
    Type checkIDField(std::string& name);

    void pushIDField(std::string& name, Type& type);

    void addChild(char let);

    [[nodiscard]]
    std::vector<std::pair<std::string, Type>> getAllVarsWithName() const;

 private:
    TIDVariable fields_;
    std::vector<std::pair<std::string, Type>> name_fields_;
};


/*!
    \brief Бор для структур
    \author Алена Иванова

    Наследник Trie, построен от StructureNode, хранит в себе описанные структуры
*/
class TIDStructure : public Trie<StructureNode> {
public:
    bool checkId(const std::string& name);

    void pushID(std::string& name);

    Type checkField(std::string& name, std::string& name_field);

    void pushField(std::string& name, std::string& name_field, Type& type_field);

    [[nodiscard]]
    std::vector<std::pair<std::string, Type>> getAllFieldsByName(const std::string& name);
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


/*!
    \brief Нода для функций
    \author Алена Иванова

    Хранит функции, используется в TIDFunction
*/
class FunctionNode : public Node {
public:
    Type getType();
    const std::string& getId();

    void setType(Type& type);
    void setId(const std::string& id);

    void setArgs(std::vector<Variable>& args);

    void checkArgs(std::vector<Type>& args);

    void addChild(char let);

private:
    std::string id_;
    Type type_;
    std::vector<Variable> args_;
};


/*!
    \brief Бор для функций
    \author Алена Иванова

    Наследник Trie, построен от FunctionNode, хранит в себе функции
*/
class TIDFunction : public Trie<FunctionNode> {
public:
    Type checkID(std::string& name, std::vector<Type>& args);
    std::string checkIDName(std::string& name, std::vector<Type>& args);

    Type checkID(std::string& name);

    void pushID(std::string& name, Type& type, std::vector<Variable>& args, std::string id);
};
