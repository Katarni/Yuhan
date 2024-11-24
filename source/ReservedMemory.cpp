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

ReservedMemory::ReservedMemory(Type type) {
    type_ = type;
    if (type_.getName() == "int") {
        data_ = static_cast<int>(0);
    } else if (type_.getName() == "string") {
        data_ = "";
    } else if (type_.getName() == "bool") {
        data_ = false;
    } else if (type_.getName() == "char") {
        data_ = static_cast<char>(0);
    } else if (type_.getName() == "float") {
        data_ = 0.0f;
    } else if (type_.getName() == "array") {
        data_ = std::vector<ReservedMemory*>();
        std::get<std::vector<ReservedMemory*>>(data_).resize(type_.getArraySize());
        for (auto& i : std::get<std::vector<ReservedMemory*>>(data_)) {
            i = new ReservedMemory(type_.getArrayType());
        }
    } else {
        data_ = std::map<std::string, ReservedMemory*>();
    }

    type_.setLvalue(true);
}

ReservedMemory *ReservedMemory::getFieldByName(const std::string &name) const {
    return std::get<std::map<std::string, ReservedMemory*>>(data_).find(name)->second;
}

void ReservedMemory::setFields(const std::vector<std::pair<std::string, Type>> &fields) {
    if (!std::holds_alternative<std::map<std::string, ReservedMemory*>>(data_)) {
        throw std::runtime_error("expected struct");
    }

    for (const auto& [name, type] : fields) {
        std::get<std::map<std::string, ReservedMemory*>>(data_)[name] = new ReservedMemory(type);
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
    if (type_.getName() == "char") {
        data_ = static_cast<char>(data[0]);
    } else if (type_.getName() == "int") {
        data_ = std::stoi(data);
    } else if (type_.getName() == "float") {
        data_ = std::stof(data);
    } else if (type_.getName() == "string") {
        data_ = data;
    }
}

Type Literal::getType() const {
    return type_;
}

void Literal::setType(Type other) {
    type_ = other;
}

std::variant<int, char, bool, float, std::string> Literal::getData() const {
    return data_;
}

bool ReservedMemory::isTrue() const {
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

std::ostream &operator<<(std::ostream &os, ReservedMemory *var) {
    if (var->type_.getName() == "int") {
        os << std::get<int>(var->data_);
    } else if (var->type_.getName() == "bool") {
        os << std::get<bool>(var->data_);
    } else if (var->type_.getName() == "char") {
        os << std::get<char>(var->data_);
    } else if (var->type_.getName() == "float") {
        os << std::get<float>(var->data_);
    } else if (var->type_.getName() == "string") {
        os << std::get<std::string>(var->data_);
    } else {
        throw std::runtime_error("no available print overload");
    }

    return os;
}

std::istream &operator>>(std::istream &is, ReservedMemory *&var) {
    if (var->type_.getName() == "int") {
        is >> std::get<int>(var->data_);
    } else if (var->type_.getName() == "bool") {
        is >> std::get<bool>(var->data_);
    } else if (var->type_.getName() == "char") {
        is >> std::get<char>(var->data_);
    } else if (var->type_.getName() == "float") {
        is >> std::get<float>(var->data_);
    } else if (var->type_.getName() == "string") {
        is >> std::get<std::string>(var->data_);
    } else {
        throw std::runtime_error("no available scan overload");
    }

    return is;
}

std::ostream &operator<<(std::ostream &os, Literal lit) {
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

Literal operator+(const ReservedMemory &lhs, const ReservedMemory &rhs) {
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

Literal operator+(const ReservedMemory &lhs, const Literal &rhs) {
    if (lhs.type_.getName() == "string" || rhs.getType().getName() == "string") {
        return {lhs.type_, std::get<std::string>(lhs.data_) + std::get<std::string>(rhs.getData())};
    }

    if (lhs.type_.getName() == "float") {
        if (rhs.getType().getName() == "float") {
            return {rhs.getType(), std::to_string(std::get<float>(lhs.data_) + std::get<float>(rhs.getData()))};
        } else if (rhs.getType().getName() == "int") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) +
                                              static_cast<float>(std::get<int>(rhs.getData())))};
        } else if (rhs.getType().getName() == "char") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) +
                                              static_cast<float>(std::get<char>(rhs.getData())))};
        } else if (rhs.getType().getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) +
                                              static_cast<float>(std::get<bool>(rhs.getData())))};
        }
    } else if (lhs.type_.getName() == "int") {
        if (rhs.getType().getName() == "float") {
            return {rhs.getType(), std::to_string(static_cast<float>(std::get<int>(lhs.data_)) +
                                              std::get<float>(rhs.getData()))};
        } else if (rhs.getType().getName() == "int") {
            return {rhs.getType(), std::to_string(std::get<int>(lhs.data_) + std::get<int>(rhs.getData()))};
        } else if (rhs.getType().getName() == "char") {
            return {lhs.type_, std::to_string(std::get<int>(lhs.data_) + std::get<char>(rhs.getData()))};
        } else if (rhs.getType().getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<int>(lhs.data_) + std::get<bool>(rhs.getData()))};
        }
    } else if (lhs.type_.getName() == "char") {
        if (rhs.getType().getName() == "float") {
            return {rhs.getType(), std::to_string(static_cast<float>(std::get<char>(lhs.data_)) +
                                              std::get<float>(rhs.getData()))};
        } else if (rhs.getType().getName() == "int") {
            return {rhs.getType(), std::to_string(std::get<char>(lhs.data_) + std::get<int>(rhs.getData()))};
        } else if (rhs.getType().getName() == "char") {
            return {rhs.getType(), std::to_string(std::get<char>(lhs.data_) + std::get<char>(rhs.getData()))};
        } else if (rhs.getType().getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<char>(lhs.data_) + std::get<bool>(rhs.getData()))};
        }
    } else if (lhs.type_.getName() == "bool") {
        if (rhs.getType().getName() == "float") {
            return {rhs.getType(), std::to_string(static_cast<float>(std::get<bool>(lhs.data_)) +
                                              std::get<float>(rhs.getData()))};
        } else if (rhs.getType().getName() == "int") {
            return {rhs.getType(), std::to_string(std::get<bool>(lhs.data_) + std::get<int>(rhs.getData()))};
        } else if (rhs.getType().getName() == "char") {
            return {rhs.getType(), std::to_string(std::get<bool>(lhs.data_) + std::get<char>(rhs.getData()))};
        } else if (rhs.getType().getName() == "bool") {
            return {rhs.getType(), std::to_string(std::get<bool>(lhs.data_) + std::get<bool>(rhs.getData()))};
        }
    }

    throw std::runtime_error("no available operator + overloaded");
}

