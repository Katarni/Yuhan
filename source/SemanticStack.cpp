#include "../include/SemanticStack.h"

const char *SemanticStack::Error::what() const noexcept {
    return what_.c_str();
}

void SemanticStack::push(const Token& operation) noexcept {
    sem_stack_.emplace(operation);
}

void SemanticStack::push(const Type& operand) noexcept {
    sem_stack_.emplace(operand);
}

Token SemanticStack::popOperation() {
    auto operation = getOperation();
    sem_stack_.pop();
    return operation;
}

Type SemanticStack::popOperand() {
    auto operand = getOperand();
    sem_stack_.pop();
    return operand;
}

Token SemanticStack::getOperation() {
    if (!std::holds_alternative<Token>(sem_stack_.top())) {
        throw std::runtime_error("expecting operation, found operator");
    }
    return std::get<Token>(sem_stack_.top());
}

Type SemanticStack::getOperand() {
    if (!std::holds_alternative<Type>(sem_stack_.top())) {
        throw std::runtime_error("expecting operator, found operation");
    }
    return std::get<Type>(sem_stack_.top());
}

void SemanticStack::checkUno() {
    auto operand = popOperand();
    auto operation = popOperation();

    Type result_operand = operand;

    switch (operation.getType()) {
        case Token::Type::PlusOrMinusOperator:
        case Token::Type::RvalueUnaryOperator:
            if (operand.getName() == "int" || operand.getName() == "char" || operand.getName() == "float") {
                if (operation.getContent() == "!") {
                    result_operand.setName("bool");
                }
                result_operand.setLvalue(false);
                push(result_operand);
                break;
            } 
            if (operand.getName() == "bool" && operation.getContent() == "!") {
                result_operand.setLvalue(false);
                push(result_operand);
                break;
            }

            throw Error("incorrect operand type in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
        case Token::Type::LvalueUnaryOperator:
            if (operand.getName() != "int" && operand.getName() != "char" && operand.getName() != "float") {
                throw Error("incorrect operand type in: " + std::to_string(operation.getLine()) +
                            " " + std::to_string(operation.getColumn()));
            }

            if (!operand.isLvalue()) {
                throw Error("expecting lvalue, got rvalue in: " + std::to_string(operation.getLine()) +
                            " " + std::to_string(operation.getColumn()));
            }

            push(result_operand);

            break;
        default:
            throw Error("incorrect operator, expected unary operator in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
    }
}

void SemanticStack::checkBinary() {
    auto rhs = popOperand();
    auto operation = popOperation();
    auto lhs = popOperand();

    if (rhs.getName() != "int" && rhs.getName() != "float" && rhs.getName() != "char" && rhs.getName() != "bool" &&
        rhs.getName() != "string" && rhs.getName() != "array") {
        throw Error("u can't do any operations with structs in: " + std::to_string(operation.getLine()) +
                    " " + std::to_string(operation.getColumn()));
    }

    if (lhs.getName() != "int" && lhs.getName() != "float" && lhs.getName() != "char" && lhs.getName() != "bool" &&
        lhs.getName() != "string" && lhs.getName() != "array") {
        throw Error("u can't do any operations with structs in: " + std::to_string(operation.getLine()) +
                    " " + std::to_string(operation.getColumn()));
    }

    Type result_type;
    
    if (operation.getContent() == "and" || operation.getContent() == "or" ||
        operation.getContent() == "*" || operation.getContent() == "/" || operation.getContent() == "-") {
        if (lhs.getName() == "array" || rhs.getName() == "array") {
           throw Error("u can't do it with arrays in: " +
                                std::to_string(operation.getLine()) + " " + std::to_string(operation.getColumn()));
        }

        if (rhs.getName() == "string" || lhs.getName() == "string") {
            throw Error("u can't do it with strings in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
        }

        if (operation.getContent() == "and" || operation.getContent() == "or") {
            result_type.setName("bool");
            goto final_push;
        } 

        if (rhs.getName() == "float" || lhs.getName() == "float") {
            result_type.setName("float");
        } else if (rhs.getName() == "int" || lhs.getName() == "int") {
            result_type.setName("int");
        } else if (rhs.getName() == "char" || lhs.getName() == "char") {
            result_type.setName("char");
        } else {
            result_type.setName("bool");
        }
    } else if (operation.getContent() == "+") {
        if (lhs.getName() == "array" || rhs.getName() == "array") {
            throw Error("u can't do it with arrays in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
        }

        if (lhs.getName() == "string" || rhs.getName() == "string") {
            if (lhs.getName() != "char" && rhs.getName() != "char") {
                throw Error("incorrect type in: " + std::to_string(operation.getLine()) +
                            " " + std::to_string(operation.getColumn()));
            }

            result_type.setName("string");
            goto final_push;
        }

        if (rhs.getName() == "float" || lhs.getName() == "float") {
            result_type.setName("float");
        } else if (rhs.getName() == "int" || lhs.getName() == "int") {
            result_type.setName("int");
        } else if (rhs.getName() == "char" || lhs.getName() == "char") {
            result_type.setName("char");
        } else {
            result_type.setName("bool");
        }
    } else if (operation.getContent() == "+=") {
        if (lhs.getName() == "array" || rhs.getName() == "array") {
            throw Error("u can't do it with arrays in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
        }

        if (!lhs.isLvalue()) {
            throw Error("needs lvalue in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
        }

        result_type.setLvalue(true);

        if (lhs.getName() == "string" || rhs.getName() == "string") {
            if (lhs.getName() != "char" && rhs.getName() != "char") {
                throw Error("incorrect type in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
            }

            result_type.setName("string");
            goto final_push;
        }

        if (rhs.getName() == "float" || lhs.getName() == "float") {
            result_type.setName("float");
        } else if (rhs.getName() == "int" || lhs.getName() == "int") {
            result_type.setName("int");
        } else if (rhs.getName() == "char" || lhs.getName() == "char") {
            result_type.setName("char");
        } else {
            result_type.setName("bool");
        }
    } else if (operation.getContent() == "*=" || operation.getContent() == "/=" || operation.getContent() == "-=") {
        if (lhs.getName() == "array" || rhs.getName() == "array") {
           throw Error("u can't do it with arrays in: " + std::to_string(operation.getLine()) +
                    " " + std::to_string(operation.getColumn()));
        }

        if (rhs.getName() == "string" || lhs.getName() == "string") {
            throw Error("u can't do it with strings in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
        }

        if (rhs.getName() == "float" || lhs.getName() == "float") {
            result_type.setName("float");
        } else if (rhs.getName() == "int" || lhs.getName() == "int") {
            result_type.setName("int");
        } else if (rhs.getName() == "char" || lhs.getName() == "char") {
            result_type.setName("char");
        } else {
            result_type.setName("bool");
        }

        if (!lhs.isLvalue()) {
            throw Error("u need lvalue in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
        }
        result_type.setLvalue(true);
    } else if (operation.getContent() == "==" || operation.getContent() == "!=") {
        result_type.setName("bool");
        if (rhs.getName() == lhs.getName()) {
            if (rhs.getName() == "array" && rhs.getArrayType() != lhs.getArrayType()) {
                throw Error("incorrect array types in: " + std::to_string(operation.getLine()) +
                            " " + std::to_string(operation.getColumn()));
            }
            goto final_push;
        }

        if (rhs.getName() == "int" || rhs.getName() == "float" || rhs.getName() == "char" || rhs.getName() == "bool") {
            if (lhs.getName() != "int" && lhs.getName() != "float" &&
                lhs.getName() != "char" && lhs.getName() != "bool") {
                throw Error("incorrect type in: " + std::to_string(operation.getLine()) +
                            " " + std::to_string(operation.getColumn()));
            }
            goto final_push;
        }

        throw Error("incorrect types in: " + std::to_string(operation.getLine()) +
                    " " + std::to_string(operation.getColumn()));
    } else if (operation.getContent() == "<" || operation.getContent() == ">" ||
                operation.getContent() == ">=" || operation.getContent() == "<=") {
        if (rhs.getName() == "array" || lhs.getName() == "array") {
            throw Error("incorrect type in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
        }

        if ((rhs.getName() == "string" || lhs.getName() == "string") && lhs.getName() != rhs.getName()) {
            throw Error("incorrect type in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
        }

        result_type.setName("bool");
    } else if (operation.getContent() == "&" || operation.getContent() == "|" || operation.getContent() == "<<" || 
                operation.getContent() == "^" || operation.getContent() == ">>" || operation.getContent() == "%") {
        if (lhs.getName() == "array" || rhs.getName() == "array") {
           throw Error("u can't do it with arrays in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
        }

        if (rhs.getName() == "string" || lhs.getName() == "string") {
            throw Error("u can't do it with strings in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
        }

        if (rhs.getName() == "float" || lhs.getName() == "float") {
            throw Error("u can't do it with float in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
        }

        if (rhs.getName() == "int" || lhs.getName() == "int") {
            result_type.setName("int");
        } else if (rhs.getName() == "char" || lhs.getName() == "char") {
            result_type.setName("char");
        } else {
            result_type.setName("bool");
        }
    } else if (operation.getContent() == "&=" || operation.getContent() == "|=" || operation.getContent() == "<<=" || 
                operation.getContent() == "^=" || operation.getContent() == ">>=" || operation.getContent() == "%=") {
        if (lhs.getName() == "array" || rhs.getName() == "array") {
           throw Error("u can't do it with arrays in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
        }

        if (rhs.getName() == "string" || lhs.getName() == "string") {
            throw Error("u can't do it with strings in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
        }

        if (rhs.getName() == "float" || lhs.getName() == "float") {
            throw Error("u can't do it with float in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
        }

        if (rhs.getName() == "int" || lhs.getName() == "int") {
            result_type.setName("int");
        } else if (rhs.getName() == "char" || lhs.getName() == "char") {
            result_type.setName("char");
        } else {
            result_type.setName("bool");
        }

        if (!lhs.isLvalue()) {
            throw Error("u need lvalue in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
        }
        result_type.setLvalue(true);
    } else if (operation.getContent() == "[") {
        if (lhs.getName() != "array" && lhs.getName() != "string") {
            throw Error("u can't get val by idx from not array type in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
        }

        if (rhs.getName() != "int" && rhs.getName() != "char" && rhs.getName() != "bool") {
            throw Error("need cast idx type to \"int\" in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
        }

        if (lhs.getName() == "string") {
            result_type.setName("char");
        } else {
            result_type = lhs.getArrayType();
        }

        result_type.setLvalue(true);
    } else if (operation.getContent() == "=") {
        if (!lhs.isLvalue()) {
            throw Error("needs lvalue in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
        }
        result_type = lhs;
        if (lhs.getName() == rhs.getName() && rhs.getName() != "array") {
            goto final_push;
        }

        if (rhs.getName() == "int" || rhs.getName() == "float" || rhs.getName() == "char" || rhs.getName() == "bool") {
            if (lhs.getName() != "int" && lhs.getName() != "float" &&
                lhs.getName() != "char" && lhs.getName() != "bool") {
                throw Error("incorrect type in: " + std::to_string(operation.getLine()) +
                            " " + std::to_string(operation.getColumn()));
            }
            goto final_push;
        }

        if (lhs.getName() == "int" || lhs.getName() == "float" || lhs.getName() == "char" || lhs.getName() == "bool") {
            if (rhs.getName() != "int" && rhs.getName() != "float" &&
                rhs.getName() != "char" && rhs.getName() != "bool") {
                throw Error("incorrect type in: " + std::to_string(operation.getLine()) +
                            " " + std::to_string(operation.getColumn()));
            }
            goto final_push;
        }

        if (lhs.getName() == "array" && lhs != rhs) {
            throw Error("incorrect type in: " + std::to_string(operation.getLine()) +
                        " " + std::to_string(operation.getColumn()));
        }
    } else {
        throw Error("operation is incorrect in: " + std::to_string(operation.getLine()) +
                    " " + std::to_string(operation.getColumn()));
    }

    final_push:
    push(result_type);
}

void SemanticStack::clear() noexcept {
    while (!sem_stack_.empty()) {
        sem_stack_.pop();
    }
}

void SemanticStack::checkType(Type type) {
    auto operand = popOperand();

    if (type.getName() == "int" || type.getName() == "char" || type.getName() == "bool" || type.getName() == "float") {
        if (operand.getName() != "int" && operand.getName() != "char" &&
        operand.getName() != "bool" && operand.getName() != "float") {
            throw std::runtime_error("can't cast");
        }
    } else {
        if (!type.compareNoLvalue(operand)) {
            throw std::runtime_error("can't cast");
        }
    }
}

void SemanticStack::checkBool() {
    auto operand = popOperand();
    if (operand.getName() != "int" && operand.getName() != "float" &&
        operand.getName() != "char" && operand.getName() != "bool") {
        throw std::runtime_error("can't cast operator to bool");
    }
}
