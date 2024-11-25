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

Identifier TIDVariable::checkID(std::string &name) {
    return getNode(name)->getVar();
}

void TIDVariable::pushID(std::string &name, Type &type,
                         const std::vector<std::pair<std::string, Type>>& fields, std::string id) {
    if (isInTrie(name)) throw std::runtime_error("Variable with this name has already been created");
    auto ptr = insert(name);
    ptr->setType(type);
    ptr->setId(std::move(id));

    if (!fields.empty()) {
        ptr->setFields(fields);
    }
}

void VariableNode::setType(Type &type) {
    var_.setType(type);
}

Type VariableNode::getType() {
    return var_.getType();
}

Identifier VariableNode::getVar() const {
    return var_;
}

void VariableNode::addChild(char let) {
    if (go_[let] != nullptr) return;
    go_[let] = new VariableNode();
}

bool TIDStructure::checkId(const std::string &name) {
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

std::vector<std::pair<std::string, Type>> TIDStructure::getAllFieldsByName(const std::string& name) {
    return getNode(name)->getAllVarsWithName();
}

Type StructureNode::checkIDField(std::string &name) {
    return fields_.checkID(name).getType();
}

void StructureNode::pushIDField(std::string &name, Type &type) {
    fields_.pushID(name, type);
    name_fields_.emplace_back(name, type);
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

const std::string& FunctionNode::getId() {
    return id_;
}

void FunctionNode::setId(const std::string& id) {
    id_ = id;
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

std::string TIDFunction::checkIDName(std::string &name, std::vector<Type> &args) {
    auto ptr = getNode(name);
    ptr->checkArgs(args);
    return ptr->getId();
}

void TIDFunction::pushID(std::string &name, Type &type, std::vector<Variable> &args, std::string id) {
    if (isInTrie(name)) throw std::runtime_error("Function with this name has already been created");
    auto ptr = insert(name);
    ptr->setType(type);
    ptr->setArgs(args);
    ptr->setId(id);
}

Type TIDFunction::checkID(std::string &name) {
    if (!isInTrie(name)) throw not_found_error();
    return getNode(name)->getType();
}

std::vector<std::pair<std::string, Type>> StructureNode::getAllVarsWithName() const {
    return name_fields_;
}

void VariableNode::setId(std::string id) {
    var_.setName(id);
}

void VariableNode::setFields(const std::vector<std::pair<std::string, Type>> &fields) {
    var_.setFields(fields);
}
