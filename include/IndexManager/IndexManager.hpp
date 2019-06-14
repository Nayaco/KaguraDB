#ifndef INDEX_MANAGER_HPP
#define INDEX_MANAGER_HPP
#include "Common.hpp"
#include "BaseSpec/Types.hpp"
#include "BaseSpec/Files.hpp"

namespace IM {

void init();

void exit();

void hasIndex(const string& indexName);

void createIndex(
                const string& indexName,
                const string& tableName,
                const string& attr
                );

void dropIndex(const string& indexName);

int getIndex(const string& attr);

}

#endif