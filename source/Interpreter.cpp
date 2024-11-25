//
// Created by Тимур Ахметзянов on 22.11.2024.
//

#include "../include/Interpreter.h"

ReservedMemory *&Interpreter::FunctionCall::getVar(const Identifier& name) {
    return vars_[name.getName()];
}

bool Interpreter::FunctionCall::findVar(const Identifier& name) {
    return vars_.find(name.getName()) != vars_.end();
}

ReservedMemory *&Interpreter::FunctionCall::createVar(const Identifier &name) {
    if (vars_.find(name.getName()) != vars_.end()) return vars_[name.getName()];

    vars_[name.getName()] = new ReservedMemory(name.getType());
    return vars_[name.getName()];
}

ReservedMemory *&Interpreter::createVar(const Identifier &name) {
    if (!function_stack_.empty()) {
        function_stack_.top().createVar(name);
    }

    if (global_vars_.find(name.getName()) != global_vars_.end()) return global_vars_[name.getName()];

    global_vars_[name.getName()] = new ReservedMemory(name.getType());
    return global_vars_[name.getName()];
}

ReservedMemory*& Interpreter::getVar(const Identifier& name) {
    if (!function_stack_.empty()) {
        if (function_stack_.top().findVar(name)) {
            return function_stack_.top().getVar(name);
        }

        if (global_vars_.find(name.getName()) != global_vars_.end()) {
            return global_vars_[name.getName()];
        }

        function_stack_.top().createVar(name);
    }

    if (global_vars_.find(name.getName()) != global_vars_.end()) {
        return global_vars_[name.getName()];
    }

    global_vars_[name.getName()] = new ReservedMemory(name.getType());
    return global_vars_[name.getName()];
}

size_t Interpreter::FunctionCall::returnAddress() const {
    return return_address_;
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

    function_stack_.emplace(cur_ + 1);

    cur_ = address + 1;
    for (size_t i = 0; i < vars.size(); ++i, ++cur_) {
        auto state = generator_->getById(cur_).first;
        if (std::get<Identifier>(state).getType().isLvalue()) {
            function_stack_.top().setVar(std::get<Identifier>(state).getName(), vars[i]);
        } else {
            function_stack_.top().setVar(std::get<Identifier>(state).getName(), *vars[i]);
        }
    }

    while (true) {
        auto state = generator_->getById(cur_);

        switch (state.second) {
            case PRNGenerator::PRNType::Identifier:
                createVar(std::get<Identifier>(state.first));
                calc_stack_.emplace(getVar(std::get<Identifier>(state.first)));
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
                calc_stack_.emplace(std::get<std::string>(state.first));
                break;
            case PRNGenerator::PRNType::SystemValue:
                switch (std::get<PRNGenerator::SysVals>(state.first)) {
                    case PRNGenerator::SysVals::FuncEnd:
                        throw std::runtime_error("function should return value");
                    case PRNGenerator::SysVals::Comma:
                    case PRNGenerator::SysVals::Semicolon:
                        calc_stack_.pop();
                        break;
                    case PRNGenerator::SysVals::Return:
                        goto func_end;
                    case PRNGenerator::SysVals::GoTo:
                    case PRNGenerator::SysVals::GoToByFalse:
                    case PRNGenerator::SysVals::Cmp:
                    case PRNGenerator::SysVals::Scan:
                    case PRNGenerator::SysVals::Print:
                        operation(std::get<PRNGenerator::SysVals>(state.first));
                        break;
                }
                break;
        }

        ++cur_;
    }

    func_end:

    cur_ = function_stack_.top().returnAddress();
    function_stack_.pop();
}

void Interpreter::lvalueOperation(const Token &operation) {
    if (operation.getType() == Token::Type::Dot) {
        std::string rhs;
        ReservedMemory *lhs = nullptr;

        rhs = std::get<std::string>(calc_stack_.top());
        calc_stack_.pop();

        lhs = std::get<ReservedMemory*>(calc_stack_.top());
        calc_stack_.pop();

        auto res = lhs->getFieldByName(rhs);
        calc_stack_.emplace(res);
    } else if (operation.getContent() == "[") {
        ReservedMemory *lhs = nullptr;
        Literal *rhs = nullptr;

        if (std::holds_alternative<ReservedMemory*>(calc_stack_.top())) {
            rhs = std::get<ReservedMemory*>(calc_stack_.top());
        } else {
            rhs = new Literal(std::get<Literal>(calc_stack_.top()));
        }
        calc_stack_.pop();

        lhs = std::get<ReservedMemory*>(calc_stack_.top());
        calc_stack_.pop();

        auto res = (*lhs)[*rhs];
        calc_stack_.emplace(res);
    } else if (operation.getType() == Token::Type::LvalueBinaryOperator) {
        Literal* rhs;
        ReservedMemory* lhs;

        if (std::holds_alternative<ReservedMemory*>(calc_stack_.top())) {
            rhs = std::get<ReservedMemory*>(calc_stack_.top());
        } else {
            rhs = new Literal(std::get<Literal>(calc_stack_.top()));
        }
        calc_stack_.pop();

        lhs = std::get<ReservedMemory*>(calc_stack_.top());
        calc_stack_.pop();

        if (operation.getContent() == "=") {
            *lhs = *rhs;
        } else if (operation.getContent() == "+=") {
            *lhs += *rhs;
        } else if (operation.getContent() == "-=") {
            *lhs -= *rhs;
        } else if (operation.getContent() == "*=") {
            *lhs *= *rhs;
        } else if (operation.getContent() == "/=") {
            *lhs /= *rhs;
        } else if (operation.getContent() == "^=") {
            *lhs ^= *rhs;
        } else if (operation.getContent() == "&=") {
            *lhs &= *rhs;
        } else if (operation.getContent() == "|=") {
            *lhs |= *rhs;
        } else if (operation.getContent() == "<<=") {
            *lhs &= *rhs;
        } else if (operation.getContent() == ">>=") {
            *lhs |= *rhs;
        }

        calc_stack_.emplace(lhs);
    } else if (operation.getType() == Token::Type::LvalueUnaryOperator) {
        ReservedMemory* lhs;

        lhs = std::get<ReservedMemory*>(calc_stack_.top());
        calc_stack_.pop();

        if (operation.getContent() == "++") {
            *lhs += Literal(Type("int", false), "1");
        } else if (operation.getContent() == "--") {
            *lhs -= Literal(Type("int", false), "1");
        }

        calc_stack_.emplace(lhs);
    }
}

