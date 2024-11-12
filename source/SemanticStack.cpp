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

void SemanticStack::checkBool() {
    auto operand = popOperand();
    if (operand.getName() != "int" && operand.getName() != "float" &&
        operand.getName() != "char" && operand.getName() != "bool") {
        throw std::runtime_error("can't cast operator to bool");
    }
}

void SemanticStack::checkBinary() {
    auto rhs = popOperand();
    auto operation = popOperation();
    auto lhs = popOperand();

    Type result_type;
    
    if (operation.getContent() == "&" || operation.getContent() == "|" || operation.getContent() == "and" || operation.getContent() == "or" ||
        operation.getContent() == "*" || operation.getContent() == "/" || operation.getContent() == "-" || operation.getContent() == "<<" ||
        operation.getContent() == "^" || operation.getContent() == ">>") {
        // check any number
    } else if (operation.getContent() == "+") {
        // check any number and sometimes string
    } else if (operation.getContent() == ".") {
        // check structure
    } else if (operation.getContent() == "+=") {
        // check any number and sometimes string, lhs is lvalue
    } else if (operation.getContent() == "&" || operation.getContent() == "|" || operation.getContent() == "and" || operation.getContent() == "or" ||
                operation.getContent() == "*" || operation.getContent() == "/" || operation.getContent() == "-" || operation.getContent() == "<<" ||
                operation.getContent() == "^" || operation.getContent() == ">>") {
        // check any number, lhs is lvalue
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
        }
    } else if (operation.getContent() == "<" || operation.getContent() == ">" || operation.getContent() == ">=" || operation.getContent() == "<=") {

    } else {
        throw std::runtime_error("operation is incorrect");
    }

    final_push:
    push(result_type);
}

