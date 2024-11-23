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
        std::cout << std::get<int>(var->data_);
    } else if (var->type_.getName() == "bool") {
        std::cout << std::get<bool>(var->data_);
    } else if (var->type_.getName() == "char") {
        std::cout << std::get<char>(var->data_);
    } else if (var->type_.getName() == "float") {
        std::cout << std::get<float>(var->data_);
    } else if (var->type_.getName() == "string") {
        std::cout << std::get<std::string>(var->data_);
    } else {
        throw std::runtime_error("no available print overload");
    }

    return os;
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
