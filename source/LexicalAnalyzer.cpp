//
// Created by Тимур Ахметзянов on 23.10.2024.
//

#include "../include/LexicalAnalyzer.h"

LexicalAnalyzer::~LexicalAnalyzer() {
    for (auto it = text_; it < text_ + text_size_; ++it) {
        delete it;
    }
}

Token LexicalAnalyzer::getToken() {
    std::string cur_content;
    State state = State::Any;

    Token cur_token;
    Token::Type cur_type = Token::Type::CloseParenthesis;

    cur_token.setLine(cur_line_);

    for (; cur_symbol_ < text_size_ + text_; ++cur_symbol_) {
        if (*cur_symbol_ == ' ') {
            if (cur_content.empty()) {
                continue;
            }

            ++cur_symbol_;
            break;
        }

        if (*cur_symbol_ == '\n') {
            ++cur_line_;
            continue;
        }

        if (*cur_symbol_ == ';') {
            if (cur_content.empty()) {
                cur_content += *cur_symbol_;
                ++cur_symbol_;
                cur_type = Token::Type::Semicolon;
            }

            break;
        }

        if (*cur_symbol_ == '(') {
            if (cur_content.empty()) {
                cur_content += *cur_symbol_;
                ++cur_symbol_;
                cur_type = Token::Type::OpenParenthesis;
            }

            break;
        }
        if (*cur_symbol_ == ')') {
            if (cur_content.empty()) {
                cur_content += *cur_symbol_;
                ++cur_symbol_;
                cur_type = Token::Type::CloseParenthesis;
            }

            break;
        }

        if (*cur_symbol_ == '[') {
            if (cur_content.empty()) {
                cur_content += *cur_symbol_;
                ++cur_symbol_;
                cur_type = Token::Type::OpenSquareBracket;
            }

            break;
        }
        if (*cur_symbol_ == ']') {
            if (cur_content.empty()) {
                cur_content += *cur_symbol_;
                ++cur_symbol_;
                cur_type = Token::Type::CloseSquareBracket;
            }

            break;
        }

        if (*cur_symbol_ == '{') {
            if (cur_content.empty()) {
                cur_content += *cur_symbol_;
                ++cur_symbol_;
                cur_type = Token::Type::OpenCurlyBrace;
            }

            break;
        }
        if (*cur_symbol_ == '}') {
            if (cur_content.empty()) {
                cur_content += *cur_symbol_;
                ++cur_symbol_;
                cur_type = Token::Type::CloseCurlyBrace;
            }

            break;
        }

        if (*cur_symbol_ == '.') {
            if (cur_content.empty()) {
                cur_content += *cur_symbol_;
                ++cur_symbol_;
                cur_type = Token::Type::Dot;
            }

            break;
        }

        if (*cur_symbol_ == ',') {
            if (cur_content.empty()) {
                cur_content += *cur_symbol_;
                ++cur_symbol_;
                cur_type = Token::Type::Comma;
            }

            break;
        }

        if (*cur_symbol_ == '%' || *cur_symbol_ == '/' || *cur_symbol_ == '|' ||
            *cur_symbol_ == '&' || *cur_symbol_ == '*') {
            if (cur_content.empty()) {
                cur_content += *cur_symbol_;
                cur_type = Token::Type::Operator;
            } else {
                break;
            }
        }

        if (*cur_symbol_ == '+' || *cur_symbol_ == '-') {
            if (cur_content.empty()) {
                cur_content += *cur_symbol_;
                cur_type = Token::Type::Operator;
            } else if (cur_content[0] == *cur_symbol_ && cur_content.size() == 1) {
                cur_content += *cur_symbol_;
                cur_type = Token::Type::UnaryOperation;
                ++cur_symbol_;
                break;
            } else {
                break;
            }
        }

        if (*cur_symbol_ == '=') {

        }

        if (*cur_symbol_ == '_' ||
            'a' <= *cur_symbol_ && *cur_symbol_ <= 'z' ||
            'A' <= *cur_symbol_ && *cur_symbol_ <= 'Z') {

        }

        if ('0' <= *cur_symbol_ && *cur_symbol_ <= '9') {

        }
    }

    if (cur_type == Token::Type::CloseParenthesis && cur_content != ")") {

    }

    cur_token.setContent(cur_content);
    cur_token.setType(cur_type);

    return cur_token;
}