Literal operator+(const Literal &lhs, const ReservedMemory &rhs) {
    if (lhs.getType().getName() == "string" || rhs.type_.getName() == "string") {
        return {lhs.getType(), std::get<std::string>(lhs.getData()) + std::get<std::string>(rhs.data_)};
    }

    if (lhs.getType().getName() == "float") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(std::get<float>(lhs.getData()) + std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {lhs.getType(), std::to_string(std::get<float>(lhs.getData()) +
                                              static_cast<float>(std::get<int>(rhs.data_)))};
        } else if (rhs.type_.getName() == "char") {
            return {lhs.getType(), std::to_string(std::get<float>(lhs.getData()) +
                                              static_cast<float>(std::get<char>(rhs.data_)))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.getType(), std::to_string(std::get<float>(lhs.getData()) +
                                              static_cast<float>(std::get<bool>(rhs.data_)))};
        }
    } else if (lhs.getType().getName() == "int") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<int>(lhs.getData())) +
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<int>(lhs.getData()) + std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {lhs.getType(), std::to_string(std::get<int>(lhs.getData()) + std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.getType(), std::to_string(std::get<int>(lhs.getData()) + std::get<bool>(rhs.data_))};
        }
    } else if (lhs.getType().getName() == "char") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<char>(lhs.getData())) +
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<char>(lhs.getData()) + std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {rhs.type_, std::to_string(std::get<char>(lhs.getData()) + std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.getType(), std::to_string(std::get<char>(lhs.getData()) + std::get<bool>(rhs.data_))};
        }
    } else if (lhs.getType().getName() == "bool") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<bool>(lhs.getData())) +
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.getData()) + std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.getData()) + std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.getData()) + std::get<bool>(rhs.data_))};
        }
    }

    throw std::runtime_error("no available operator + overloaded");
}

