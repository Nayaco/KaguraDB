#include "IndexManager/IndexManager.hpp"

namespace IM {

void init () { 
    for (auto& _index: CM::indices) {
        if(!hasIndex(_index.first)) {
            throw RuntimeError(REType::notfound, "Index Init: index not exists");
        }
    }
}

void exit () { }

bool hasIndex (const string& indexName) {
    auto filename = FSpec::genIndexName(indexName);
    return CS::FS::exists(filename.c_str());
}

void createIndex (
    const string& indexName
) {
    if (hasIndex(indexName)) {
        throw RuntimeError(REType::notfound, "index exists");
    }
    CS::createBlockSet(FSpec::genIndexName(indexName), FSpec::FileType::INDEX);
}

void dropIndex (const string& indexName) {
    if(!hasIndex(indexName)) {
        throw RuntimeError(REType::notfound, "index not exists");
    }
    CS::deleteBlockSet(FSpec::genIndexName(indexName));
}

int getIndex (
    const string& tableName, 
    const string& attr,
    const Key&    key
) {
    auto indexInstance = CM::getIndex(tableName, attr);
}

void setIndex (
    const string& tableName,
    const string& attr,
    const Key&    key,
    const int     offset
) {
    auto indexInstance = CM::getIndex(tableName, attr);
}

}