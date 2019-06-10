#ifndef SCHEMA_SPEC
#define SCHEMA_SPEC
#include "Common.hpp"
#include "BaseSpec/Types.hpp"
namespace CM {
constexpr size_t NAME_LENGTH = 64;

#define INT_T   0x0100
#define FLOAT_T 0x0200
#define CHAR_T  0x0400
#define LEN_MASK 0x00FF
#define UNIQUE_T 0x1000

uint32_t encodeProperties(const Attribute& attr);
tuple<ValueType, int, bool> decodeProperties(uint32_t);

}

#endif