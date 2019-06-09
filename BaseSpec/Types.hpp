#ifndef TYPE_SPEC_HPP
#define TYPE_SPEC_HPP

#include "Errors/BaseLogger.hpp"
#include <cstring>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

constexpr size_t INT_SIZE = sizeof(int);
constexpr size_t FLOAT_SIZE = sizeof(float);
constexpr size_t SINGLE_CHAR_SIZE = sizeof(char);
constexpr size_t NUL_SIZE = 0;
enum class ValueType: int{
    INT         = 0,
    FLOAT       = 1,
    CHAR        = 2,
    NUL        = 3
};

struct Attribute {
    ValueType   type;
    size_t      charLength;
    bool        unique;
    std::string name;
    size_t size() const {
        switch (type) {
        case ValueType::INT: {
            return INT_SIZE;
        }
        case ValueType::FLOAT: {
            return FLOAT_SIZE;
        }
        case ValueType::CHAR: {
            return charLength * SINGLE_CHAR_SIZE;
        }
        case ValueType::NUL: {
            return NUL_SIZE;
        }
        }
    }
};

struct Value {
    ValueType type;
    size_t charlength;
    int intval;
    float floatval;
    char charval[256];

    Value() = default;
    Value (const Attribute &attr)
        : type(attr.type), charlength(attr.charLength) {}
    
    const char *val() const {
        switch (type) {
         case ValueType::INT: {
            return reinterpret_cast<const char*>(&intval);
        }
        case ValueType::FLOAT: {
            return reinterpret_cast<const char*>(&floatval);
        }
        case ValueType::CHAR: {
            return charval;
        }
        case ValueType::NUL: {
            return nullptr;
        }
        }
    }

    char *val(){
        switch (type) {
         case ValueType::INT: {
            return reinterpret_cast<char*>(&intval);
        }
        case ValueType::FLOAT: {
            return reinterpret_cast<char*>(&floatval);
        }
        case ValueType::CHAR: {
            return charval;
        }
        case ValueType::NUL: {
            return nullptr;
        }
        }
    }
};

#endif