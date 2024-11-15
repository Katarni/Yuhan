#include "../include/ReservedMemory.h"

ReservedMemory::ReservedMemory(Type type) {
    if (type.getName() == "int") {
        data_ = static_cast<int>(0);
    } else if (type.getName() == "string") {
        data_ = "";
    } else if (type.getName() == "bool") {
        data_ = false;
    } else if (type.getName() == "char") {
        data_ = static_cast<char>(0);
    } else if (type.getName() == "float") {
        data_ = 0.0f;
    } else if (type.getName() == "array") {
        data_ = std::vector<ReservedMemory*>();
        std::get<std::vector<ReservedMemory*>>(data_).resize(type.getArraySize());
        for (auto& i : std::get<std::vector<ReservedMemory*>>(data_)) {
            i = new ReservedMemory(type.getArrayType());
        }
    } else {

    }
}
