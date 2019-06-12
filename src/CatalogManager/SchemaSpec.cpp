#include "CatalogManager/SchemaSpec.hpp"

namespace CM {

uint32_t encodeProperties(const Attribute& attr) {
    uint32_t encode = 0;
    switch (attr.type) {
    case ValueType::CHAR: {
        encode |= CHAR_T;
        encode |= LEN_MASK & attr.charLength;
        break;
    }
    case ValueType::FLOAT: {
        encode |= FLOAT_T;
        break;
    }
    case ValueType::INT: {
        encode |= INT_T;
        break;
    }
    }
    if(attr.unique)encode |= UNIQUE_T;
    return encode;
}

tuple<ValueType, size_t, bool> decodeProperties(uint32_t encoded) {
    uint32_t typeValue = encoded & TYPE_MASK;
    ValueType valueType; 
    switch(typeValue) {
    case CHAR_T: {
        valueType = ValueType::CHAR;
        break;
    } 
    case INT_T: {
        valueType = ValueType::INT;
        break;
    } 
    case FLOAT_T: {
        valueType = ValueType::FLOAT;
        break;
    } 
    }
    int charLength = encoded & LEN_MASK;
    bool unique = encoded & UNIQUE_T;
    return std::make_tuple(valueType, static_cast<size_t>(charLength), unique);
}

}