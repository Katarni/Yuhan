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
    if (val == SysVals::While || val == SysVals::For) {
        cycles_starts_.push(cur_);
        break_stack_.emplace(std::vector<size_t>());
    }

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

void PRNGenerator::pushCycleStatement() {
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
    // temp here (for tests)
    if (break_stack_.empty()) return;

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
