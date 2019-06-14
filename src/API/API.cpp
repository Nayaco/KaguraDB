#include "API/API.hpp"
#include "CatalogManager/CatalogManager.hpp"
#include "RecordManager/RecordManager.hpp"

namespace API {
void createTable(
                const string &tableName, 
                const string &pk,
                const vector<Attribute> &attrs
) {
    CM::createTable(tableName, pk, attrs);
    RM::createTable(tableName);
}

void dropTable(const string &tableName) {
    CM::dropTable(tableName);
    RM::dropTable(tableName);
}

void createIndex(
                const string &indexName, 
                const string &tableName,
                const string &attrName
) {
    
}

void dropIndex(const string &indexName) {

}

tuple<SchemaInstance, Records> select(
                const vector<string> &attrs,
                const string& tableName,
                const Predicates& predicates
) {
    CM::checkPredicates(tableName, predicates);
    SchemaInstance schema = CM::getSchema(tableName);
    Records records = RM::project(RM::selectRecords(schema, predicates), schema, attrs);
    return std::make_tuple(schema, records);
}

void insert     (
                const string& tableName, 
                const Record& record
) {
    RM::insertRecord(tableName, record);
}

int deleteRecord  (
                const string& tableName,
                const Predicates& predicates
) {
    if (predicates.empty()) {
        return RM::deleteAllRecord(tableName);
    } else {
        CM::checkPredicates(tableName, predicates);
        SchemaInstance schema = CM::getSchema(tableName);
        return RM::deleteRecord(schema, predicates);
    }
}   

}