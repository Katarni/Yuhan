//
// Created by Timur Akhmetzianov on 11.11.2024
//

#pragma once

#include <utility>

#include "Trie.h"
#include "Token.h"


class SemanticStack {
 public:
    SemanticStack() = default;

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
    std::stack<std::variant<Type, Token>> sem_stack_;

    Type getOperand();
    Token getOperation();
};