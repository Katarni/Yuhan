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

    void push(ReservedMemory* identifier);
    void push(Token operation);
    void push(Type literal);

 private:
    enum class PRNType {
        Identifier, Operation, Literal,
        Address, Function
    };

    size_t cur_;

    std::vector<std::variant<ReservedMemory*, Token, Type, size_t>> prn_; // сюда добавить функции и служебные вещи
    std::vector<PRNType> types_; // информация о содержимом ячейки
};
