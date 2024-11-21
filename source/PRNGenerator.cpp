//
// Created by Timur Akhmetzianov on 17.11.2024
//

#include "../include/PRNGenerator.h"

void PRNGenerator::push(Identifier identifier) {
    prn_.emplace_back(identifier);
    types_.push_back(PRNType::Identifier);
    ++cur_;
}

void PRNGenerator::push(Token operation) {
    prn_.emplace_back(operation);
    types_.push_back(PRNType::Operation);
    ++cur_;
}

void PRNGenerator::push(Literal literal) {
    prn_.emplace_back(literal);
    types_.push_back(PRNType::Literal);
    ++cur_;
}

void PRNGenerator::pushField(const std::string &field) {
    prn_.emplace_back(field);
    types_.push_back(PRNType::FieldName);
    ++cur_;
}

void PRNGenerator::pushFuncCall(const std::string &func_id) {
    prn_.emplace_back(func_id);
    types_.push_back(PRNType::Function);
    ++cur_;
}

void PRNGenerator::pushFuncDef(const std::string &func_id) {
    func_pos_[func_id] = cur_;
    push(SysVals::FuncStart);
}

void PRNGenerator::push(PRNGenerator::SysVals val) {
    prn_.emplace_back(val);
    types_.emplace_back(PRNType::SystemValue);
    ++cur_;
}

void PRNGenerator::setFuncArgsCnt(const std::string &func_id, size_t cnt) {
    if (func_id == main_id_ && cnt != 0) {
        throw std::runtime_error("main can't require args");
    }

    func_args_[func_id] = cnt;
}

void PRNGenerator::setMainId(const std::string &id) {
    if (!main_id_.empty()) {
        throw std::runtime_error("more than one main in program");
    }
    main_id_ = id;
}

void PRNGenerator::pushWhileStatement() {
    break_stack_.top().push_back(cur_);
    push(-1);
    push(SysVals::GoToByFalse);
}

void PRNGenerator::push(size_t address) {
    prn_.emplace_back(address);
    types_.push_back(PRNType::Address);
    ++cur_;
}

void PRNGenerator::push(size_t address, size_t pos) {
    if (pos >= prn_.size()) {
        prn_.resize(pos + 1);
        types_.resize(pos + 1);
    }

    prn_[pos] = address;
    types_[pos] = PRNType::Address;
}

void PRNGenerator::closeCycle() {
    for (auto idx : break_stack_.top()) {
        prn_[idx] = cur_;
    }
    break_stack_.pop();
    cycles_starts_.pop();
}

void PRNGenerator::pushBreak() {
    break_stack_.top().push_back(cur_);
    push(-1);
    push(SysVals::GoTo);
}

void PRNGenerator::pushContinue() {
    push(cycles_starts_.top());
    push(SysVals::GoTo);
}

size_t PRNGenerator::getCurIdx() const {
    return cur_;
}

void PRNGenerator::pushCycleStart() {
    cycles_starts_.push(cur_);
    break_stack_.emplace();
}

void PRNGenerator::pushSwitch() {
    break_stack_.emplace();
}

void PRNGenerator::closeSwitch() {
    for (auto idx : break_stack_.top()) {
        prn_[idx] = cur_;
    }
    break_stack_.pop();
}

size_t PRNGenerator::size() const {
    return prn_.size();
}

std::pair<std::variant<Identifier, Token, Literal, size_t, std::string, PRNGenerator::SysVals>, PRNGenerator::PRNType>
PRNGenerator::getById(size_t idx) const {
    if (idx >= prn_.size()) {
        throw std::runtime_error("container out of range");
    }

    return {prn_[idx], types_[idx]};
}

std::ofstream &operator<<(std::ofstream &os,
                         std::pair<std::variant<Identifier, Token, Literal, size_t,
                                        std::string, PRNGenerator::SysVals>, PRNGenerator::PRNType> state) {
    switch (state.second) {
        case PRNGenerator::PRNType::Identifier:
            os << "Identifier: " << std::get<Identifier>(state.first).getName();
            break;
        case PRNGenerator::PRNType::Operation:
            os << "Operator: " << std::get<Token>(state.first).getContent();
            break;
        case PRNGenerator::PRNType::Literal:
            // get val here
            os << "Literal: " << std::get<Literal>(state.first).getType().getName();
            break;
        case PRNGenerator::PRNType::Address:
            os << "Address: " << std::get<size_t>(state.first);
            break;
        case PRNGenerator::PRNType::Function:
            os << "Function: " << std::get<std::string>(state.first);
            break;
        case PRNGenerator::PRNType::FieldName:
            os << "Struct Field: " << std::get<std::string>(state.first);
            break;
        case PRNGenerator::PRNType::SystemValue:
            switch (std::get<PRNGenerator::SysVals>(state.first)) {
                case PRNGenerator::SysVals::FuncEnd:
                    os << "Function End" << std::endl;
                    break;
                case PRNGenerator::SysVals::FuncStart:
                    os << "Function Start" << std::endl;
                    break;
                case PRNGenerator::SysVals::Semicolon:
                    os << "Semicolon" << std::endl;
                    break;
                case PRNGenerator::SysVals::Return:
                    os << "Return" << std::endl;
                    break;
                case PRNGenerator::SysVals::GoTo:
                    os << "GoTo" << std::endl;
                    break;
                case PRNGenerator::SysVals::GoToByFalse:
                    os << "GoTo by False" << std::endl;
                    break;
                case PRNGenerator::SysVals::Cmp:
                    os << "Switch Cmp" << std::endl;
                    break;
            }
            break;
    }

    os << std::endl;

    return os;
}
