//
// Created bu Timur Akhmetzianov on 15.11.2024
//

#pragma once

#include "Trie.h"
#include "Token.h"


class ReservedMemory {
 public:
    ReservedMemory(Type type);

 private:
    Type type_;
    std::variant<int, bool, float, char, std::string, std::vector<ReservedMemory*>> data_;
};
