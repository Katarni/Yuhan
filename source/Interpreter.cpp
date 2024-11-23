//
// Created by Тимур Ахметзянов on 22.11.2024.
//

#include "../include/Interpreter.h"

ReservedMemory *Interpreter::FunctionCall::getVar(const std::string &name) {
    return vars_[name];
}

void Interpreter::global() {

}

void Interpreter::callFunc(const std::string &func, const std::vector<ReservedMemory*>& vars) {
    size_t address;
    if (func == "main") {
        address = generator_->getFuncParams(generator_->mainId()).first;
    } else {
        address = generator_->getFuncParams(func).first;
    }

    function_stack_.emplace_back(cur_ + 1);

    cur_ = address + 1;
    for (size_t i = 0; i < vars.size(); ++i, ++cur_) {
        auto state = generator_->getById(cur_).first;
        if (std::get<Identifier>(state).getType().isLvalue()) {
            function_stack_.back().setVar(std::get<Identifier>(state).getName(), vars[i]);
        } else {
            function_stack_.back().setVar(std::get<Identifier>(state).getName(), *vars[i]);
        }
    }

    while (true) {
        auto state = generator_->getById(cur_);

        switch (state.second) {
            case PRNGenerator::PRNType::Identifier:
                calc_stack_.emplace(std::get<Identifier>(state.first));
                break;
            case PRNGenerator::PRNType::Operation:
                operation(std::get<Token>(state.first));
                break;
            case PRNGenerator::PRNType::Literal:
                calc_stack_.emplace(std::get<Literal>(state.first));
                break;
            case PRNGenerator::PRNType::Address:
                calc_stack_.emplace(std::get<size_t>(state.first));
                break;
            case PRNGenerator::PRNType::Function:
                // functionCall
                break;
            case PRNGenerator::PRNType::FieldName:
                break;
            case PRNGenerator::PRNType::SystemValue:
                break;
        }

        ++cur_;
    }
}

void Interpreter::operation(const Token &operation) {

}

void Interpreter::FunctionCall::setVar(const std::string& name, ReservedMemory *var_ptr) {
    vars_[name] = var_ptr;
    is_lvalue_arg_[name] = true;
}

void Interpreter::FunctionCall::setVar(const std::string& name, const ReservedMemory& var) {
    vars_[name] = new ReservedMemory(var);
    is_lvalue_arg_[name] = false;
}

Interpreter::FunctionCall::~FunctionCall() {
    for (const auto& [key, val] : vars_) {
        if (is_lvalue_arg_[key]) continue;
        delete val;
    }
}
