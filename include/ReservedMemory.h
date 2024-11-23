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

    bool isTrue() const;

    [[nodiscard]]
    std::variant<int, char, bool, float, std::string> getData() const;

    friend std::ostream& operator<<(std::ostream& os, Literal lit);

 private:
    std::variant<int, char, bool, float, std::string> data_;
    Type type_;
};


class ReservedMemory {
 public:
    explicit ReservedMemory(Type type);
    ReservedMemory(const ReservedMemory& other) = default;
    ReservedMemory& operator=(const ReservedMemory& other) = default;

    [[nodiscard]]
    Type& getType() {
        return type_;
    }

    bool isTrue() const;

    [[nodiscard]]
    ReservedMemory* getFieldByName(const std::string& name) const;

    void setFields(const std::vector<std::pair<std::string, Type>>& fields);

    friend std::ostream& operator<<(std::ostream& os, ReservedMemory* var);
    friend std::istream& operator>>(std::istream& is, ReservedMemory* var);

 private:
    Type type_;
    std::variant<int, bool, float, char, std::string,
                std::vector<ReservedMemory*>, std::map<std::string, ReservedMemory*>> data_;
};
