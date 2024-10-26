//
// Created by Тимур Ахметзянов on 23.10.2024.
//

#include "../include/LexicalAnalyzer.h"

LexicalAnalyzer::LexicalAnalyzer(char *text, size_t text_size) : text_(text), cur_symbol_(text), text_size_(text_size),
                                                                 cur_line_(0), cur_col_(0) {
    std::ifstream reserved_words_fin("../reserved-words.txt");
    reserved_words_fin.seekg(0, std::ios::end);
    std::streamsize reserved_words_file_size = reserved_words_fin.tellg();
    reserved_words_fin.seekg(0, std::ios::beg);

    auto reserved_words_text = new char[reserved_words_file_size];
    reserved_words_fin.read(reserved_words_text, reserved_words_file_size);

    std::vector<std::pair<char *, size_t>> words_ptrs;
    words_ptrs.emplace_back(reserved_words_text, 0);
    for (int i = 0; i < reserved_words_file_size; ++i) {
        if (*(reserved_words_text + i) == '\n') {
            words_ptrs.back().second = reserved_words_text + i - words_ptrs.back().first;

            if (i != reserved_words_file_size - 1) {
                words_ptrs.emplace_back((reserved_words_text + i + 1), 0);
            }
        }
    }

    reserved_words_ = new Trie;
    for (auto [start, length]: words_ptrs) {
        if (length == 0) continue;

        reserved_words_->insert(start, length);
    }

    delete[] reserved_words_text;
}

LexicalAnalyzer::~LexicalAnalyzer() {
    delete[] text_;
}

