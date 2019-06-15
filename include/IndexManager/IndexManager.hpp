#ifndef INDEX_MANAGER_HPP
#define INDEX_MANAGER_HPP
#include "Common.hpp"
#include "BufferManager/BufferManager.hpp"
#include "CatalogManager/CatalogManager.hpp"
#include "BaseSpec/Types.hpp"
#include "BaseSpec/Files.hpp"
#include "IndexManager/IndexSpec.hpp"
#include "IndexManager/BPlusIndex.hpp"

namespace IM {

void init ();

void exit ();

bool hasIndex (const string& indexName);

void createIndex (
                const string& indexName
                );

void dropIndex (const string& indexName);

int getIndex (
                const string& tableName, 
                const string& attr,
                const Key&    key
                );

void setIndex (
                const string& tableName,
                const string& attr,
                const Key&    key,
                const int     offset
                );

}

#endif