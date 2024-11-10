#include "../include/TIDTree.h"

Type TIDTree::NodeTID::checkID(std::string &name) {
    return variables_.checkID(name);
}

void TIDTree::NodeTID::pushID(std::string &name, Type &type) {
    variables_.pushID(name, type);
}

Type TIDTree::NodeTID::checkFieldOfStruct(std::string &name, std::string &field_name) {
    return structs_.checkField(name, field_name);
}

bool TIDTree::NodeTID::checkStruct(std::string &name) {
    return structs_.checkId(name);
}

Type TIDTree::NodeTID::checkFunction(std::string &name, std::vector<Variable> &args) {
    return functions_.checkID(name, args);
}

void TIDTree::NodeTID::pushFunction(std::string &name, Type &type, std::vector<Variable> &args) {
    functions_.pushID(name, type, args);
}

void TIDTree::NodeTID::pushStruct(std::string &name) {
    structs_.pushID(name);
}

void TIDTree::NodeTID::pushFieldOfStruct(std::string &name, std::string &name_field, Type &type_field) {
    return structs_.pushField(name, name_field, type_field);
}

TIDTree::NodeTID *TIDTree::NodeTID::getParent() {
    return parent_;
}

TIDTree::NodeTID::NodeTID(bool is_struct, bool is_namespace, std::string& namespace_, TIDTree::NodeTID* parent) :
        is_struct_(is_struct),
        is_namespace_(is_namespace),
        namespace_(namespace_),
        parent_(parent) {}

bool TIDTree::NodeTID::isNamespace() const {
    return is_namespace_;
}

bool TIDTree::NodeTID::isStruct() const {
    return is_struct_;
}

std::string TIDTree::NodeTID::getNamespace() const {
    return namespace_;
}

void TIDTree::closeScope() {
    current_scope_ = current_scope_->getParent();
}

void TIDTree::createScope(bool is_struct, bool is_namespace, std::string namespace_) {
    current_scope_ = new NodeTID(is_struct, is_namespace, namespace_, current_scope_);
}

Type TIDTree::checkVariable(std::string &name) {
    return checkVariable(current_scope_, name);
}

Type TIDTree::checkVariable(TIDTree::NodeTID *ptr, std::string &name) {
    if (ptr == nullptr) throw std::runtime_error("Нет имени в боре");
    try {
        return ptr->checkID(name);
    } catch (std::runtime_error &error) {
        std::string new_name = name;
        if (ptr->isNamespace()) {
            new_name = ptr->getNamespace() + "::" + name;
        }
        return checkVariable(ptr->getParent(), new_name);
    }
}

bool TIDTree::checkStruct(std::string &name) {
    return checkStruct(current_scope_, name);
}

bool TIDTree::checkStruct(TIDTree::NodeTID *ptr, std::string &name) {
    if (ptr == nullptr) throw std::runtime_error("Нет имени в боре");
    try {
        return ptr->checkStruct(name);
    } catch (std::runtime_error &error) {
        std::string new_name = name;
        if (ptr->isNamespace() || ptr->isStruct()) {
            new_name = ptr->getNamespace() + "::" + name;
        }
        return checkStruct(ptr->getParent(), new_name);
    }
}

Type TIDTree::checkFunction(std::string &name, std::vector<Variable> &args) {
    return checkFunction(current_scope_, name, args);
}

Type TIDTree::checkFunction(TIDTree::NodeTID *ptr, std::string &name, std::vector<Variable> &args) {
    if (ptr == nullptr) throw std::runtime_error("Нет имени в боре");
    try {
        return ptr->checkFunction(name, args);
    } catch (std::runtime_error &error) {
        std::string new_name = name;
        if (ptr->isNamespace()) {
            new_name = ptr->getNamespace() + "::" + name;
        }
        return checkFunction(ptr->getParent(), new_name, args);
    }
}

void TIDTree::pushVariable(std::string &name, Type &type) {
    pushVariable(current_scope_, name, type);
}

void TIDTree::pushField(TIDTree::NodeTID *ptr, std::string& name, std::string &name_field, Type& type_field) {
    if (ptr == nullptr) return;
    ptr->pushFieldOfStruct(name, name_field, type_field);
    if (ptr->isNamespace() || ptr->isStruct()) {
        std::string struct_name = ptr->getNamespace() + "::" + name;
        pushField(ptr->getParent(), name, name_field, type_field);
    }
}

void TIDTree::pushVariable(TIDTree::NodeTID *ptr, std::string &name, Type& type) {
    if (ptr == nullptr) return;
    if (ptr->isStruct()) {
        std::string name_of_struct = ptr->getNamespace();
        pushField(ptr->getParent(), name_of_struct, name, type);
        return;
    }
    ptr->pushID(name, type);
    if (ptr->isNamespace()) {
        std::string new_name = ptr->getNamespace() + "::" + name;
        pushVariable(ptr->getParent(), new_name, type);
        return;
    }
}

void TIDTree::pushStruct(std::string &name) {
    pushStruct(current_scope_, name);
}

void TIDTree::pushStruct(TIDTree::NodeTID *ptr, std::string &name) {
    if (ptr == nullptr) return;
    ptr->pushStruct(name);
    if (ptr->isNamespace() || ptr->isStruct()) {
        std::string new_name = ptr->getNamespace() + "::" + name;
        pushStruct(ptr->getParent(), new_name);
    }
}

void TIDTree::pushFunction(std::string &name, Type &type, std::vector<Variable> &args) {
    pushFunction(current_scope_, name, type, args);
}

void TIDTree::pushFunction(TIDTree::NodeTID *ptr, std::string &name, Type &type, std::vector<Variable> &args) {
    if (ptr == nullptr) return;
    ptr->pushFunction(name, type, args);
    if (ptr->isNamespace()) {
        std::string new_name = ptr->getNamespace() + "::" + name;
        pushFunction(ptr->getParent(), new_name, type, args);
        return;
    }
}
