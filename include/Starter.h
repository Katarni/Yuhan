// 
// Created by Тимур Ахметзянов on 26.10.24
//

#pragma once

#include "SyntacticAnalyzer.h"


class Starter {
 public:
    explicit Starter(char* file_text, size_t text_size, const std::string& reserved_words_file);
    ~Starter();

    void startAnalysis();

 private:
    char* file_text_;
    size_t text_size_;
    SyntacticAnalyzer *syntex_;
};
