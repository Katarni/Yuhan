//
// Created by Тимур Ахметзянов on 23.10.2024.
//

#pragma once

#include "includes.h"


/*!
    \brief Класс лексемы
    \author Katarni
    \warning Наследован от std::exception

    Используется для хранения лексемы, ее типа и местарасполажения.
    Наследован от std::exception.
*/
class Token : public std::exception {
 public:
    /// Набор возможных типов лексемы
    enum class Type {
        ReservedWord, Identifier,
        LvalueBinaryOperator, RvalueBinaryOperator,
        Dot, Comma, NumericLiteral, Semicolon,
        OpenParenthesis, CloseParenthesis,
        OpenCurlyBrace, CloseCurlyBrace,
        OpenSquareBracket, CloseSquareBracket,
        LvalueUnaryOperator, RvalueUnaryOperator,
        StringLiteral, EndOfFile, Another, ExponentialLiteral,
        PlusOrMinusOperator, FloatLiteral, CharLiteral, 
        LessThan, GreaterThan, Colon, DoubleColon, Ampersand, 
        NamespaceIdentifier
    };

    Token() : std::exception(), line_(0), column_(0), type_(Type::Another) {}
    Token(const Token& other) = default;

    [[nodiscard]]
    const char* what() const noexcept override {
        return what_.c_str();
    }

    friend bool operator==(const Token& lhs, const Token& rhs);

    void setLine(size_t line);
    void setColumn(size_t column);
    void setType(Type type);
    void setContent(const std::string &content);

    [[nodiscard]]
    const std::string &getContent() const;

    [[nodiscard]]
    size_t getLine() const;

    [[nodiscard]]
    size_t getColumn() const;

    [[nodiscard]]
    Type getType() const;

 private:
    size_t line_, column_;
    Type type_;
    std::string content_, what_;

    void generateWhat(); ///< Генерирует описание ошибки при изменении полей класса
};
