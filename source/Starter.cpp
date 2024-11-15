// 
// Created by Тимур Ахметзянов on 26.10.24
//

#include "../include/Starter.h"

Starter::Starter(char* file_text, size_t text_size, const std::string& reserved_words_file) : file_text_(file_text), text_size_(text_size) {
    auto lexer = new LexicalAnalyzer(file_text, text_size, reserved_words_file);
    syntex_ = new SyntacticAnalyzer(lexer);
}

Starter::~Starter() {
    delete syntex_;
}

void Starter::startAnalysis() {
    syntex_->startAnalysis();
    std::cout << "OK";
}
