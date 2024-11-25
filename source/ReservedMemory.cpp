#include "../include/ReservedMemory.h"

bool Type::isLvalue() const {
    return is_lvalue_;
}

const std::string &Type::getName() const {
    return name_;
}

void Type::setLvalue(bool is_lvalue) {
    is_lvalue_ = is_lvalue;
}

void Type::setName(const std::string &name) {
    name_ = name;
}

Type::Type(std::string name, bool lvalue) : name_(std::move(name)), size_array_(0), array_type_(nullptr),
                                            is_lvalue_(lvalue) {}

Type::Type(const Type &other) {
    name_ = other.name_;
    size_array_ = other.size_array_;
    if (other.array_type_ != nullptr) {
        array_type_ = new Type(*other.array_type_);
    } else {
        array_type_ = nullptr;
    }
    for (const auto& [key, val] : other.fields_) {
        if (val != nullptr) {
            fields_[key] = new Type();
            *fields_[key] = *val;
        }
    }
    is_lvalue_ = other.is_lvalue_;
}

Type& Type::operator=(const Type &other) {
    name_ = other.name_;
    size_array_ = other.size_array_;
    if (other.array_type_ != nullptr) {
        array_type_ = new Type(*other.array_type_);
    } else {
        array_type_ = nullptr;
    }
    for (const auto& [key, val] : other.fields_) {
        if (val != nullptr) {
            fields_[key] = new Type();
            *fields_[key] = *val;
        }
    }
    is_lvalue_ = other.is_lvalue_;
    return *this;
}

void Type::setArrayType(Type type) {
    array_type_ = new Type(type);
}

void Type::setArraySize(size_t array_size) {
    array_size = array_size;
}

Type::~Type() {
    clear();
    if (array_type_ == nullptr) return;
    delete array_type_;
}

Type Type::getArrayType() const {
    return *array_type_;
}

Type::Type() : name_(""), size_array_(0), array_type_(nullptr), is_lvalue_(false) {}

bool Type::compareWithCast(const Type &other) const {
    if (name_ != "array" && name_ == other.name_) {
        return true;
    }

    if (name_ == "int" || name_ == "char" ||
        name_ == "float" || name_ == "bool") {
        if (other.name_ == "int" || other.name_ == "char" ||
            other.name_ == "float" || other.name_ == "bool") {
            return true;
        }
        return false;
    }
    if (name_ == "string") {
        if (other.name_ == "string") {
            return true;
        }
        return false;
    }
    if (name_ != "array" && other.name_ == name_) {
        return true;
    }
    if (size_array_ != other.size_array_) {
        return false;
    }
    return array_type_->compareWithCast(*other.array_type_);
}

size_t Type::getArraySize() const {
    return size_array_;
}

ReservedMemory::ReservedMemory(Type type) : Literal(type, "") {
    setLvalue(true);
}

ReservedMemory *ReservedMemory::getFieldByName(const std::string &name) const {
    return std::get<std::map<std::string, ReservedMemory*>>(structs_data_).find(name)->second;
}

void ReservedMemory::setFields(const std::vector<std::pair<std::string, Type>> &fields) {
    if (!std::holds_alternative<std::map<std::string, ReservedMemory*>>(structs_data_)) {
        throw std::runtime_error("expected struct");
    }

    for (const auto& [name, type] : fields) {
        std::get<std::map<std::string, ReservedMemory*>>(structs_data_)[name] = new ReservedMemory(type);
    }
}

void Identifier::setFields(const std::vector<std::pair<std::string, Type>> &fields) {
    type_.setFields(fields);
}

void Type::setFields(const std::vector<std::pair<std::string, Type>> &fields) {
    for (const auto& [name, type] : fields) {
        fields_[name] = new Type(type);
        fields_[name]->setLvalue(true);
    }
}

void Identifier::setType(Type other) {
    type_.clear();
    type_ = other;
    type_.setLvalue(true);
}

const std::string &Identifier::getName() const {
    return name_;
}

Type Identifier::getType() const {
    return type_;
}

void Identifier::setName(const std::string &name) {
    name_ = name;
}

void Type::clear() {
    for (const auto& [key, val] : fields_) {
        delete val;
    }
}

Literal::Literal(Type type, std::string data) : type_(type) {
    if (data.empty() && type_.getName() != "string") {
        data = "0";
    }

    if (type_.getName() == "char") {
        data_ = static_cast<char>(data[0]);
    } else if (type_.getName() == "int") {
        data_ = std::stoi(data);
    } else if (type_.getName() == "float") {
        data_ = std::stof(data);
    } else if (type_.getName() == "string") {
        data_ = data;
    } else if (type_.getName() == "bool") {
        data_ = bool(std::stoi(data));
    }
}

