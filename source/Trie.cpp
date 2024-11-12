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
    if (isInTrie(name)) throw std::runtime_error("Variable with this name has already been created");
    auto ptr = insert(name);
    ptr->setType(type);
}

void VariableNode::setType(Type &type) {
   type_ = type;
}

Type VariableNode::getType() const {
    return type_;
}

void VariableNode::addChild(char let) {
    if (go_[let] != nullptr) return;
    go_[let] = new VariableNode();
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
    if (isInTrie(name)) throw std::runtime_error("Struct with this name has already been created");
    insert(name);
}

Type StructureNode::checkIDField(std::string &name) {
    return fields_.checkID(name);
}

void StructureNode::pushIDField(std::string &name, Type& type) {
    fields_.pushID(name, type);
}

void StructureNode::addChild(char let) {
    if (go_[let] != nullptr) return;
    go_[let] = new StructureNode();
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

void FunctionNode::checkArgs(std::vector<Variable> &args) {
    if (args.size() != args_.size()) {
        throw std::runtime_error("Incorrect number of arguments");
    }
    for (int i = 0; i < args_.size(); ++i) {
        if (args[i].getType() != args_[i].getType()) {
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

bool Type::operator!=(const Type &other) const {
    return !(operator==(other));
}

Type TIDFunction::checkID(std::string &name, std::vector<Variable> &args) {
    auto ptr = getNode(name);
    ptr->checkArgs(args);
    return ptr->getType();
}

void TIDFunction::pushID(std::string &name, Type& type, std::vector<Variable> &args) {
    auto ptr = insert(name);
    ptr->setType(type);
    ptr->setArgs(args);
}

bool Type::isLvalue() const {
    return is_lvalue_;
}

const std::string& Type::getName() const {
    return name_;
}

void Type::setLvalue(bool is_lvalue) {
    is_lvalue_ = is_lvalue;
}

void Type::setName(const std::string& name) {
    name_ = name;
}

Type::Type(std::string name, bool lvalue) : name_(std::move(name)), size_array_(0), array_type_(nullptr), is_lvalue_(lvalue) {}

Type::Type(const Type &other) {
    name_ = other.name_;
    size_array_ = other.size_array_;
    if (other.array_type_ != nullptr) {
        array_type_ = new Type(*other.array_type_);
    } else {
        array_type_ = nullptr;
    }
    is_lvalue_ = other.is_lvalue_;
}

Type Type::operator=(const Type &other) {
    name_ = other.name_;
    size_array_ = other.size_array_;
    if (other.array_type_ != nullptr) {
        array_type_ = new Type(*other.array_type_);
    } else {
        array_type_ = nullptr;
    }
    is_lvalue_ = other.is_lvalue_;
    return *this;
}

void Type::setArrayType(Type type) {
    array_type_ = new Type(type);
}

void Type::setArraySize(size_t array_size) {
    array_size = array_size;
}

Type::~Type() {
    if (array_type_ == nullptr) return;
    delete array_type_;
}

Type Type::getArrayType() const {
    return *array_type_;
}

Type::Type() : name_(""), size_array_(0), array_type_(nullptr), is_lvalue_(false) {}

