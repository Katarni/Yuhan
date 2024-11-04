// 
// Created by Тимур Ахметзянов on 26.10.24
//

#include "../include/Starter.h"

Starter::Starter(char* file_text, size_t text_size, const std::string& reserved_words_file) : file_text_(file_text), text_size_(text_size) {
    lexer_ = new LexicalAnalyzer(file_text, text_size, reserved_words_file);
}

Starter::~Starter() {
    delete lexer_;
}

std::vector<Token> Starter::getAllTokens() {
    Token cur_token;
    Token eof_token;
    eof_token.setType(Token::Type::EndOfFile);

    std::vector<Token> tokens;

    while (cur_token = lexer_->getToken(), cur_token != eof_token) {
        tokens.push_back(cur_token);
    }

    return tokens;
}