Type Literal::getType() const {
    return type_;
}

void Literal::setType(Type other) {
    type_ = other;
}

std::variant<int, char, bool, float, std::string>& Literal::getData() {
    return data_;
}

bool Literal::isTrue() const {
    if (type_.getName() == "int") {
        return std::get<int>(data_);
    } else if (type_.getName() == "bool") {
        return std::get<bool>(data_);
    } else if (type_.getName() == "char") {
        return std::get<char>(data_);
    } else if (type_.getName() == "float") {
        return static_cast<bool>(std::get<float>(data_));
    }

    throw std::runtime_error("can't compare types");
}

std::istream &operator>>(std::istream &is, ReservedMemory *&var) {
    if (var->getType().getName() == "int") {
        is >> std::get<int>(var->getData());
    } else if (var->getType().getName() == "bool") {
        is >> std::get<bool>(var->getData());
    } else if (var->getType().getName() == "char") {
        is >> std::get<char>(var->getData());
    } else if (var->getType().getName() == "float") {
        is >> std::get<float>(var->getData());
    } else if (var->getType().getName() == "string") {
        is >> std::get<std::string>(var->getData());
    } else {
        throw std::runtime_error("no available scan overload");
    }

    return is;
}

std::ostream &operator<<(std::ostream &os, const Literal& lit) {
    if (lit.type_.getName() == "int") {
        std::cout << std::get<int>(lit.data_);
    } else if (lit.type_.getName() == "bool") {
        std::cout << std::get<bool>(lit.data_);
    } else if (lit.type_.getName() == "char") {
        std::cout << std::get<char>(lit.data_);
    } else if (lit.type_.getName() == "float") {
        std::cout << std::get<float>(lit.data_);
    } else if (lit.type_.getName() == "string") {
        std::cout << std::get<std::string>(lit.data_);
    } else {
        throw std::runtime_error("no available print overload");
    }

    return os;
}

Literal operator+(Literal& lhs, Literal& rhs) {
    if (lhs.type_.getName() == "string" || rhs.type_.getName() == "string") {
        return {lhs.type_, std::get<std::string>(lhs.data_) + std::get<std::string>(rhs.data_)};
    }

    if (lhs.type_.getName() == "float") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(std::get<float>(lhs.data_) + std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) +
                                              static_cast<float>(std::get<int>(rhs.data_)))};
        } else if (rhs.type_.getName() == "char") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) +
                                              static_cast<float>(std::get<char>(rhs.data_)))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) +
                                              static_cast<float>(std::get<bool>(rhs.data_)))};
        }
    } else if (lhs.type_.getName() == "int") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<int>(lhs.data_)) +
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<int>(lhs.data_) + std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {lhs.type_, std::to_string(std::get<int>(lhs.data_) + std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<int>(lhs.data_) + std::get<bool>(rhs.data_))};
        }
    } else if (lhs.type_.getName() == "char") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<char>(lhs.data_)) +
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<char>(lhs.data_) + std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {rhs.type_, std::to_string(std::get<char>(lhs.data_) + std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<char>(lhs.data_) + std::get<bool>(rhs.data_))};
        }
    } else if (lhs.type_.getName() == "bool") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<bool>(lhs.data_)) +
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.data_) + std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.data_) + std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.data_) + std::get<bool>(rhs.data_))};
        }
    }

    throw std::runtime_error("no available operator + overloaded");
}

Literal operator*(Literal& lhs, Literal& rhs) {
    if (lhs.type_.getName() == "float") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(std::get<float>(lhs.data_) * std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) *
                                              static_cast<float>(std::get<int>(rhs.data_)))};
        } else if (rhs.type_.getName() == "char") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) *
                                              static_cast<float>(std::get<char>(rhs.data_)))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) *
                                              static_cast<float>(std::get<bool>(rhs.data_)))};
        }
    } else if (lhs.type_.getName() == "int") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<int>(lhs.data_)) *
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<int>(lhs.data_) * std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {lhs.type_, std::to_string(std::get<int>(lhs.data_) * std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<int>(lhs.data_) * std::get<bool>(rhs.data_))};
        }
    } else if (lhs.type_.getName() == "char") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<char>(lhs.data_)) *
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<char>(lhs.data_) * std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {rhs.type_, std::to_string(std::get<char>(lhs.data_) * std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<char>(lhs.data_) * std::get<bool>(rhs.data_))};
        }
    } else if (lhs.type_.getName() == "bool") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<bool>(lhs.data_)) *
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.data_) * std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.data_) * std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.data_) * std::get<bool>(rhs.data_))};
        }
    }

    throw std::runtime_error("no available operator * overloaded");
}

