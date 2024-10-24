//
// Created by Тимур Ахметзянов on 24.10.2024.
//

#include "../include/Token.h"

void Token::generateWhat() {
    what_ = "in " + std::to_string(line_) + ":" + std::to_string(column_);
    what_ += " error: unexpected ";
    switch (type_) {
        case Type::ReservedWord:
            what_ += "reserved word: \"" + content_ + "\n";
            break;
        case Type::Identifier:
            what_ += "identifier: \"" + content_ + "\n";
            break;
        case Type::RvalueBinaryOperator:
        case Type::LvalueBinaryOperator:
            what_ += "binary operator: \"" + content_ + "\n";
            break;
        case Type::Dot:
            what_ += "dot";
            break;
        case Type::Comma:
            what_ += "comma";
            break;
        case Type::StringLiteral:
        case Type::ExponentialLiteral:
        case Type::NumericLiteral:
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
        case Type::EndOfFile:
        case Type::Another:
            what_ += "symbol: \"" + content_ + "\"";
            break;
        case Type::RvalueUnaryOperator:
        case Type::LvalueUnaryOperator:
            what_ += "unary operation";
            break;
    }
}

void Token::setLine(size_t line) {
    line_ = line;
    generateWhat();
}

void Token::setColumn(size_t column) {
    column_ = column;
    generateWhat();
}

void Token::setType(Token::Type type) {
    type_ = type;
    generateWhat();
}

void Token::setContent(const std::string &content) {
    content_ = content;
    generateWhat();
}

const std::string &Token::getContent() const {
    return content_;
}

size_t Token::getLine() const {
    return line_;
}

size_t Token::getColumn() const {
    return column_;
}

Token::Type Token::getType() const {
    return type_;
}

bool operator==(const Token &lhs, const Token &rhs) {
    return lhs.content_ == rhs.content_ && lhs.type_ == rhs.type_;
}