Token LexicalAnalyzer::getToken() {
    std::string cur_content;

    Token cur_token;
    Token::Type cur_type = Token::Type::CloseParenthesis;

    bool string_literal = false, pos_set = false, letter_first = false;
    bool comment_ = false;

    for (; cur_symbol_ < text_size_ + text_; ++cur_symbol_, ++cur_col_) {
        if (*cur_symbol_ == '\0' || *cur_symbol_ == '\r') {
            continue;
        }

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

        if (comment_) {
            if (*cur_symbol_ == '\n') {
                comment_ = false;
                ++cur_line_;
                cur_col_ = -1;
            }
            continue;
        }

        if (*cur_symbol_ == ' ') {
            if (cur_content.empty()) {
                continue;
            }

            ++cur_symbol_;
            ++cur_col_;
            break;
        }

        if (*cur_symbol_ == '\n') {
            cur_col_ = -1;
            if (cur_content.empty()) {
                ++cur_line_;
                continue;
            } else {
                break;
            }
        }

        if (!pos_set) {
            pos_set = true;
            cur_token.setLine(cur_line_);
            cur_token.setColumn(cur_col_);
        }

        if (*cur_symbol_ == ';' || *cur_symbol_ == '(' || *cur_symbol_ == ')' || 
            *cur_symbol_ == '[' || *cur_symbol_ == ']' || *cur_symbol_ == '{' || 
            *cur_symbol_ == '}' || *cur_symbol_ == ',') {
            if (cur_content.empty()) {
                cur_content += *cur_symbol_;
                ++cur_symbol_;
                ++cur_col_;
                if (*cur_symbol_ == ';') {
                    cur_type = Token::Type::Semicolon;
                } else if (*cur_symbol_ == '(') {
                    cur_type = Token::Type::OpenParenthesis;
                } else if (*cur_symbol_ == ')') {
                    cur_type = Token::Type::CloseParenthesis;
                } else if (*cur_symbol_ == '[') {
                    cur_type = Token::Type::OpenSquareBracket;
                } else if (*cur_symbol_ == ']') {
                    cur_type = Token::Type::CloseSquareBracket;
                } else if (*cur_symbol_ == '{') {
                    cur_type = Token::Type::OpenCurlyBrace;
                } else if (*cur_symbol_ == '}') {
                    cur_type = Token::Type::CloseCurlyBrace;
                } else if (*cur_symbol_ == ',') {
                    cur_type = Token::Type::Comma;
                }
            }

            break;
        }

        if (*cur_symbol_ == '.') {
            if (cur_content.empty()) {
                cur_content += *cur_symbol_;
                ++cur_symbol_;
                ++cur_col_;
                cur_type = Token::Type::Dot;
                break;
            } else {
                if (cur_type == Token::Type::NumericLiteral) {
                    cur_type = Token::Type::FloatLiteral;
                    cur_content += *cur_symbol_;
                    continue;
                } else {
                    break;
                }
            }
        }

        if (*cur_symbol_ == '%' || *cur_symbol_ == '|' || *cur_symbol_ == '/' ||
            *cur_symbol_ == '&' || *cur_symbol_ == '*' || *cur_symbol_ == '^') {
            if (cur_content.empty()) {
                cur_content += *cur_symbol_;
                cur_type = Token::Type::RvalueBinaryOperator;
                continue;
            } else if (cur_content.size() == 1 && *cur_symbol_ == '/' && cur_content[0] == '/') {
                comment_ = true;
                cur_content.clear();
                cur_type = Token::Type::CloseParenthesis;
                pos_set = false;
                continue;
            } else {
                break;
            }
        }

        if (*cur_symbol_ == '+' || *cur_symbol_ == '-') {
            if (cur_content.empty()) {
                cur_content += *cur_symbol_;
                cur_type = Token::Type::PlusOrMinusOperator;
                continue;
            } else if (cur_content[0] == *cur_symbol_ && cur_content.size() == 1) {
                cur_content += *cur_symbol_;
                cur_type = Token::Type::LvalueUnaryOperator;
                ++cur_symbol_;
                ++cur_col_;
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
                if (cur_content.size() == 1 && (cur_content[0] == '+' || cur_content[0] == '-' ||
                    cur_content[0] == '%' || cur_content[0] == '*' || cur_content[0] == '/' ||
                    cur_content[0] == '|' || cur_content[0] == '&' || cur_content[0] == '^') ||
                    cur_content.size() == 2 && (cur_content == "<<" || cur_content == ">>")) {
                    cur_type = Token::Type::LvalueBinaryOperator;
                } else if (cur_content.size() == 1 &&
                            (cur_content[0] == '=' || cur_content[0] == '<' ||
                            cur_content[0] == '>' || cur_content[0] == '!')) {
                    cur_type = Token::Type::RvalueBinaryOperator;
                }

                cur_content += *cur_symbol_;
                ++cur_symbol_;
                ++cur_col_;

                break;
            }
        }

        if (*cur_symbol_ == '<' || *cur_symbol_ == '>') {
            if (cur_content.empty()) {
                cur_type = Token::Type::RvalueBinaryOperator;
                cur_content += *cur_symbol_;
                continue;
            } else {
                if (cur_content.size() == 1 && cur_content[0] == *cur_symbol_) {
                    cur_content += *cur_symbol_;
                    continue;
                } else {
                    break;
                }
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

        if (cur_type == Token::Type::Another) {
            cur_content += *cur_symbol_;
            continue;
        }

        if (*cur_symbol_ == '_' ||
            ('a' <= *cur_symbol_ && *cur_symbol_ <= 'z') ||
            ('A' <= *cur_symbol_ && *cur_symbol_ <= 'Z')) {

            if (cur_content.empty()) {
                letter_first = true;
                cur_content += *cur_symbol_;
                continue;
            } else {
                if (cur_type == Token::Type::NumericLiteral) {
                    if (*cur_symbol_ == 'e') {
                        cur_content += *cur_symbol_;
                        cur_type = Token::Type::ExponentialLiteral;
                        letter_first = false;
                    } else {
                        cur_content += *cur_symbol_;
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
                    cur_type == Token::Type::ExponentialLiteral ||
                    cur_type == Token::Type::FloatLiteral) {
                    cur_content += *cur_symbol_;
                    continue;
                } else {
                    break;
                }
            }
        }

        cur_content += *cur_symbol_;
        cur_type = Token::Type::Another;
        letter_first = false;
    }

    if (cur_content.empty()) {
        cur_type = Token::Type::EndOfFile;
    }

    if (cur_type == Token::Type::ExponentialLiteral && cur_content.back() == 'e' || 
        cur_type == Token::Type::FloatLiteral && cur_content.back() == '.') {
        cur_type = Token::Type::Another;
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