Literal operator+(const Literal &lhs, const Literal &rhs) {
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

Literal operator*(const ReservedMemory &lhs, const ReservedMemory &rhs) {
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

Literal operator*(const ReservedMemory &lhs, const Literal &rhs) {
    if (lhs.type_.getName() == "float") {
        if (rhs.getType().getName() == "float") {
            return {rhs.getType(), std::to_string(std::get<float>(lhs.data_) * std::get<float>(rhs.getData()))};
        } else if (rhs.getType().getName() == "int") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) *
                                              static_cast<float>(std::get<int>(rhs.getData())))};
        } else if (rhs.getType().getName() == "char") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) *
                                              static_cast<float>(std::get<char>(rhs.getData())))};
        } else if (rhs.getType().getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) *
                                              static_cast<float>(std::get<bool>(rhs.getData())))};
        }
    } else if (lhs.type_.getName() == "int") {
        if (rhs.getType().getName() == "float") {
            return {rhs.getType(), std::to_string(static_cast<float>(std::get<int>(lhs.data_)) *
                                                  std::get<float>(rhs.getData()))};
        } else if (rhs.getType().getName() == "int") {
            return {rhs.getType(), std::to_string(std::get<int>(lhs.data_) * std::get<int>(rhs.getData()))};
        } else if (rhs.getType().getName() == "char") {
            return {lhs.type_, std::to_string(std::get<int>(lhs.data_) * std::get<char>(rhs.getData()))};
        } else if (rhs.getType().getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<int>(lhs.data_) * std::get<bool>(rhs.getData()))};
        }
    } else if (lhs.type_.getName() == "char") {
        if (rhs.getType().getName() == "float") {
            return {rhs.getType(), std::to_string(static_cast<float>(std::get<char>(lhs.data_)) *
                                                  std::get<float>(rhs.getData()))};
        } else if (rhs.getType().getName() == "int") {
            return {rhs.getType(), std::to_string(std::get<char>(lhs.data_) * std::get<int>(rhs.getData()))};
        } else if (rhs.getType().getName() == "char") {
            return {rhs.getType(), std::to_string(std::get<char>(lhs.data_) * std::get<char>(rhs.getData()))};
        } else if (rhs.getType().getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<char>(lhs.data_) * std::get<bool>(rhs.getData()))};
        }
    } else if (lhs.type_.getName() == "bool") {
        if (rhs.getType().getName() == "float") {
            return {rhs.getType(), std::to_string(static_cast<float>(std::get<bool>(lhs.data_)) *
                                                  std::get<float>(rhs.getData()))};
        } else if (rhs.getType().getName() == "int") {
            return {rhs.getType(), std::to_string(std::get<bool>(lhs.data_) * std::get<int>(rhs.getData()))};
        } else if (rhs.getType().getName() == "char") {
            return {rhs.getType(), std::to_string(std::get<bool>(lhs.data_) * std::get<char>(rhs.getData()))};
        } else if (rhs.getType().getName() == "bool") {
            return {rhs.getType(), std::to_string(std::get<bool>(lhs.data_) * std::get<bool>(rhs.getData()))};
        }
    }

    throw std::runtime_error("no available operator * overloaded");
}

