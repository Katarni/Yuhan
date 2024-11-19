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

    void push(Identifier identifier);
    void push(Token operation);
    void push(Type literal);
    void push(const std::string& field);

 private:
    enum class PRNType {
        Identifier, Operation, Literal,
        Address, Function, FieldName
    };

    size_t cur_;

    std::vector<std::variant<Identifier, Token, Type,
                            size_t, std::string>> prn_; // сюда добавить функции и служебные вещи
    std::vector<PRNType> types_; // информация о содержимом ячейки
};
