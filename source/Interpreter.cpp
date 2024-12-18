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
        if (function_stack_.top().findVar(name)) {
            return function_stack_.top().getVar(name);
        }

        if (global_vars_.find(name.getName()) != global_vars_.end()) {
            return global_vars_[name.getName()];
        }

        return function_stack_.top().createVar(name);
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

        return function_stack_.top().createVar(name);
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
    std::string func_name;
    std::vector<std::variant<ReservedMemory*, Literal>> args;
    std::variant<ReservedMemory, Literal> ret;

    cur_ = 0;

    bool func = false;

    calc_stack_.emplace();

    while (cur_ < generator_->size()) {
        auto state = generator_->getById(cur_);

        if (state.second == PRNGenerator::PRNType::SystemValue &&
            std::get<PRNGenerator::SysVals>(state.first) == PRNGenerator::SysVals::FuncStart) {
            func = true;
        }

        if (func) {
            if (state.second == PRNGenerator::PRNType::SystemValue &&
                std::get<PRNGenerator::SysVals>(state.first) == PRNGenerator::SysVals::FuncEnd) {
                func = false;
            }
            ++cur_;
            continue;
        }

        switch (state.second) {
            case PRNGenerator::PRNType::Identifier:
                createVar(std::get<Identifier>(state.first));
                calc_stack_.top().emplace(getVar(std::get<Identifier>(state.first)));
                break;
            case PRNGenerator::PRNType::Operation:
                operation(std::get<Token>(state.first));
                break;
            case PRNGenerator::PRNType::Literal:
                calc_stack_.top().emplace(std::get<Literal>(state.first));
                break;
            case PRNGenerator::PRNType::Address:
                calc_stack_.top().emplace(std::get<size_t>(state.first));
                break;
            case PRNGenerator::PRNType::Function:
                func_name = std::get<std::string>(state.first);
                args.resize(generator_->getFuncParams(func_name).second);
                for (int i = static_cast<int>(args.size() - 1); i >= 0; --i) {
                    if (std::holds_alternative<ReservedMemory*>(calc_stack_.top().top())) {
                        args[i] = std::get<ReservedMemory*>(calc_stack_.top().top());
                    } else {
                        args[i] = std::get<Literal>(calc_stack_.top().top());
                    }
                    calc_stack_.top().pop();
                }

                callFunc(func_name, args);

                break;
            case PRNGenerator::PRNType::FieldName:
                calc_stack_.top().emplace(std::get<std::string>(state.first));
                break;
            case PRNGenerator::PRNType::SystemValue:
                switch (std::get<PRNGenerator::SysVals>(state.first)) {
                    case PRNGenerator::SysVals::Semicolon:
                        while (!calc_stack_.top().empty()) {
                            calc_stack_.top().pop();
                        }
                        break;
                    case PRNGenerator::SysVals::GoTo:
                    case PRNGenerator::SysVals::GoToByFalse:
                    case PRNGenerator::SysVals::SwitchCmp:
                    case PRNGenerator::SysVals::Scan:
                    case PRNGenerator::SysVals::Print:
                        operation(std::get<PRNGenerator::SysVals>(state.first));
                        break;
                }
                break;
        }

        ++cur_;
    }

    calc_stack_.pop();
}

