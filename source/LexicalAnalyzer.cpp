//
// Created by Тимур Ахметзянов on 23.10.2024.
//

#include "../include/LexicalAnalyzer.h"

LexicalAnalyzer::LexicalAnalyzer(char *text, size_t text_size,
                                 const std::string& reserved_words_file) : text_(text),
                                                                            cur_symbol_(text),
                                                                            text_size_(text_size),
                                                                            cur_line_(0), cur_col_(0) {
    std::ifstream reserved_words_fin(reserved_words_file);
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
    bool comment = false, char_literal = false, ignore_next = false;

    for (; cur_symbol_ < text_size_ + text_; ++cur_symbol_, ++cur_col_) {
        if (*cur_symbol_ == '\0' || *cur_symbol_ == '\r') {
            continue;
        }

        if (comment) {
            if (*cur_symbol_ == '\n') {
                comment = false;
                ++cur_line_;
                cur_col_ = -1;
            }
            continue;
        }

        if (ignore_next) {
            switch (*cur_symbol_) {
                case 'n':
                    cur_content += '\n';
                    break;
                case '\'':
                    cur_content += '\'';
                    break;
                case '\"':
                    cur_content += '\"';
                    break;
                case '0':
                    cur_content += '\0';
                    break;
                case 'r':
                    cur_content += '\r';
                    break;
                case '\\':
                    cur_content += '\\';
                    break;
                default:
                    cur_type = Token::Type::Another;
            }
            ignore_next = false;
            continue;
        }

        if (*cur_symbol_ == '\\'  && (string_literal || char_literal)) {
            ignore_next = true;
            continue;
        }

        if (*cur_symbol_ == '\"' && !char_literal) {
            if (!string_literal) {
                if (cur_content.empty()) {
                    string_literal = true;
                    cur_type = Token::Type::StringLiteral;
                    pos_set = true;
                    cur_token.setLine(cur_line_);
                    cur_token.setColumn(cur_col_);
                    continue;
                } else {
                    break;
                }
            } else {
                ++cur_symbol_;
                string_literal = false;
                break;
            }
        }

        if (*cur_symbol_ == '\'' && !string_literal) {
            if (!char_literal) {
                if (cur_content.empty()) {
                    char_literal = true;
                    cur_type = Token::Type::CharLiteral;
                    pos_set = true;
                    cur_token.setLine(cur_line_);
                    cur_token.setColumn(cur_col_);
                    continue;
                } else {
                    break;
                }
            } else {
                ++cur_symbol_;
                char_literal = false;
                break;
            }
        }

        if (string_literal || char_literal) {
            cur_content += *cur_symbol_;
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
                ++cur_symbol_;
                ++cur_col_;
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
                if (*cur_symbol_ == '&') {
                    cur_type = Token::Type::Ampersand;
                } else {
                    cur_type = Token::Type::RvalueBinaryOperator;
                }
                continue;
            } else if (cur_content.size() == 1 && *cur_symbol_ == '/' && cur_content[0] == '/') {
                comment = true;
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

        if (*cur_symbol_ == ':') {
            if (cur_content.empty()) {
                cur_content += *cur_symbol_;
                cur_type = Token::Type::Another;
                continue;
            } else {
                if (letter_first) {
                    if (cur_content.size() >= 2 && cur_content[cur_content.size() - 1] == ':' && cur_content[cur_content.size() - 2] != ':'
                         || cur_content.size() < 2 || cur_content.size() >= 1 && cur_content[cur_content.size() - 1] != ':') {
                        cur_content += *cur_symbol_;
                        continue;
                    } else {
                        cur_type = Token::Type::Another;
                        letter_first = false;
                        continue;
                    }
                } else {
                    break;
                }
            }
        }

        if (*cur_symbol_ == '<' || *cur_symbol_ == '>') {
            if (cur_content.empty()) {
                if (*cur_symbol_ == '<') {
                    cur_type = Token::Type::LessThan;
                } else {
                    cur_type = Token::Type::GreaterThan;
                }
                cur_content += *cur_symbol_;
                continue;
            } else {
                if (cur_content.size() == 1 && cur_content[0] == *cur_symbol_) {
                    cur_content += *cur_symbol_;
                    cur_type = Token::Type::RvalueBinaryOperator;
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
                    if (cur_content.size() >= 2 && cur_content[cur_content.size() - 1] == ':' && cur_content[cur_content.size() - 2] == ':' ||
                        cur_content.size() >= 2 && cur_content[cur_content.size() - 1] != ':' && cur_content[cur_content.size() - 2] != ':' ||
                        cur_content.size() < 2) {
                        cur_content += *cur_symbol_;
                        continue;
                    } else {
                        cur_type = Token::Type::Another;
                        letter_first = false;
                        cur_content += *cur_symbol_;
                        continue;
                    }
                } else if (cur_type == Token::Type::ExponentialLiteral || cur_type == Token::Type::FloatLiteral) {
                    cur_content += *cur_symbol_;
                    cur_type = Token::Type::Another;
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

    if (cur_content.empty() && cur_type == Token::Type::CloseParenthesis) {
        cur_type = Token::Type::EndOfFile;
    }

    if (cur_type == Token::Type::ExponentialLiteral && cur_content.back() == 'e' || 
        cur_type == Token::Type::FloatLiteral && cur_content.back() == '.' ||
        cur_type == Token::Type::CharLiteral && cur_content.size() != 1 ||
        cur_type == Token::Type::StringLiteral && string_literal || 
        cur_type == Token::Type::CharLiteral && char_literal) {
        cur_type = Token::Type::Another;
    }

    if (letter_first) {
        if (cur_content.back() == ':') {
            cur_type = Token::Type::Another;
        } else if (reserved_words_->isInTrie(cur_content)) {
            cur_type = Token::Type::ReservedWord;
            if (cur_content == "and" || cur_content == "or") {
                cur_type = Token::Type::RvalueBinaryOperator;
            } else if (cur_content == "false") {
                cur_type = Token::Type::NumericLiteral;
                cur_content = "0";
            } else if (cur_content == "true") {
                cur_type = Token::Type::NumericLiteral;
                cur_content = "1";
            }
        } else if (cur_content.find(":") != std::string::npos) {
            cur_type = Token::Type::NamespaceIdentifier; 
        } else {
            cur_type = Token::Type::Identifier;
        }
    }

    cur_token.setContent(cur_content);
    cur_token.setType(cur_type);

    return cur_token;
}
