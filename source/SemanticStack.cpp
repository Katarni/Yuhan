#include "../include/SemanticStack.h"

void SemanticStack::push(const Token& operation) {
    sem_stack_.emplace(operation);
}

void SemanticStack::push(const Type& operand) {
    sem_stack_.emplace(operand);
}
