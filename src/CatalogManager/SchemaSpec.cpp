#include "CatalogManager/SchemaSpec.hpp"

namespace CM {

uint32_t encodeProperties(const Attribute& attr) {
    
    switch (attr.type) {
    case ValueType::CHAR: {
        
        break;
    }
    case ValueType::FLOAT: {
        break;
    }
    case ValueType::INT: {
        break;
    }
    }
}
tuple<ValueType, int, bool> decodeProperties(uint32_t);

}