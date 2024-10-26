// 
// Created by Тимур Ахметзянов on 26.10.24
//

#pragma once

#include "LexicalAnalyzer.h"


class Starter {
 public:
    explicit Starter(char* file_text, size_t text_size);
    ~Starter();

    std::vector<Token> getAllTokens();

 private:
    char* file_text_;
    size_t text_size_;
    LexicalAnalyzer *lexer_;
};
