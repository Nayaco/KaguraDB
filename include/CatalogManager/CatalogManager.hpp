#ifndef CM_CATALOG_MANAGER_HPP
#define CM_CATALOG_MANAGER_HPP
#include "Common.hpp"
#include "BaseSpec/Files.hpp"
#include "BaseSpec/Schema.hpp"
#include "BufferManager/BufferManager.hpp"
#include "CatalogManager/SchemaSpec.hpp" 
namespace CM {

using namespace CS;
extern unordered_map<string, SchemaInstance> schemas;
extern unordered_map<string, IndexInstance> indices;

void init();
void exit();

void createTable(const string& tableName, const string& pk, 
            const vector<Attribute> &attrs);
void dropTable(const string&);

void createIndex(const string& indexName, const string& tableName, 
            const string& key);
void dropIndex(const string& indexName);

bool checkPredicates(const string& tableName, 
            const Predicates& predicates);

bool hasSchema(const string& tableName);

bool hasIndex(const string& indexName);
bool hasIndex(const string& tableName,
                const string& attribute);

SchemaInstance getSchema(const string& tableName);

IndexInstance getIndex(const string& indexName);
IndexInstance getIndex(const string& tableName,
                const string& attribute);

}
#endif