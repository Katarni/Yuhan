//
// Created by Timur Akhmetzianov on 17.11.2024
//

#include "../include/PRNGenerator.h"

void PRNGenerator::push(ReservedMemory* identifier) {
    prn_.emplace_back(identifier);
    types_.push_back(PRNType::Identifier);
    ++cur_;
}

void PRNGenerator::push(Token operation) {
    prn_.emplace_back(operation);
    types_.push_back(PRNType::Operation);
    ++cur_;
}

void PRNGenerator::push(Type literal) {
    prn_.emplace_back(literal);
    types_.push_back(PRNType::Literal);
    ++cur_;
}
