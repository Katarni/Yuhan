//
// Created bu Timur Akhmetzianov on 15.11.2024
//

#pragma once

#include "Token.h"


class Type {
 public:
    Type();

    explicit Type(std::string name, bool lvalue);

    Type(const Type &other);

    ~Type();

    Type& operator=(const Type &other);

    bool operator==(const Type& other) const;
    bool operator!=(const Type& other) const;

    [[nodiscard]]
    bool compareNoLvalue(const Type& other) const;

    [[nodiscard]]
    bool compareWithCast(const Type &other) const;

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
    std::map<std::string, Type*> fields_;
};


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