Literal operator*(const Literal &lhs, const ReservedMemory &rhs) {
    if (lhs.getType().getName() == "float") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(std::get<float>(lhs.getData()) * std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {lhs.getType(), std::to_string(std::get<float>(lhs.getData()) *
                                                  static_cast<float>(std::get<int>(rhs.data_)))};
        } else if (rhs.type_.getName() == "char") {
            return {lhs.getType(), std::to_string(std::get<float>(lhs.getData()) *
                                                  static_cast<float>(std::get<char>(rhs.data_)))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.getType(), std::to_string(std::get<float>(lhs.getData()) *
                                                  static_cast<float>(std::get<bool>(rhs.data_)))};
        }
    } else if (lhs.getType().getName() == "int") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<int>(lhs.getData())) *
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<int>(lhs.getData()) * std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {lhs.getType(), std::to_string(std::get<int>(lhs.getData()) * std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.getType(), std::to_string(std::get<int>(lhs.getData()) * std::get<bool>(rhs.data_))};
        }
    } else if (lhs.getType().getName() == "char") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<char>(lhs.getData())) *
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<char>(lhs.getData()) * std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {rhs.type_, std::to_string(std::get<char>(lhs.getData()) * std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.getType(), std::to_string(std::get<char>(lhs.getData()) * std::get<bool>(rhs.data_))};
        }
    } else if (lhs.getType().getName() == "bool") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<bool>(lhs.getData())) *
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.getData()) * std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.getData()) * std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.getData()) * std::get<bool>(rhs.data_))};
        }
    }

    throw std::runtime_error("no available operator * overloaded");
}

Literal operator*(const Literal &lhs, const Literal &rhs) {
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

Literal operator-(const ReservedMemory &lhs, const ReservedMemory &rhs) {
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

Literal operator-(const ReservedMemory &lhs, const Literal &rhs) {
    if (lhs.type_.getName() == "float") {
        if (rhs.getType().getName() == "float") {
            return {rhs.getType(), std::to_string(std::get<float>(lhs.data_) - std::get<float>(rhs.getData()))};
        } else if (rhs.getType().getName() == "int") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) -
                                              static_cast<float>(std::get<int>(rhs.getData())))};
        } else if (rhs.getType().getName() == "char") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) -
                                              static_cast<float>(std::get<char>(rhs.getData())))};
        } else if (rhs.getType().getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) -
                                              static_cast<float>(std::get<bool>(rhs.getData())))};
        }
    } else if (lhs.type_.getName() == "int") {
        if (rhs.getType().getName() == "float") {
            return {rhs.getType(), std::to_string(static_cast<float>(std::get<int>(lhs.data_)) -
                                                  std::get<float>(rhs.getData()))};
        } else if (rhs.getType().getName() == "int") {
            return {rhs.getType(), std::to_string(std::get<int>(lhs.data_) - std::get<int>(rhs.getData()))};
        } else if (rhs.getType().getName() == "char") {
            return {lhs.type_, std::to_string(std::get<int>(lhs.data_) - std::get<char>(rhs.getData()))};
        } else if (rhs.getType().getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<int>(lhs.data_) - std::get<bool>(rhs.getData()))};
        }
    } else if (lhs.type_.getName() == "char") {
        if (rhs.getType().getName() == "float") {
            return {rhs.getType(), std::to_string(static_cast<float>(std::get<char>(lhs.data_)) -
                                                  std::get<float>(rhs.getData()))};
        } else if (rhs.getType().getName() == "int") {
            return {rhs.getType(), std::to_string(std::get<char>(lhs.data_) - std::get<int>(rhs.getData()))};
        } else if (rhs.getType().getName() == "char") {
            return {rhs.getType(), std::to_string(std::get<char>(lhs.data_) - std::get<char>(rhs.getData()))};
        } else if (rhs.getType().getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<char>(lhs.data_) - std::get<bool>(rhs.getData()))};
        }
    } else if (lhs.type_.getName() == "bool") {
        if (rhs.getType().getName() == "float") {
            return {rhs.getType(), std::to_string(static_cast<float>(std::get<bool>(lhs.data_)) -
                                                  std::get<float>(rhs.getData()))};
        } else if (rhs.getType().getName() == "int") {
            return {rhs.getType(), std::to_string(std::get<bool>(lhs.data_) - std::get<int>(rhs.getData()))};
        } else if (rhs.getType().getName() == "char") {
            return {rhs.getType(), std::to_string(std::get<bool>(lhs.data_) - std::get<char>(rhs.getData()))};
        } else if (rhs.getType().getName() == "bool") {
            return {rhs.getType(), std::to_string(std::get<bool>(lhs.data_) - std::get<bool>(rhs.getData()))};
        }
    }

    throw std::runtime_error("no available operator - overloaded");
}

