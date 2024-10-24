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

    Token cur_token;
    Token::Type cur_type = Token::Type::CloseParenthesis;

    cur_token.setLine(cur_line_);

    bool string_literal = false, pos_set = false, letter_first = false;

    for (; cur_symbol_ < text_size_ + text_; ++cur_symbol_) {
        if (*cur_symbol_ == '\"') {
            if (!string_literal) {
                if (cur_content.empty()) {
                    string_literal = true;
                    cur_type = Token::Type::StringLiteral;
                    continue;
                } else {
                    break;
                }
            } else {
                ++cur_symbol_;
                break;
            }
        }

        if (string_literal) {
            cur_content += *cur_symbol_;
            continue;
        }

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

        if (cur_type == Token::Type::Another) {
            cur_content += *cur_symbol_;
            continue;
        }

        if (!pos_set) {
            pos_set = true;
            cur_token.setLine(cur_line_);
            cur_token.setColumn(cur_symbol_ - text_);
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
                cur_type = Token::Type::RvalueBinaryOperator;
                continue;
            } else {
                break;
            }
        }

        if (*cur_symbol_ == '+' || *cur_symbol_ == '-') {
            if (cur_content.empty()) {
                cur_content += *cur_symbol_;
                cur_type = Token::Type::RvalueBinaryOperator;
                continue;
            } else if (cur_content[0] == *cur_symbol_ && cur_content.size() == 1) {
                cur_content += *cur_symbol_;
                cur_type = Token::Type::LvalueUnaryOperator;
                ++cur_symbol_;
                break;
            } else {
                break;
            }
        }

        if (*cur_symbol_ == '=') {
            if (cur_content.empty()) {
                cur_content += *cur_symbol_;
                cur_type = Token::Type::LvalueBinaryOperator;
                continue;
            } else {
                if (cur_content.size() == 1 &&
                    (cur_content[0] == '+' || cur_content[0] == '-' || cur_content[0] == '%' ||
                    cur_content[0] == '*' || cur_content[0] == '/' || cur_content[0] == '<' ||
                    cur_content[0] == '>' || cur_content[0] == '|' ||
                    cur_content[0] == '!' || cur_content[0] == '&')) {

                    cur_content += *cur_symbol_;
                    ++cur_symbol_;
                    cur_type = Token::Type::LvalueBinaryOperator;
                } else if (cur_content.size() == 1 && cur_content[0] == '=') {
                    cur_content += *cur_symbol_;
                    cur_type = Token::Type::RvalueBinaryOperator;
                    ++cur_symbol_;
                }

                break;
            }
        }

        if (*cur_symbol_ == '<' || *cur_symbol_ == '>') {
            if (cur_content.empty()) {
                cur_type = Token::Type::LvalueBinaryOperator;
                cur_content += *cur_symbol_;
                continue;
            } else {
                break;
            }
        }

        if (*cur_symbol_ == '!') {
            if (cur_content.empty()) {
                cur_type = Token::Type::RvalueUnaryOperator;
                cur_content += *cur_symbol_;
                continue;
            } else {
                break;
            }
        }

        if (*cur_symbol_ == '_' ||
            'a' <= *cur_symbol_ && *cur_symbol_ <= 'z' ||
            'A' <= *cur_symbol_ && *cur_symbol_ <= 'Z') {

            if (cur_content.empty()) {
                letter_first = true;
                cur_content += *cur_symbol_;
            } else {
                if (cur_type == Token::Type::NumericLiteral) {
                    if (*cur_symbol_ == 'e') {
                        cur_content += *cur_symbol_;
                        cur_type = Token::Type::ExponentialLiteral;
                        letter_first = false;
                    } else {
                        cur_type = Token::Type::Another;
                    }
                    continue;
                } else if (letter_first) {
                    cur_content += *cur_symbol_;
                    continue;
                } else {
                    break;
                }
            }
        }

        if ('0' <= *cur_symbol_ && *cur_symbol_ <= '9') {
            if (cur_content.empty()) {
                cur_type = Token::Type::NumericLiteral;
                cur_content += *cur_symbol_;
                continue;
            } else {
                if (cur_type == Token::Type::NumericLiteral || letter_first ||
                    cur_type == Token::Type::ExponentialLiteral) {
                    cur_content += *cur_symbol_;
                    continue;
                } else {
                    break;
                }
            }
        }

        cur_type = Token::Type::Another;
    }

    if (cur_content.empty()) {
        cur_type = Token::Type::EndOfFile;
    }

    if (letter_first) {
        if (reserved_words_->isInTrie(cur_content)) {
            cur_type = Token::Type::ReservedWord;
            if (cur_content == "and" || cur_content == "or") {
                cur_type = Token::Type::RvalueBinaryOperator;
            }
        } else {
            cur_type = Token::Type::Identifier;
        }
    }

    cur_token.setContent(cur_content);
    cur_token.setType(cur_type);

    return cur_token;
}
