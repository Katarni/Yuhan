#include "../include/SemanticStack.h"

void SemanticStack::push(const Token& operation) {
    sem_stack_.emplace(operation);
}

void SemanticStack::push(const Type& operand) {
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
        case Token::Type::RvalueUnaryOperator:
            if (operand.getName() == "int" && operand.getName() == "char" && operand.getName() == "float") {
                if (operation.getContent() == "!") {
                    result_operand.setName("bool");
                } 
                result_operand.setLvalue("false");
                push(result_operand);
                break;
            } 
            if (operand.getName() == "bool" && operation.getContent() == "!") {
                result_operand.setLvalue("false");
                push(result_operand);
                break;
            }

            throw std::runtime_error("incorrect operand type");
        case Token::Type::LvalueUnaryOperator:
            if (operand.getName() != "int" && operand.getName() != "char" && operand.getName() != "float") {
                throw std::runtime_error("incorrect operand type");
            }

            if (!operand.isLvalue()) {
                throw std::runtime_error("expecting lvalue, got rvalue");
            }

            push(result_operand);

            break;
        default:
            throw std::runtime_error("incorrect operator, expected unary operator");
    }
}

void SemanticStack::checkBinary() {
    auto rhs = popOperand();
    auto operation = popOperation();
    auto lhs = popOperand();

    if (rhs.getName() != "int" && rhs.getName() != "float" && rhs.getName() != "char" && rhs.getName() != "bool" &&
        rhs.getName() != "stirng" && rhs.getName() != "array") {
        throw std::runtime_error("u can't do any operations with structs");
    }

    if (lhs.getName() != "int" && lhs.getName() != "float" && lhs.getName() != "char" && lhs.getName() != "bool" &&
        lhs.getName() != "stirng" && lhs.getName() != "array") {
        throw std::runtime_error("u can't do any operations with structs");
    }

    Type result_type;
    
    if (operation.getContent() == "and" || operation.getContent() == "or" ||
        operation.getContent() == "*" || operation.getContent() == "/" || operation.getContent() == "-") {
        if (lhs.getName() == "array" || rhs.getName() == "array") {
           throw std::runtime_error("u can't do it with arrays");
        }

        if (rhs.getName() == "string" || lhs.getName() == "string") {
            throw std::runtime_error("u can't do it with strings");
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
            throw std::runtime_error("u can't do it with arrays");
        }

        if (lhs.getName() == "string" || rhs.getName() == "string") {
            if (lhs.getName() != "char" && rhs.getName() != "char") {
                throw std::runtime_error("incorrect type");
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
    } else if (operation.getContent() == ".") {
        // check structure
    } else if (operation.getContent() == "+=") {
        if (lhs.getName() == "array" || rhs.getName() == "array") {
            throw std::runtime_error("u can't do it with arrays");
        }

        if (!lhs.isLvalue()) {
            throw std::runtime_error("needs lvalue");
        }

        result_type.setLvalue(true);

        if (lhs.getName() == "string" || rhs.getName() == "string") {
            if (lhs.getName() != "char" && rhs.getName() != "char") {
                throw std::runtime_error("incorrect type");
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
           throw std::runtime_error("u can't do it with arrays");
        }

        if (rhs.getName() == "string" || lhs.getName() == "string") {
            throw std::runtime_error("u can't do it with strings");
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
            throw std::runtime_error("u need lvalue");
        }
        result_type.setLvalue(true);
    } else if (operation.getContent() == "==" || operation.getContent() == "!=") {
        result_type.setName("bool");
        if (rhs.getName() == lhs.getName()) {
            if (rhs.getName() == "array" && rhs.getArrayType() != lhs.getArrayType()) {
                throw std::runtime_error("incorrect array types");
            }
            goto final_push;
        }

        if (rhs.getName() == "int" || rhs.getName() == "float" || rhs.getName() == "char" || rhs.getName() == "bool") {
            if (lhs.getName() != "int" && lhs.getName() != "float" && lhs.getName() != "char" && lhs.getName() != "bool") {
                throw std::runtime_error("incorrect type");
            }
            goto final_push;
        }

        throw std::runtime_error("incorrect types");
    } else if (operation.getContent() == "<" || operation.getContent() == ">" || operation.getContent() == ">=" || operation.getContent() == "<=") {
        if (rhs.getName() == "array" || lhs.getName() == "array") {
            throw std::runtime_error("incorrect type");
        }

        if ((rhs.getName() == "string" || lhs.getName() == "string") && lhs.getName() != rhs.getName()) {
            throw std::runtime_error("incorrect type");
        }

        result_type.setName("bool");
    } else if (operation.getContent() == "&" || operation.getContent() == "|" || operation.getContent() == "<<" || 
                operation.getContent() == "^" || operation.getContent() == ">>" || operation.getContent() == "%") {
        if (lhs.getName() == "array" || rhs.getName() == "array") {
           throw std::runtime_error("u can't do it with arrays");
        }

        if (rhs.getName() == "string" || lhs.getName() == "string") {
            throw std::runtime_error("u can't do it with strings");
        }

        if (rhs.getName() == "float" || lhs.getName() == "float") {
            throw std::runtime_error("u can't do it with float");
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
           throw std::runtime_error("u can't do it with arrays");
        }

        if (rhs.getName() == "string" || lhs.getName() == "string") {
            throw std::runtime_error("u can't do it with strings");
        }

        if (rhs.getName() == "float" || lhs.getName() == "float") {
            throw std::runtime_error("u can't do it with float");
        }

        if (rhs.getName() == "int" || lhs.getName() == "int") {
            result_type.setName("int");
        } else if (rhs.getName() == "char" || lhs.getName() == "char") {
            result_type.setName("char");
        } else {
            result_type.setName("bool");
        }

        if (!lhs.isLvalue()) {
            throw std::runtime_error("u need lvalue");
        }
        result_type.setLvalue(true);
    } else if (operation.getContent() == "[") {
        if (rhs.getName() != "array" && rhs.getName() == "string") {
            throw std::runtime_error("u can't get val by idx from not array type");
        }

        if (lhs.getName() != "int" && lhs.getName() != "char" && lhs.getName() != "bool") {
            throw std::runtime_error("need cast idx type to \"int\"");
        }

        if (rhs.getName() == "string") {
            result_type.setName("char");
        } else {
            result_type = rhs.getArrayType();
        }

        result_type.setLvalue(true);
    } else {
        throw std::runtime_error("operation is incorrect");
    }

    final_push:
    push(result_type);
}

void SemanticStack::clear() {
    while (!sem_stack_.empty()) {
        sem_stack_.pop();
    }
}

void SemanticStack::checkType(Type type) {
    auto operand = popOperand();

    if (type.getName() == "int" || type.getName() == "char" || type.getName() == "bool" || type.getName() == "float") {
        if (operand.getName() != "int" && operand.getName() != "char" && operand.getName() != "bool" && operand.getName() != "float") {
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
