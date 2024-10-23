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
        Comma, Punctuation
    };

    Token(const std::string &content, size_t line, size_t col, Type type) : std::exception(),
                                                                            line_(line), column_(col),
                                                                            type_(type), content_(content) {
        what_ = "in " + std::to_string(line_) + ":" + std::to_string(column_);
        what_ += " error: unexpected token: \"" + content_ + "\" with type: ";
        switch (type_) {
            case Type::ReservedWord:
                what_ += "reserved word";
                break;
            case Type::Identifier:
                what_ += "identifier";
                break;
            case Type::Operator:
                what_ += "operator";
                break;
            case Type::Dot:
                what_ += "dot";
                break;
            case Type::Comma:
                what_ += "comma";
                break;
            case Type::Punctuation:
                what_ += "bracket";
                break;
        }
    }

    Token(std::string &&content, size_t line, size_t col, Type type) : std::exception(),
                                                                    line_(line), column_(col),
                                                                    type_(type),
                                                                    content_(std::move(content)) {
        what_ = "in " + std::to_string(line_) + ":" + std::to_string(column_);
        what_ += " error: unexpected token: \"" + content_ + "\" with type: ";
        switch (type_) {
            case Type::ReservedWord:
                what_ += "reserved word";
                break;
            case Type::Identifier:
                what_ += "identifier";
                break;
            case Type::Operator:
                what_ += "operator";
                break;
            case Type::Dot:
                what_ += "dot";
                break;
            case Type::Comma:
                what_ += "comma";
                break;
            case Type::Punctuation:
                what_ += "bracket";
                break;
        }
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
};
