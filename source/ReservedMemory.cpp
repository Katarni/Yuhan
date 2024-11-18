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
    is_lvalue_ = other.is_lvalue_;
}

Type Type::operator=(const Type &other) {
    name_ = other.name_;
    size_array_ = other.size_array_;
    if (other.array_type_ != nullptr) {
        array_type_ = new Type(*other.array_type_);
    } else {
        array_type_ = nullptr;
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

    }
}
