//
// Created by Timur Akhmetzianov on 11.11.2024
//

#pragma once

#include "Trie.h"
#include "Token.h"
#include "PRNGenerator.h"


class SemanticStack {
 public:
    SemanticStack(PRNGenerator* generator) : generator_(generator) {}

    void push(const Type& operand) noexcept;
    void push(const Token& operation) noexcept;

    Type popOperand();
    Token popOperation();

    void checkUno();
    void checkBinary();
    void checkBool();
    void checkType(Type type);

    void clear() noexcept;

    class Error : public std::exception {
     public: 
        explicit Error(std::string what) : std::exception(), what_(std::move(what)) {}
        explicit Error(const char* what) : std::exception(), what_(what) {}

        [[nodiscard]]
        const char* what() const noexcept override;
     private:
        std::string what_;
    };

 private:
    PRNGenerator* generator_;
    std::stack<std::variant<Type, Token>> sem_stack_;

    Type getOperand();
    Token getOperation();
};