Literal operator-(const Literal &lhs, const ReservedMemory &rhs) {
    if (lhs.getType().getName() == "float") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(std::get<float>(lhs.getData()) - std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {lhs.getType(), std::to_string(std::get<float>(lhs.getData()) -
                                                  static_cast<float>(std::get<int>(rhs.data_)))};
        } else if (rhs.type_.getName() == "char") {
            return {lhs.getType(), std::to_string(std::get<float>(lhs.getData()) -
                                                  static_cast<float>(std::get<char>(rhs.data_)))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.getType(), std::to_string(std::get<float>(lhs.getData()) -
                                                  static_cast<float>(std::get<bool>(rhs.data_)))};
        }
    } else if (lhs.getType().getName() == "int") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<int>(lhs.getData())) -
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<int>(lhs.getData()) - std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {lhs.getType(), std::to_string(std::get<int>(lhs.getData()) - std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.getType(), std::to_string(std::get<int>(lhs.getData()) - std::get<bool>(rhs.data_))};
        }
    } else if (lhs.getType().getName() == "char") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<char>(lhs.getData())) -
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<char>(lhs.getData()) - std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {rhs.type_, std::to_string(std::get<char>(lhs.getData()) - std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.getType(), std::to_string(std::get<char>(lhs.getData()) - std::get<bool>(rhs.data_))};
        }
    } else if (lhs.getType().getName() == "bool") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<bool>(lhs.getData())) -
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.getData()) - std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.getData()) - std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.getData()) - std::get<bool>(rhs.data_))};
        }
    }

    throw std::runtime_error("no available operator - overloaded");
}