void Interpreter::callFunc(const std::string &func, const std::vector<std::variant<ReservedMemory*, Literal>>& vars) {
    size_t address;
    if (func == "main") {
        address = generator_->getFuncParams(generator_->mainId()).first;
    } else {
        address = generator_->getFuncParams(func).first;
    }

    function_stack_.emplace(cur_);
    calc_stack_.emplace();

    cur_ = address + 1;

    for (int i = 1; i < vars.size(); ++i, ++cur_) {}

    for (size_t i = 0; i < vars.size(); ++i, ++cur_) {
        auto state = generator_->getById(cur_).first;
        if (std::get<Identifier>(state).getType().isLvalue()) {
            function_stack_.top().setVar(std::get<Identifier>(state).getName(), std::get<ReservedMemory*>(vars[i]));
        } else {
            if (std::holds_alternative<ReservedMemory*>(vars[i])) {
                function_stack_.top().setVar(std::get<Identifier>(state).getName(), *std::get<ReservedMemory*>(vars[i]));
            } else {
                function_stack_.top().setVar(std::get<Identifier>(state).getName(), std::get<Literal>(vars[i]));
            }
        }
    }

    std::string func_name;
    std::vector<std::variant<ReservedMemory*, Literal>> args;
    std::variant<ReservedMemory, Literal> ret;
    while (true) {
        auto state = generator_->getById(cur_);

        switch (state.second) {
            case PRNGenerator::PRNType::Identifier:
                createVar(std::get<Identifier>(state.first));
                calc_stack_.top().emplace(getVar(std::get<Identifier>(state.first)));
                break;
            case PRNGenerator::PRNType::Operation:
                operation(std::get<Token>(state.first));
                break;
            case PRNGenerator::PRNType::Literal:
                calc_stack_.top().emplace(std::get<Literal>(state.first));
                break;
            case PRNGenerator::PRNType::Address:
                calc_stack_.top().emplace(std::get<size_t>(state.first));
                break;
            case PRNGenerator::PRNType::Function:
                func_name = std::get<std::string>(state.first);
                args.resize(generator_->getFuncParams(func_name).second);
                for (int i = static_cast<int>(args.size() - 1); i >= 0; --i) {
                    if (std::holds_alternative<ReservedMemory*>(calc_stack_.top().top())) {
                        args[i] = std::get<ReservedMemory*>(calc_stack_.top().top());
                    } else {
                        args[i] = std::get<Literal>(calc_stack_.top().top());
                    }
                    calc_stack_.top().pop();
                }

                callFunc(func_name, args);

                break;
            case PRNGenerator::PRNType::FieldName:
                calc_stack_.top().emplace(std::get<std::string>(state.first));
                break;
            case PRNGenerator::PRNType::SystemValue:
                switch (std::get<PRNGenerator::SysVals>(state.first)) {
                    case PRNGenerator::SysVals::FuncEnd:
                        throw std::runtime_error("function should return value");
                    case PRNGenerator::SysVals::Semicolon:
                        while (!calc_stack_.top().empty()) {
                            calc_stack_.top().pop();
                        }
                        break;
                    case PRNGenerator::SysVals::Return:
                        if (std::holds_alternative<ReservedMemory*>(calc_stack_.top().top())) {
                            ret = *std::get<ReservedMemory*>(calc_stack_.top().top());
                            calc_stack_.pop();
                            if (!calc_stack_.empty()) {
                                calc_stack_.top().emplace(new ReservedMemory(std::get<ReservedMemory>(ret)));
                            }
                        } else {
                            ret = std::get<Literal>(calc_stack_.top().top());
                            calc_stack_.pop();
                            if (!calc_stack_.empty()) {
                                calc_stack_.top().emplace(std::get<Literal>(ret));
                            }
                        }
                        goto func_end;
                    case PRNGenerator::SysVals::GoTo:
                    case PRNGenerator::SysVals::GoToByFalse:
                    case PRNGenerator::SysVals::SwitchCmp:
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

        rhs = std::get<std::string>(calc_stack_.top().top());
        calc_stack_.top().pop();

        lhs = std::get<ReservedMemory*>(calc_stack_.top().top());
        calc_stack_.top().pop();

        auto res = lhs->getFieldByName(rhs);
        calc_stack_.top().emplace(res);
    } else if (operation.getContent() == "[") {
        ReservedMemory *lhs = nullptr;
        Literal *rhs = nullptr;

        if (std::holds_alternative<ReservedMemory*>(calc_stack_.top().top())) {
            rhs = std::get<ReservedMemory*>(calc_stack_.top().top());
        } else {
            rhs = new Literal(std::get<Literal>(calc_stack_.top().top()));
        }
        calc_stack_.top().pop();

        lhs = std::get<ReservedMemory*>(calc_stack_.top().top());
        calc_stack_.top().pop();

        auto res = (*lhs)[*rhs];
        calc_stack_.top().emplace(res);
    } else if (operation.getType() == Token::Type::LvalueBinaryOperator) {
        Literal* rhs;
        ReservedMemory* lhs;

        if (std::holds_alternative<ReservedMemory*>(calc_stack_.top().top())) {
            rhs = std::get<ReservedMemory*>(calc_stack_.top().top());
        } else {
            rhs = new Literal(std::get<Literal>(calc_stack_.top().top()));
        }
        calc_stack_.top().pop();

        lhs = std::get<ReservedMemory*>(calc_stack_.top().top());
        calc_stack_.top().pop();

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
            *lhs <<= *rhs;
        } else if (operation.getContent() == ">>=") {
            *lhs >>= *rhs;
        }

        calc_stack_.top().emplace(lhs);
    } else if (operation.getType() == Token::Type::LvalueUnaryOperator) {
        ReservedMemory* lhs;

        lhs = std::get<ReservedMemory*>(calc_stack_.top().top());
        calc_stack_.top().pop();

        if (operation.getContent() == "++") {
            *lhs += Literal(Type("int", false), "1");
        } else if (operation.getContent() == "--") {
            *lhs -= Literal(Type("int", false), "1");
        }

        calc_stack_.top().emplace(lhs);
    } else {
        Literal *rhs = nullptr, *lhs = nullptr, res;
        bool got_reserved_1 = false, got_reserved_2 = false;

        if (std::holds_alternative<ReservedMemory*>(calc_stack_.top().top())) {
            rhs = std::get<ReservedMemory*>(calc_stack_.top().top());
            got_reserved_1 = true;
        } else {
            rhs = new Literal(std::get<Literal>(calc_stack_.top().top()));
        }
        calc_stack_.top().pop();

        if (std::holds_alternative<ReservedMemory*>(calc_stack_.top().top())) {
            lhs = std::get<ReservedMemory*>(calc_stack_.top().top());
            got_reserved_2 = true;
        } else {
            lhs = new Literal(std::get<Literal>(calc_stack_.top().top()));
        }
        calc_stack_.top().pop();

        if (operation.getContent() == "==") {
            if (got_reserved_1 && got_reserved_2) {
                res = *static_cast<ReservedMemory*>(lhs) == *static_cast<ReservedMemory*>(rhs);
            } else {
                res = *lhs == *rhs;
            }
        } else {
            if (got_reserved_1 && got_reserved_2) {
                res = *static_cast<ReservedMemory*>(lhs) != *static_cast<ReservedMemory*>(rhs);
            } else {
                res = *lhs != *rhs;
            }
        }

        calc_stack_.top().emplace(res);
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

        if (std::holds_alternative<ReservedMemory*>(calc_stack_.top().top())) {
            rhs = std::get<ReservedMemory*>(calc_stack_.top().top());
        } else {
            rhs = new Literal(std::get<Literal>(calc_stack_.top().top()));
        }
        calc_stack_.top().pop();

        if (std::holds_alternative<ReservedMemory*>(calc_stack_.top().top())) {
            lhs = std::get<ReservedMemory*>(calc_stack_.top().top());
        } else {
            lhs = new Literal(std::get<Literal>(calc_stack_.top().top()));
        }
        calc_stack_.top().pop();

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

        calc_stack_.top().emplace(res);
    } else if (operation.getType() == Token::Type::RvalueUnaryOperator) {
        Literal *literal;
        if (std::holds_alternative<ReservedMemory*>(calc_stack_.top().top())) {
            literal = std::get<ReservedMemory*>(calc_stack_.top().top());
        } else {
            literal = new Literal(std::get<Literal>(calc_stack_.top().top()));
        }
        calc_stack_.top().pop();

        Literal res;
        if (operation.getContent() == "!") {
            res = !(*literal);
        }
        calc_stack_.top().emplace(res);
    }
}

void Interpreter::operation(PRNGenerator::SysVals operation) {
    size_t point;
    ReservedMemory *lhs_var = nullptr, *rhs_var = nullptr;
    Literal lhs_lit, rhs_lit;

    Literal *rhs = nullptr, *lhs = nullptr, res;
    bool got_reserved_1 = false, got_reserved_2 = false;

    switch (operation) {
        case PRNGenerator::SysVals::GoTo:
            cur_ = std::get<size_t>(calc_stack_.top().top()) - 1;
            calc_stack_.top().pop();
            break;
        case PRNGenerator::SysVals::GoToByFalse:
            point = std::get<size_t>(calc_stack_.top().top());
            calc_stack_.top().pop();

            if (std::holds_alternative<ReservedMemory*>(calc_stack_.top().top())) {
                lhs_var = std::get<ReservedMemory*>(calc_stack_.top().top());

                if (!lhs_var->isTrue()) {
                    cur_ = point - 1;
                }
            } else {
                lhs_lit = std::get<Literal>(calc_stack_.top().top());

                if (!lhs_lit.isTrue()) {
                    cur_ = point - 1;
                }
            }

            calc_stack_.top().pop();
            break;
        case PRNGenerator::SysVals::SwitchCmp:
            if (std::holds_alternative<ReservedMemory*>(calc_stack_.top().top())) {
                rhs = std::get<ReservedMemory*>(calc_stack_.top().top());
                got_reserved_1 = true;
            } else {
                rhs = new Literal(std::get<Literal>(calc_stack_.top().top()));
            }
            calc_stack_.top().pop();

            if (std::holds_alternative<ReservedMemory*>(calc_stack_.top().top())) {
                lhs = std::get<ReservedMemory*>(calc_stack_.top().top());
                got_reserved_2 = true;
            } else {
                lhs = new Literal(std::get<Literal>(calc_stack_.top().top()));
            }

            if (got_reserved_1 && got_reserved_2) {
                res = *static_cast<ReservedMemory*>(lhs) == *static_cast<ReservedMemory*>(rhs);
            } else {
                res = *lhs == *rhs;
            }

            calc_stack_.top().emplace(res);
            break;
        case PRNGenerator::SysVals::Scan:
            std::cin >> std::get<ReservedMemory*>(calc_stack_.top().top());
            break;
        case PRNGenerator::SysVals::Print:
            if (std::holds_alternative<ReservedMemory*>(calc_stack_.top().top())) {
                rhs_var = std::get<ReservedMemory*>(calc_stack_.top().top());

                std::cout << *rhs_var;
            } else {
                rhs_lit = std::get<Literal>(calc_stack_.top().top());

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
