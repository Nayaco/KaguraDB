#ifndef API_HPP
#define API_HPP
#include "Common.hpp"
#include "BaseSpec/Types.hpp"
#include "BaseSpec/Schema.hpp"
namespace API {
void createTable(
                const string &tableName, 
                const string &pk,
                const vector<Attribute> &attrs
                );

void dropTable(const string &tableName);

void createIndex(
                const string &indexName, 
                const string &tableName,
                const string &attrName
                );

void dropIndex(const string &indexName);

tuple<SchemaInstance, Records> select(
                const vector<string> &attributes,
                const string& tableName,
                const Predicates& predicates
                );

void insert     (
                const string& tableName, 
                const Record& record
                );

int deleteRecord  (
                const string& tableName,
                const Predicates& predicates
                );         
}

#endif