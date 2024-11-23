//
// Created by Timur Akhmetzianov on 22.11.2024
//

#pragma once

#include "PRNGenerator.h"


class Interpreter {
 public:
    explicit Interpreter(PRNGenerator *generator) : generator_(generator), cur_(0) {}

    void global();

    void callFunc(const std::string& func, const std::vector<ReservedMemory*>& vars);

 private:
    class FunctionCall {
     public:
        explicit FunctionCall(size_t return_address) : return_address_(return_address) {}
        ~FunctionCall();

        ReservedMemory* getVar(const std::string& name);

        void setVar(const std::string& name, ReservedMemory* var_ptr);
        void setVar(const std::string& name, const ReservedMemory& var);

     private:
        size_t return_address_;
        std::map<std::string, ReservedMemory*> vars_;
        std::map<std::string, bool> is_lvalue_arg_;
    };

    size_t cur_;

    std::stack<std::variant<Identifier, Literal, size_t>> calc_stack_;
    PRNGenerator *generator_;

    std::map<std::string, ReservedMemory*> global_vars_;
    std::deque<FunctionCall> function_stack_;

    void operation(const Token& operation);
};