Literal operator-(Literal& lhs, Literal& rhs) {
    if (lhs.type_.getName() == "float") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(std::get<float>(lhs.data_) - std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) -
                                              static_cast<float>(std::get<int>(rhs.data_)))};
        } else if (rhs.type_.getName() == "char") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) -
                                              static_cast<float>(std::get<char>(rhs.data_)))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) -
                                              static_cast<float>(std::get<bool>(rhs.data_)))};
        }
    } else if (lhs.type_.getName() == "int") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<int>(lhs.data_)) -
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<int>(lhs.data_) - std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {lhs.type_, std::to_string(std::get<int>(lhs.data_) - std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<int>(lhs.data_) - std::get<bool>(rhs.data_))};
        }
    } else if (lhs.type_.getName() == "char") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<char>(lhs.data_)) -
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<char>(lhs.data_) - std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {rhs.type_, std::to_string(std::get<char>(lhs.data_) - std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<char>(lhs.data_) - std::get<bool>(rhs.data_))};
        }
    } else if (lhs.type_.getName() == "bool") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<bool>(lhs.data_)) -
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.data_) - std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.data_) - std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.data_) - std::get<bool>(rhs.data_))};
        }
    }

    throw std::runtime_error("no available operator - overloaded");
}

Literal operator/(Literal& lhs, Literal& rhs) {
    if (lhs.type_.getName() == "float") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(std::get<float>(lhs.data_) / std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) /
                                              static_cast<float>(std::get<int>(rhs.data_)))};
        } else if (rhs.type_.getName() == "char") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) /
                                              static_cast<float>(std::get<char>(rhs.data_)))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) /
                                              static_cast<float>(std::get<bool>(rhs.data_)))};
        }
    } else if (lhs.type_.getName() == "int") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<int>(lhs.data_)) /
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<int>(lhs.data_) / std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {lhs.type_, std::to_string(std::get<int>(lhs.data_) / std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<int>(lhs.data_) / std::get<bool>(rhs.data_))};
        }
    } else if (lhs.type_.getName() == "char") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<char>(lhs.data_)) /
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<char>(lhs.data_) / std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {rhs.type_, std::to_string(std::get<char>(lhs.data_) / std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<char>(lhs.data_) / std::get<bool>(rhs.data_))};
        }
    } else if (lhs.type_.getName() == "bool") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<bool>(lhs.data_)) /
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.data_) / std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.data_) / std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.data_) / std::get<bool>(rhs.data_))};
        }
    }

    throw std::runtime_error("no available operator / overloaded");
}

