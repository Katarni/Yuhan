#pragma once

#include "Trie.h"


enum class TypeScope {
    Namespace, Struct, Function, Loop, Switch, SwitchItem, Other
};

class TIDTree {
public:
    TIDTree();

    ~TIDTree();

    void closeScope();

    void createScope(TypeScope type = TypeScope::Other, std::string namespace_ = "");

    Type checkVariable(std::string name);

    void pushVariable(std::string name, Type type);

    bool checkStruct(std::string name);

    void pushStruct(std::string name);

    Type checkField(std::string name, std::string name_field);

    Type checkFunction(std::string name, std::vector<Type>& args);

    void pushFunction(std::string& name, Type& type, std::vector<Variable>& args);

    void checkReturn(Type type_return);

    void checkBreak();

    void checkContinue();

private:
    class NodeTID {
    public:
        NodeTID(TypeScope type, std::string& namespace_, NodeTID *parent);

        ~NodeTID();

        Type checkID(std::string &name);

        void pushID(std::string &name, Type &type);

        Type checkFieldOfStruct(std::string &name, std::string &field_name);

        bool checkStruct(std::string& name);

        void pushStruct(std::string& name);

        void pushFieldOfStruct(std::string& name, std::string& name_field, Type& type_field);

        Type checkFunction(std::string& name, std::vector<Type>& args);

        Type checkFunction(std::string& name);

        void pushFunction(std::string& name, Type& type, std::vector<Variable>& args);

        NodeTID *getParent();

        [[nodiscard]]
        bool isNamespace() const;

        [[nodiscard]]
        bool isStruct() const;

        const std::string& getNamespace() const;

        void addChild(NodeTID *child);

        TypeScope getType() const;

    private:
        TypeScope type_;
        std::string namespace_;
        TIDVariable variables_;
        TIDFunction functions_;
        TIDStructure structs_;
        NodeTID *parent_;
        std::vector<NodeTID*> children_;
    };
    NodeTID *current_scope_;

    Type checkVariable(NodeTID *ptr, std::string& name);

    bool checkStruct(NodeTID *ptr, std::string& name);

    Type checkFunction(NodeTID *ptr, std::string& name, std::vector<Type>& args);

    void pushField(TIDTree::NodeTID *ptr, std::string& name, std::string &name_field, Type& type_field);

    void pushVariable(NodeTID *ptr, std::string& name, Type& type);

    void pushStruct(NodeTID *ptr, std::string& name);

    void pushFunction(NodeTID *ptr, std::string& name, Type& type, std::vector<Variable>& args);

    Type checkField(NodeTID *ptr, std::string& name, std::string& name_field);

    void checkReturn(NodeTID *ptr, Type type_return);

    void checkBreak(NodeTID *ptr);

    void checkContinue(NodeTID *ptr);
};
