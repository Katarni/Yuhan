//
// Created by Тимур Ахметзянов on 23.10.2024.
//

#pragma once

#include "includes.h"


class Token : public std::exception {
 public:
    enum class Type {
        ReservedWord, Identifier,
        Operator, Dot,
        Comma, Literal, Semicolon,
        OpenParenthesis, CloseParenthesis,
        OpenCurlyBrace, CloseCurlyBrace,
        OpenSquareBracket, CloseSquareBracket,
        Another
    };

    Token(const std::string &content, size_t line, size_t col, Type type) : std::exception(),
                                                                            line_(line), column_(col),
                                                                            type_(type), content_(content) {
        generateWhat();
    }

    Token(std::string &&content, size_t line, size_t col, Type type) : std::exception(),
                                                                    line_(line), column_(col),
                                                                    type_(type),
                                                                    content_(std::move(content)) {
        generateWhat();
    }

    [[nodiscard]]
    const char* what() const noexcept override {
        return what_.c_str();
    }

    friend bool operator==(const Token& lhs, const Token& rhs) {
        return lhs.content_ == rhs.content_;
    }

 private:
    size_t line_, column_;
    Type type_;
    std::string content_, what_;

    void generateWhat() {
        what_ = "in " + std::to_string(line_) + ":" + std::to_string(column_);
        what_ += " error: unexpected ";
        switch (type_) {
            case Type::ReservedWord:
                what_ += "reserved word: \"" + content_ + "\n";
                break;
            case Type::Identifier:
                what_ += "identifier: \"" + content_ + "\n";
                break;
            case Type::Operator:
                what_ += "operator: \"" + content_ + "\n";
                break;
            case Type::Dot:
                what_ += "dot";
                break;
            case Type::Comma:
                what_ += "comma";
                break;
            case Type::Literal:
                what_ += "literal: \"" + content_ + "\n";
                break;
            case Type::Semicolon:
                what_ += "semicolon";
                break;
            case Type::OpenParenthesis:
                what_ += "opening parenthesis";
                break;
            case Type::CloseParenthesis:
                what_ += "closing parenthesis";
                break;
            case Type::OpenCurlyBrace:
                what_ += "opening curly brace";
                break;
            case Type::CloseCurlyBrace:
                what_ += "closing curly brace";
                break;
            case Type::OpenSquareBracket:
                what_ += "opening square bracket";
                break;
            case Type::CloseSquareBracket:
                what_ += "closing square bracket";
                break;
            case Type::Another:
                what_ += "symbol: \"" + content_ + "\"";
                break;
        }
    }
};
