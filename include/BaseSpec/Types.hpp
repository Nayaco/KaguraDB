#ifndef TYPE_SPEC_HPP
#define TYPE_SPEC_HPP

#include "Common.hpp"

constexpr size_t INT_SIZE = sizeof(int);
constexpr size_t FLOAT_SIZE = sizeof(float);
constexpr size_t SINGLE_CHAR_SIZE = sizeof(char);
constexpr size_t NUL_SIZE = 0;
enum class ValueType: int{
    INT         = 1,
    FLOAT       = 2,
    CHAR        = 4,
    NUL         = 0
};

struct Attribute {
    ValueType   type;
    size_t      charLength;
    bool        unique;
    string name;
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
        return NUL_SIZE;
    }
    string toString() {
        std::stringstream ss;
        ss<<name<<" ";
        switch (type) {
        case ValueType::INT: {
            ss<<"INT"<<" ";
            break;
        }
        case ValueType::FLOAT: {
            ss<<"FLOAT"<<" ";
            break;
        }
        case ValueType::CHAR: {
            ss<<"CHAR("<<charLength<<")"<<" ";
            break;
        }
        case ValueType::NUL: {
            ss<<"NULL";
            break;
        }
        }
        ss<<(unique?"UNIQUE":"DUNPLICATABLE")<<"\n";
        return ss.str();
    }
};

struct Value {
    ValueType type;
    size_t charLength;
    union {
        int intval;
        float floatval;
        char charval[256];
    };
    
    Value() = default;
    Value (const Attribute &attr)
        : type(attr.type), charLength(attr.charLength) {}
    
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
        return nullptr;
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
        return nullptr;
    }

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
        return NUL_SIZE;
    }

    string toString() const {
        std::stringstream ss;
        switch (type) {
        case ValueType::INT: {
            ss<<"I"<<std::to_string(intval);
            break;
        }
        case ValueType::FLOAT: {
            ss<<"F"<<std::to_string(floatval);
            break;
        }
        case ValueType::CHAR: {
            ss<<"C("<<charLength<<")'"<<string(charval)<<'\'';
            break;
        }
        case ValueType::NUL: {
            ss<<"NULL";
            break;
        }
        }
        return ss.str();
    }
    
    bool operator ==(const Value& rhs) const {
        if (type == ValueType::NUL || rhs.type == ValueType::NUL)
            throw SQLError("TypeError: Nullvalue comparison is not allowed");
        if (type != rhs.type) {
            throw SQLError("TypeError: Different types at sides of equal operator");
        }
        switch (type) {
        case ValueType::INT: {
            return intval == rhs.intval;
        }
        case ValueType::FLOAT: {
            return floatval == rhs.floatval;
        }
        case ValueType::CHAR: {
            return strcmp(charval, rhs.charval) == 0;
        }
        case ValueType::NUL: {
            return false;
        }
        }
        return false;
    }

    bool operator !=(const Value& rhs) const {
        if (type == ValueType::NUL || rhs.type == ValueType::NUL)
            throw SQLError("TypeError: Nullvalue comparison is not allowed");

        if (type != rhs.type) {
            throw SQLError("TypeError: Different types at sides of unequal operator");
        }
        return !(*this == rhs);
    }

    bool operator <(const Value& rhs) const {
        if (type == ValueType::NUL || rhs.type == ValueType::NUL)
            throw SQLError("TypeError: Nullvalue comparison is not allowed");

        if (type != rhs.type) {
            throw SQLError("TypeError: Different types at sides of less operator");
        }
        switch (type) {
        case ValueType::INT: {
            return intval < rhs.intval;
        }
        case ValueType::FLOAT: {
            return floatval < rhs.floatval;
        }
        case ValueType::CHAR: {
            return strcmp(charval, rhs.charval) < 0;
        }
        case ValueType::NUL: {
            return false;
        }
        }
        return false;
    }

    bool operator >(const Value& rhs) const {
        if (type == ValueType::NUL || rhs.type == ValueType::NUL)
            throw SQLError("TypeError: Nullvalue comparison is not allowed");

        if (type != rhs.type) {
            throw SQLError("TypeError: Different types at sides of greater operator");
        }
        switch (type) {
        case ValueType::INT: {
            return intval > rhs.intval;
        }
        case ValueType::FLOAT: {
            return floatval > rhs.floatval;
        }
        case ValueType::CHAR: {
            return strcmp(charval, rhs.charval) > 0;
        }
        case ValueType::NUL: {
            return false;
        }
        }
        return false;
    }

    bool operator <=(const Value& rhs) const {
        if (type == ValueType::NUL || rhs.type == ValueType::NUL)
            throw SQLError("TypeError: Nullvalue comparison is not allowed");

        if (type != rhs.type) {
            throw SQLError("TypeError: Different types at sides of less or equal operator");
        }
        return !(*this > rhs);
    }

    bool operator >=(const Value& rhs) const {
        if (type == ValueType::NUL || rhs.type == ValueType::NUL)
            throw SQLError("TypeError: Nullvalue comparison is not allowed");

        if (type != rhs.type) {
            throw SQLError("TypeError: Different types at sides of greater or equal operator");
        }
        return !(*this < rhs);
    }

};

#endif