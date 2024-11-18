#include <utility>

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
    for (auto &[let, to]: go_) {
        delete to;
        to = nullptr;
    }
}

Node::Node() : terminal_(false) {}

ReservedMemory* TIDVariable::checkID(std::string &name) {
    return getNode(name)->getVar();
}

void TIDVariable::pushID(std::string &name, Type &type, const std::vector<std::pair<std::string, Type>>& fields) {
    if (isInTrie(name)) throw std::runtime_error("Variable with this name has already been created");
    auto ptr = insert(name);
    ptr->setType(type);

    if (!fields.empty()) {
        ptr->getVar()->setFields(fields);
    }
}

void VariableNode::setType(Type &type) {
    var_ = new ReservedMemory(type);
}

Type VariableNode::getType() const {
    return var_->getType();
}

ReservedMemory* VariableNode::getVar() const {
    return var_;
}

void VariableNode::addChild(char let) {
    if (go_[let] != nullptr) return;
    go_[let] = new VariableNode();
}

bool TIDStructure::checkId(std::string &name) {
    return isInTrie(name);
}

Type TIDStructure::checkField(std::string &name, std::string &name_field) {
    return getNode(name)->checkIDField(name_field);
}

void TIDStructure::pushField(std::string &name, std::string &name_field, Type &type_field) {
    getNode(name)->pushIDField(name_field, type_field);
}

void TIDStructure::pushID(std::string &name) {
    if (isInTrie(name)) throw std::runtime_error("Struct with this name has already been created");
    insert(name);
}

Type StructureNode::checkIDField(std::string &name) {
    return fields_.checkID(name)->getType();
}

void StructureNode::pushIDField(std::string &name, Type &type) {
    fields_.pushID(name, type);
}

void StructureNode::addChild(char let) {
    if (go_[let] != nullptr) return;
    go_[let] = new StructureNode();
}

void Variable::setName(std::string &name) {
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

Variable::Variable(std::string name, Type type) : name_(std::move(name)), type_(type) {}

Type FunctionNode::getType() {
    return type_;
}

void FunctionNode::setType(Type &type) {
    type_ = type;
}

void FunctionNode::setArgs(std::vector<Variable> &args) {
    args_ = args;
}

void FunctionNode::checkArgs(std::vector<Type> &args) {
    if (args.size() != args_.size()) {
        throw std::runtime_error("Incorrect number of arguments");
    }
    for (int i = 0; i < args_.size(); ++i) {
        if (args_[i].getType().isLvalue() && ! args[i].isLvalue()) {
            throw std::runtime_error("Incorrect category of argument " + args_[i].getName());
        }
        if (!args[i].compareWithCast(args_[i].getType())) {
            throw std::runtime_error("Incorrect argument type");
        }
    }
}

void FunctionNode::addChild(char let) {
    if (go_[let] != nullptr) return;
    go_[let] = new FunctionNode();
}

bool Type::operator==(const Type &other) const {
    if (is_lvalue_ != other.is_lvalue_) return false;
    if (name_ != other.name_) return false;
    if (name_ != "array") return true;
    if (size_array_ != other.size_array_) return false;
    return *array_type_ == *other.array_type_;
}

bool Type::compareNoLvalue(const Type &other) const {
    if (name_ != other.name_) return false;
    if (name_ != "array") return true;
    if (size_array_ != other.size_array_) return false;
    return *array_type_ == *other.array_type_;
}

bool Type::operator!=(const Type &other) const {
    return !(operator==(other));
}

Type TIDFunction::checkID(std::string &name, std::vector<Type> &args) {
    auto ptr = getNode(name);
    ptr->checkArgs(args);
    return ptr->getType();
}

void TIDFunction::pushID(std::string &name, Type &type, std::vector<Variable> &args) {
    if (isInTrie(name)) throw std::runtime_error("Function with this name has already been created");
    auto ptr = insert(name);
    ptr->setType(type);
    ptr->setArgs(args);
}

Type TIDFunction::checkID(std::string &name) {
    if (!isInTrie(name)) throw not_found_error();
    return getNode(name)->getType();
}

std::vector<std::pair<std::string, Type>> TIDStructure::getAllFieldsByName(const std::string& name) const {
    StructureNode *ptr = root_;
    for (auto& let : name) {
        ptr = dynamic_cast<StructureNode*>(ptr->next(let));
        if (ptr == nullptr) throw not_found_error();
    }

    if (!ptr->isTerminal()) throw not_found_error();

    return ptr->getAllVarsWithName();
}

std::vector<std::pair<std::string, Type>> StructureNode::getAllVarsWithName() const {
    return fields_.getAllVarsWithName();
}

std::vector<std::pair<std::string, Type>> TIDVariable::getAllVarsWithName() const {
    std::vector<std::pair<std::string, Type>> vars;
    root_->getAllVars(vars, "");
    return vars;
}

void VariableNode::getAllVars(std::vector<std::pair<std::string, Type>> &vars, std::string cur) {
    if (isTerminal()) {
        vars.emplace_back(cur, getType());
    }

    for (auto [key, val] : go_) {
        if (val == nullptr) continue;

        dynamic_cast<VariableNode*>(val)->getAllVars(vars, cur + key);
    }
}
