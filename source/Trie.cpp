#include "../include/Trie.h"

bool Node::isTerminal() const {
    return terminal_;
}

Node *Node::next(char let) {
    return go_[let];
}

void Node::addChild(char let) {
    if (go_[let] != nullptr) return;
    go_[let] = new Node();
}

void Node::setTerminal() {
    terminal_ = true;
}

Node::~Node() {
    for (auto& [let, to] : go_) {
        delete to;
        to = nullptr;
    }
}

Node::Node() : terminal_(false) {}

Type TIDVariable::checkID(std::string &name) {
    return getNode(name)->getType();
}

void TIDVariable::pushID(std::string &name, Type& type) {
    if (isInTrie(name)) throw std::runtime_error("Переменная с таким именем есть");
    auto ptr = insert(name);
    ptr->setType(type);
}

void VariableNode::setType(Type &type) {
   type_ = type;
}

Type VariableNode::getType() const {
    return type_;
}

bool TIDStructure::checkId(std::string &name) {
    return isInTrie(name);
}

Type TIDStructure::checkField(std::string& name, std::string& name_field) {
    return getNode(name)->checkIDField(name_field);
}

void TIDStructure::pushField(std::string &name, std::string &name_field, Type &type_field) {
    getNode(name)->pushIDField(name_field, type_field);
}

void TIDStructure::pushID(std::string &name) {
    if (isInTrie(name)) throw std::runtime_error("Такая структура уже есть");
    insert(name);
}

Type StructureNode::checkIDField(std::string &name) {
    return fields_.checkID(name);
}

void StructureNode::pushIDField(std::string &name, Type& type) {
    fields_.pushID(name, type);
}

void Variable::setName(std::string& name) {
    name_ = name;
}

void Variable::setType(Type &type) {
    type_ = type;
}

Type Variable::getType() {
    return type_;
}

std::string Variable::getName() {
    return name_;
}

Type FunctionNode::getType() {
    return type_;
}

void FunctionNode::setType(Type &type) {
    type_ = type;
}

void FunctionNode::setArgs(std::vector<Variable> &args) {
    args_ = args;
}

void FunctionNode::checkArgs(std::vector<Variable> &args) {
    if (args.size() != args_.size()) {
        throw std::runtime_error("Неправильное количество аргументов");
    }
    for (int i = 0; i < args_.size(); ++i) {
        if (args[i].getType() != args_[i].getType()) {
            throw std::runtime_error("Неправильный тип аргумента");
        }
    }
}

bool Type::operator==(const Type &other) const {
    return name_ == other.name_ && is_lvalue_ == other.is_lvalue_;
}

bool Type::operator!=(const Type &other) const {
    return !(operator==(other));
}