Literal operator-(const Literal &lhs, const Literal &rhs) {
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

Literal operator/(const ReservedMemory &lhs, const ReservedMemory &rhs) {
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

Literal operator/(const ReservedMemory &lhs, const Literal &rhs) {
    if (lhs.type_.getName() == "float") {
        if (rhs.getType().getName() == "float") {
            return {rhs.getType(), std::to_string(std::get<float>(lhs.data_) / std::get<float>(rhs.getData()))};
        } else if (rhs.getType().getName() == "int") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) /
                                              static_cast<float>(std::get<int>(rhs.getData())))};
        } else if (rhs.getType().getName() == "char") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) /
                                              static_cast<float>(std::get<char>(rhs.getData())))};
        } else if (rhs.getType().getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<float>(lhs.data_) /
                                              static_cast<float>(std::get<bool>(rhs.getData())))};
        }
    } else if (lhs.type_.getName() == "int") {
        if (rhs.getType().getName() == "float") {
            return {rhs.getType(), std::to_string(static_cast<float>(std::get<int>(lhs.data_)) /
                                                  std::get<float>(rhs.getData()))};
        } else if (rhs.getType().getName() == "int") {
            return {rhs.getType(), std::to_string(std::get<int>(lhs.data_) / std::get<int>(rhs.getData()))};
        } else if (rhs.getType().getName() == "char") {
            return {lhs.type_, std::to_string(std::get<int>(lhs.data_) / std::get<char>(rhs.getData()))};
        } else if (rhs.getType().getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<int>(lhs.data_) / std::get<bool>(rhs.getData()))};
        }
    } else if (lhs.type_.getName() == "char") {
        if (rhs.getType().getName() == "float") {
            return {rhs.getType(), std::to_string(static_cast<float>(std::get<char>(lhs.data_)) /
                                                  std::get<float>(rhs.getData()))};
        } else if (rhs.getType().getName() == "int") {
            return {rhs.getType(), std::to_string(std::get<char>(lhs.data_) / std::get<int>(rhs.getData()))};
        } else if (rhs.getType().getName() == "char") {
            return {rhs.getType(), std::to_string(std::get<char>(lhs.data_) / std::get<char>(rhs.getData()))};
        } else if (rhs.getType().getName() == "bool") {
            return {lhs.type_, std::to_string(std::get<char>(lhs.data_) / std::get<bool>(rhs.getData()))};
        }
    } else if (lhs.type_.getName() == "bool") {
        if (rhs.getType().getName() == "float") {
            return {rhs.getType(), std::to_string(static_cast<float>(std::get<bool>(lhs.data_)) /
                                                  std::get<float>(rhs.getData()))};
        } else if (rhs.getType().getName() == "int") {
            return {rhs.getType(), std::to_string(std::get<bool>(lhs.data_) / std::get<int>(rhs.getData()))};
        } else if (rhs.getType().getName() == "char") {
            return {rhs.getType(), std::to_string(std::get<bool>(lhs.data_) / std::get<char>(rhs.getData()))};
        } else if (rhs.getType().getName() == "bool") {
            return {rhs.getType(), std::to_string(std::get<bool>(lhs.data_) / std::get<bool>(rhs.getData()))};
        }
    }

    throw std::runtime_error("no available operator / overloaded");
}

Literal operator/(const Literal &lhs, const ReservedMemory &rhs) {
    if (lhs.getType().getName() == "float") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(std::get<float>(lhs.getData()) / std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {lhs.getType(), std::to_string(std::get<float>(lhs.getData()) /
                                                  static_cast<float>(std::get<int>(rhs.data_)))};
        } else if (rhs.type_.getName() == "char") {
            return {lhs.getType(), std::to_string(std::get<float>(lhs.getData()) /
                                                  static_cast<float>(std::get<char>(rhs.data_)))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.getType(), std::to_string(std::get<float>(lhs.getData()) /
                                                  static_cast<float>(std::get<bool>(rhs.data_)))};
        }
    } else if (lhs.getType().getName() == "int") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<int>(lhs.getData())) /
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<int>(lhs.getData()) / std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {lhs.getType(), std::to_string(std::get<int>(lhs.getData()) / std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.getType(), std::to_string(std::get<int>(lhs.getData()) / std::get<bool>(rhs.data_))};
        }
    } else if (lhs.getType().getName() == "char") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<char>(lhs.getData())) /
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<char>(lhs.getData()) / std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {rhs.type_, std::to_string(std::get<char>(lhs.getData()) / std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {lhs.getType(), std::to_string(std::get<char>(lhs.getData()) / std::get<bool>(rhs.data_))};
        }
    } else if (lhs.getType().getName() == "bool") {
        if (rhs.type_.getName() == "float") {
            return {rhs.type_, std::to_string(static_cast<float>(std::get<bool>(lhs.getData())) /
                                              std::get<float>(rhs.data_))};
        } else if (rhs.type_.getName() == "int") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.getData()) / std::get<int>(rhs.data_))};
        } else if (rhs.type_.getName() == "char") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.getData()) / std::get<char>(rhs.data_))};
        } else if (rhs.type_.getName() == "bool") {
            return {rhs.type_, std::to_string(std::get<bool>(lhs.getData()) / std::get<bool>(rhs.data_))};
        }
    }

    throw std::runtime_error("no available operator / overloaded");
}

Literal operator/(const Literal &lhs, const Literal &rhs) {
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


