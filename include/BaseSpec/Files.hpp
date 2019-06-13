#ifndef FILE_SPEC_HPP
#define FILE_SPEC_HPP
#include "Common.hpp"
namespace FSpec {

enum class FileType: uint32_t {
    INDEX       = 0x1F7F,
    CATALOG     = 0x3C7F,
    TABLE       = 0x3C6A
};

#define DELETED_MARK  0x80000000U
#define DELETED_MASK  0x7fffffffU

string genCatalogName();
string genTableName(const string& tablename);
string genIndexName(const string& tablename, const string& pk);

namespace Meta {
struct CatalogHeader {
    uint32_t filetype;
    uint32_t blockNum;
    uint32_t startOffset;
    uint32_t blockOffset;
    uint32_t tableOffset;
    uint32_t indexOffset;
};

struct TableHeader {
    uint32_t filetype;
    uint32_t blockNum;
    uint32_t recordNum;
    uint32_t startOffset;
    uint32_t blockOffset;
};

struct IndexHeader {
    uint32_t filetype;
    uint32_t blockNum;
    uint32_t rootOffset;
    uint32_t blockOffset;
};
}

}

#endif