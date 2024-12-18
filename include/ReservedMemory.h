//
// Created bu Timur Akhmetzianov on 15.11.2024
//

#pragma once

/*!
    \file
    \brief Описание класснов, хранящих данные
    \authors Алена Иванова, Katarni

    Классы, описывающие типы данных, литерары, идентификаторы и занимаемую память
*/

#include "Token.h"


/*!
    \brief Тип данных
    \author Алена Иванова

    Используется для хранения типа у литерала и переменной
*/
class Type {
 public:
    Type();

    explicit Type(std::string name, bool lvalue);

    Type(const Type &other);

    ~Type();

    Type& operator=(const Type &other);

    bool operator==(const Type& other) const; ///< Поэлементное сравнение на равенство
    bool operator!=(const Type& other) const;

    [[nodiscard]]
    bool compareNoLvalue(const Type& other) const; ///< Сравнение типов без учета lvalue/rvalue

    [[nodiscard]]
    bool compareWithCast(const Type &other) const; ///< Сравнение типов с учетов неявных преобразований

    [[nodiscard]]
    bool isLvalue() const;
    [[nodiscard]]
    const std::string& getName() const;
    [[nodiscard]]
    Type getArrayType() const;
    [[nodiscard]]
    size_t getArraySize() const;

    void setArraySize(size_t array_size);
    void setArrayType(Type type);
    void setLvalue(bool is_lvalue);
    void setName(const std::string& name);
    void setFields(const std::vector<std::pair<std::string, Type>>& fields);

    void clear();

    std::map<std::string, Type*> getFields();

 private:
    std::string name_;
    size_t size_array_;
    Type* array_type_;
    bool is_lvalue_;
    std::map<std::string, Type*> fields_; ///< Если тип пользовательский (создан с помощью struct, хранит типы ее полей
};


/*!
    \brief Класс переменной (идентификатора)
    \author Katarni
    \warning Не хранит значение переменной

    Хранит имя переменной и ее тип (тип всегда lvalue)
*/
class Identifier {
 public:
    Identifier() = default;
    Identifier(std::string id, Type type) : name_(id), type_(type) {}

    [[nodiscard]]
    Type getType() const;

    void setType(Type other);

    void setFields(const std::vector<std::pair<std::string, Type>>& fields);

    [[nodiscard]]
    const std::string &getName() const;

    void setName(const std::string &name);

 private:
    std::string name_;
    Type type_;
};


/*!
    \brief Класс литерала
    \author Katarni

    Хранит тип и значение литерала.
    Определение всех операций над литералами
*/
class Literal {
 public:
    Literal() = default;
    Literal(Type type, std::string data);

    [[nodiscard]]
    Type getType() const;
    void setType(Type other);

    [[nodiscard]]
    bool isTrue() const;

    [[nodiscard]]
    std::variant<int, char, bool, float, std::string>& getData();
    std::variant<int, char, bool, float, std::string> getData() const;

    friend std::ostream& operator<<(std::ostream& os, const Literal& lit);

    friend Literal operator!(const Literal& literal);

    friend Literal operator+(const Literal& lhs, const Literal& rhs);
    friend Literal operator-(const Literal& lhs, const Literal& rhs);
    friend Literal operator*(const Literal& lhs, const Literal& rhs);
    friend Literal operator/(const Literal& lhs, const Literal& rhs);
    friend Literal operator%(const Literal& lhs, const Literal& rhs);
    friend Literal operator&&(const Literal& lhs, const Literal& rhs);
    friend Literal operator||(const Literal& lhs, const Literal& rhs);
    friend Literal operator&(const Literal& lhs, const Literal& rhs);
    friend Literal operator|(const Literal& lhs, const Literal& rhs);
    friend Literal operator^(const Literal& lhs, const Literal& rhs);
    friend Literal operator>>(const Literal& lhs, const Literal& rhs);
    friend Literal operator<<(const Literal& lhs, const Literal& rhs);

    friend Literal operator==(const Literal& lhs, const Literal& rhs);
    friend Literal operator!=(const Literal& lhs, const Literal& rhs);
    friend Literal operator<(const Literal& lhs, const Literal& rhs);
    friend Literal operator>(const Literal& lhs, const Literal& rhs);
    friend Literal operator<=(const Literal& lhs, const Literal& rhs);
    friend Literal operator>=(const Literal& lhs, const Literal& rhs);

 protected:
    std::variant<int, char, bool, float, std::string> data_;
    Type type_;

    void setLvalue(bool lvalue) {
        type_.setLvalue(lvalue);
    }
};


/*!
    \brief Занятая память
    \author Katarni
    \warning непосредственно может хранить только массивы и структуры. Если используется базовый тип,
            то хранится в инстансе родителя.

    Хранит данные, наследник Literal. Операции (большинство) исполняются в родительском классе (полиморфизм)
*/
class ReservedMemory : public Literal {
 public:
    ReservedMemory() = default;
    explicit ReservedMemory(Type type);
    ReservedMemory(const Literal& other);
    ReservedMemory(const ReservedMemory& other);
    ReservedMemory& operator=(const ReservedMemory& other);
    ReservedMemory& operator=(const Literal& other);

    [[nodiscard]]
    ReservedMemory*& getFieldByName(const std::string& name);

    void setFields(const std::vector<std::pair<std::string, Type>>& fields);

    friend std::istream& operator>>(std::istream& is, ReservedMemory*& var);

    ReservedMemory*& operator[](const Literal& idx);
    ReservedMemory*& operator[](size_t idx);

    ReservedMemory& operator+=(const Literal& other);
    ReservedMemory& operator-=(const Literal& other);
    ReservedMemory& operator/=(const Literal& other);
    ReservedMemory& operator*=(const Literal& other);
    ReservedMemory& operator%=(const Literal& other);
    ReservedMemory& operator^=(const Literal& other);
    ReservedMemory& operator&=(const Literal& other);
    ReservedMemory& operator|=(const Literal& other);
    ReservedMemory& operator<<=(const Literal& other);
    ReservedMemory& operator>>=(const Literal& other);

    friend Literal operator==(ReservedMemory& lhs, ReservedMemory& rhs);
    friend Literal operator!=(ReservedMemory& lhs, ReservedMemory& rhs);

 private:
    std::variant<std::vector<ReservedMemory*>, std::map<std::string, ReservedMemory*>> structs_data_;
};
