//
// Created by Timur Akhmetzianov on 11.11.2024
//

#pragma once

#include "Trie.h"
#include "Token.h"


class SemanticStack {
 public:
    SemanticStack() = default;

    void push(const Type& operand);
    void push(const Token& operation);

    Type popOperand();
    Token popOperation();

    bool checkUno();

 private:
    std::stack<std::variant<Type, Token>> sem_stack_;

    Type getOperand();
    Token getOperation();
};
