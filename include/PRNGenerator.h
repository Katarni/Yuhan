//
// Created by Timur Akhmetzianov on 15.11.2024
//

#pragma once

#include "Token.h"
#include "Trie.h"
#include "ReservedMemory.h"


class PRNGenerator {
 public:
    PRNGenerator() : cur_(0) {}

    enum class SysVals {
        FuncEnd, FuncStart, Semicolon, Return, While,
        GoTo, GoToByFalse
    };

    void push(Identifier identifier);
    void push(Token operation);
    void push(Literal literal);
    void pushField(const std::string& field);
    void pushFuncCall(const std::string& func_id);
    void push(SysVals val);
    void pushAddress(size_t address);

    void pushFuncDef(const std::string& func_id);

    void setFuncArgsCnt(const std::string& func_id, size_t cnt);

    void setMainId(const std::string& id);

    void pushWhileStatement();
    void closeCycle();
    void pushBreak();
    void pushContinue();

 private:
    size_t cur_;
    std::string main_id_;
    std::map<std::string, size_t> func_pos_, func_args_;
    std::stack<std::vector<size_t>> break_stack_;
    std::stack<size_t> cycles_starts_;

    enum class PRNType {
        Identifier, Operation, Literal,
        Address, Function, FieldName,
        SystemValue
    };

    std::vector<std::variant<Identifier, Token, Literal,
                            size_t, std::string, SysVals>> prn_; // сюда добавить функции и служебные вещи
    std::vector<PRNType> types_; // информация о содержимом ячейки
};