Literal operator%(Literal& lhs, Literal& rhs) {
    if (lhs.type_.getName() == "int") {
        if (rhs.type_.getName() == "int") {
            return {Type("int", false), std::to_string(std::get<int>(lhs.data_) % std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {Type("int", false), std::to_string(std::get<int>(lhs.data_) % std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {Type("int", false), std::to_string(std::get<int>(lhs.data_) % std::get<bool>(rhs.data_))};
        }
    } else if (lhs.type_.getName() == "char") {
        if (rhs.type_.getName() == "int") {
            return {Type("int", false), std::to_string(std::get<char>(lhs.data_) % std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {Type("int", false), std::to_string(std::get<char>(lhs.data_) % std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {Type("int", false), std::to_string(std::get<char>(lhs.data_) % std::get<bool>(rhs.data_))};
        }
    } else if (lhs.type_.getName() == "bool") {
        if (rhs.type_.getName() == "int") {
            return {Type("int", false), std::to_string(std::get<bool>(lhs.data_) % std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {Type("int", false), std::to_string(std::get<bool>(lhs.data_) % std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {Type("int", false), std::to_string(std::get<bool>(lhs.data_) % std::get<bool>(rhs.data_))};
        }
    }

    throw std::runtime_error("no available operator % overloaded");
}

Literal operator&&(Literal& lhs, Literal& rhs) {
    if (lhs.type_.getName() == "float") {
        if (rhs.type_.getName() == "float") {
            return {Type("bool", false), std::to_string(std::get<float>(lhs.data_) && std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {Type("bool", false), std::to_string(std::get<float>(lhs.data_) &&
                                              static_cast<float>(std::get<int>(rhs.data_)))};
        } else if (rhs.type_.getName() == "char") {
            return {Type("bool", false), std::to_string(std::get<float>(lhs.data_) &&
                                              static_cast<float>(std::get<char>(rhs.data_)))};
        } else if (rhs.type_.getName() == "bool") {
            return {Type("bool", false), std::to_string(std::get<float>(lhs.data_) &&
                                              static_cast<float>(std::get<bool>(rhs.data_)))};
        }
    } else if (lhs.type_.getName() == "int") {
        if (rhs.type_.getName() == "float") {
            return {Type("bool", false), std::to_string(static_cast<float>(std::get<int>(lhs.data_)) &&
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {Type("bool", false), std::to_string(std::get<int>(lhs.data_) && std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {Type("bool", false), std::to_string(std::get<int>(lhs.data_) && std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {Type("bool", false), std::to_string(std::get<int>(lhs.data_) && std::get<bool>(rhs.data_))};
        }
    } else if (lhs.type_.getName() == "char") {
        if (rhs.type_.getName() == "float") {
            return {Type("bool", false), std::to_string(static_cast<float>(std::get<char>(lhs.data_)) &&
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {Type("bool", false), std::to_string(std::get<char>(lhs.data_) && std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {Type("bool", false), std::to_string(std::get<char>(lhs.data_) && std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {Type("bool", false), std::to_string(std::get<char>(lhs.data_) && std::get<bool>(rhs.data_))};
        }
    } else if (lhs.type_.getName() == "bool") {
        if (rhs.type_.getName() == "float") {
            return {Type("bool", false), std::to_string(static_cast<float>(std::get<bool>(lhs.data_)) &&
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {Type("bool", false), std::to_string(std::get<bool>(lhs.data_) && std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {Type("bool", false), std::to_string(std::get<bool>(lhs.data_) && std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {Type("bool", false), std::to_string(std::get<bool>(lhs.data_) && std::get<bool>(rhs.data_))};
        }
    }

    throw std::runtime_error("no available operator and overloaded");
}

Literal operator||(Literal& lhs, Literal& rhs) {
    if (lhs.type_.getName() == "float") {
        if (rhs.type_.getName() == "float") {
            return {Type("bool", false), std::to_string(std::get<float>(lhs.data_) || std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {Type("bool", false), std::to_string(std::get<float>(lhs.data_) ||
                                                        static_cast<float>(std::get<int>(rhs.data_)))};
        } else if (rhs.type_.getName() == "char") {
            return {Type("bool", false), std::to_string(std::get<float>(lhs.data_) ||
                                                        static_cast<float>(std::get<char>(rhs.data_)))};
        } else if (rhs.type_.getName() == "bool") {
            return {Type("bool", false), std::to_string(std::get<float>(lhs.data_) ||
                                                        static_cast<float>(std::get<bool>(rhs.data_)))};
        }
    } else if (lhs.type_.getName() == "int") {
        if (rhs.type_.getName() == "float") {
            return {Type("bool", false), std::to_string(static_cast<float>(std::get<int>(lhs.data_)) ||
                                                        std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {Type("bool", false), std::to_string(std::get<int>(lhs.data_) || std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {Type("bool", false), std::to_string(std::get<int>(lhs.data_) || std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {Type("bool", false), std::to_string(std::get<int>(lhs.data_) || std::get<bool>(rhs.data_))};
        }
    } else if (lhs.type_.getName() == "char") {
        if (rhs.type_.getName() == "float") {
            return {Type("bool", false), std::to_string(static_cast<float>(std::get<char>(lhs.data_)) ||
                                                        std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {Type("bool", false), std::to_string(std::get<char>(lhs.data_) || std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {Type("bool", false), std::to_string(std::get<char>(lhs.data_) || std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {Type("bool", false), std::to_string(std::get<char>(lhs.data_) || std::get<bool>(rhs.data_))};
        }
    } else if (lhs.type_.getName() == "bool") {
        if (rhs.type_.getName() == "float") {
            return {Type("bool", false), std::to_string(static_cast<float>(std::get<bool>(lhs.data_)) ||
                                                        std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {Type("bool", false), std::to_string(std::get<bool>(lhs.data_) || std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {Type("bool", false), std::to_string(std::get<bool>(lhs.data_) || std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {Type("bool", false), std::to_string(std::get<bool>(lhs.data_) || std::get<bool>(rhs.data_))};
        }
    }

    throw std::runtime_error("no available operator or overloaded");
}

