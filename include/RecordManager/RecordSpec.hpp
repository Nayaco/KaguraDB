#ifndef RECORD_SPEC_HPP
#define RECORD_SPEC_HPP
#include "Common.hpp"
#include "BaseSpec/Types.hpp"
#include "BaseSpec/Schema.hpp"
#include "CatalogManager/CatalogManager.hpp"
#include "BufferManager/BufferManager.hpp"
namespace RM {
using Records = vector<Record>;
using Predicates = vector<Predicate>;
struct RecordBlockHeader {
    uint32_t nOfs;
    uint32_t pOfs;
};

size_t recordBinarySize(const SchemaInstance);
}
#endif