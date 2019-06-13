#ifndef RECORD_MANAGER_HPP
#define RECORD_MANAGER_HPP
#include "Common.hpp"
#include "RecordManager/RecordSpec.hpp"
namespace RM {
void init();
void exit();

bool hasTable(const string& tableName);
void createTable(const string& tableName);
void dropTable(const string& tableName);

int insertRecord(const string& tableName, 
                    const Record&);

int deleteAllRecord(const string& tableName);
int deleteRecord(SchemaInstance schema, 
                    const Predicates& predicates);
int deleteRecord(const string& tableName, 
                    const vector<int>& offsets);

Records selectRecords(SchemaInstance schema,
                    const Predicates& predicates);
Records selectRecordsOfOffset(SchemaInstance schema, 
                    const Predicates& predicates, 
                    const vector<int>& offsets);

Records project(const Records& records, 
                    SchemaInstance schema, 
                    const vector<string>& fields);
}
#endif