void Interpreter::operation(const Token &operation) {
    if (operation.getType() == Token::Type::LvalueBinaryOperator ||
        operation.getType() == Token::Type::LvalueUnaryOperator ||
        operation.getType() == Token::Type::Dot ||
        operation.getContent() == "[" ||
        operation.getContent() == "==" ||
        operation.getContent() == "!=") {

        lvalueOperation(operation);
        return;
    }

    if (operation.getType() == Token::Type::RvalueBinaryOperator ||
        operation.getType() == Token::Type::LessThan ||
        operation.getType() == Token::Type::GreaterThan) {
        Literal *rhs = nullptr, *lhs = nullptr, res;

        if (std::holds_alternative<ReservedMemory*>(calc_stack_.top())) {
            rhs = std::get<ReservedMemory*>(calc_stack_.top());
        } else {
            rhs = new Literal(std::get<Literal>(calc_stack_.top()));
        }
        calc_stack_.pop();

        if (std::holds_alternative<ReservedMemory*>(calc_stack_.top())) {
            lhs = std::get<ReservedMemory*>(calc_stack_.top());
        } else {
            lhs = new Literal(std::get<Literal>(calc_stack_.top()));
        }
        calc_stack_.pop();

        if (operation.getContent() == "+") {
            res = *lhs + *rhs;
        } else if (operation.getContent() == "*") {
            res = *lhs * *rhs;
        } else if (operation.getContent() == "/") {
            res = *lhs / *rhs;
        } else if (operation.getContent() == "-") {
            res = *lhs - *rhs;
        } else if (operation.getContent() == "%") {
            res = *lhs % *rhs;
        } else if (operation.getContent() == "and") {
            res = *lhs && *rhs;
        } else if (operation.getContent() == "or") {
            res = *lhs || *rhs;
        } else if (operation.getContent() == "&") {
            res = *lhs & *rhs;
        } else if (operation.getContent() == "|") {
            res = *lhs | *rhs;
        } else if (operation.getContent() == "^") {
            res = *lhs ^ *rhs;
        } else if (operation.getContent() == "<<") {
            res = *lhs << *rhs;
        } else if (operation.getContent() == ">>") {
            res = *lhs >> *rhs;
        } else if (operation.getContent() == "<") {
            res = *lhs < *rhs;
        } else if (operation.getContent() == ">") {
            res = *lhs > *rhs;
        } else if (operation.getContent() == "<=") {
            res = *lhs <= *rhs;
        } else if (operation.getContent() == ">=") {
            res = *lhs >= *rhs;
        }

        calc_stack_.emplace(res);
    } else if (operation.getType() == Token::Type::RvalueUnaryOperator) {
        Literal *literal;
        if (std::holds_alternative<ReservedMemory*>(calc_stack_.top())) {
            literal = std::get<ReservedMemory*>(calc_stack_.top());
        } else {
            literal = new Literal(std::get<Literal>(calc_stack_.top()));
        }
        calc_stack_.pop();

        Literal res;
        if (operation.getContent() == "!") {
            res = !(*literal);
        }
        calc_stack_.emplace(res);
    }
}

void Interpreter::operation(PRNGenerator::SysVals operation) {
    size_t point;
    ReservedMemory *lhs_var = nullptr, *rhs_var = nullptr;
    Literal lhs_lit, rhs_lit;
    switch (operation) {
        case PRNGenerator::SysVals::GoTo:
            cur_ = std::get<size_t>(calc_stack_.top()) - 1;
            calc_stack_.pop();
            break;
        case PRNGenerator::SysVals::GoToByFalse:
            point = std::get<size_t>(calc_stack_.top());
            calc_stack_.pop();

            if (std::holds_alternative<ReservedMemory*>(calc_stack_.top())) {
                lhs_var = std::get<ReservedMemory*>(calc_stack_.top());

                if (!lhs_var->isTrue()) {
                    cur_ = point - 1;
                }
            } else {
                lhs_lit = std::get<Literal>(calc_stack_.top());

                if (!lhs_lit.isTrue()) {
                    cur_ = point - 1;
                }
            }

            calc_stack_.pop();
            break;
        case PRNGenerator::SysVals::Cmp:
            break;
        case PRNGenerator::SysVals::Scan:
            std::cin >> std::get<ReservedMemory*>(calc_stack_.top());
            break;
        case PRNGenerator::SysVals::Print:
            if (std::holds_alternative<ReservedMemory*>(calc_stack_.top())) {
                rhs_var = std::get<ReservedMemory*>(calc_stack_.top());

                std::cout << *rhs_var;
            } else {
                rhs_lit = std::get<Literal>(calc_stack_.top());

                std::cout << rhs_lit;
            }
            break;
    }
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
