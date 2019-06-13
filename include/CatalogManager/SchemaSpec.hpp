#ifndef SCHEMA_SPEC
#define SCHEMA_SPEC
#include "Common.hpp"
#include "BaseSpec/Types.hpp"
namespace CM {
constexpr size_t NAME_LENGTH = 64;

#define TYPE_MASK 0x0F00
#define INT_T   0x0100
#define FLOAT_T 0x0200
#define CHAR_T  0x0400
#define LEN_MASK 0x00FF
#define UNIQUE_T 0x1000

uint32_t encodeProperties(const Attribute& attr);
tuple<ValueType, size_t, bool> decodeProperties(uint32_t);

namespace BlockStruct {
#define SCHEMA_BLOCK_SIZE       136
#define ATTRIBUTE_BLOCK_SIZE    68        

struct ShemaBlock {
    uint32_t nextOfs;
    uint32_t attrNum;
    char     schemaName[64];
    char     pk[64];
};

struct AttributeBlock {
    char     name[64];
    uint32_t properties;
};
}

}

#endif