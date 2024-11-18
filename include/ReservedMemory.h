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

    Type operator=(const Type &other);

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

 private:
    std::string name_;
    size_t size_array_;
    Type* array_type_;
    bool is_lvalue_;
};


class ReservedMemory {
 public:
    explicit ReservedMemory(Type type);

    [[nodiscard]]
    Type getType() const {
        return type_;
    }

 private:
    Type type_;
    std::variant<int, bool, float, char, std::string, std::vector<ReservedMemory*>> data_;
};
