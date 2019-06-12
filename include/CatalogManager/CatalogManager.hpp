#ifndef CM_CATALOG_MANAGER_HPP
#define CM_CATALOG_MANAGER_HPP
#include "Common.hpp"
#include "BaseSpec/Files.hpp"
#include "BaseSpec/Schema.hpp"
#include "BufferManager/BufferManager.hpp"
#include "CatalogManager/SchemaSpec.hpp" 
namespace CM {

using SchemaInstance = shared_ptr<Schema>;
using CMAP = unordered_map<string, SchemaInstance>;
using IMAP = unordered_map<string, Index>;

using COFS = unordered_map<string, int>;
using IOFS = unordered_map<string, int>;
using CMAPI = unordered_map<string, string>;

using namespace CS;
class CatalogManager {
    CMAP schemas;
    IMAP indices;
    
    COFS schemaOffsets;
    IOFS indexOffsets;
    
    CMAPI schemaToIndex;

    BufferInstance bufferInstance;
    FSpec::Meta::CatalogHeader cHeader;
    int tailSchema;
    int tailIndex;
public:
    CatalogManager(const CatalogManager&) = delete;
    CatalogManager& operator= (const CatalogManager&) = delete;
    
    explicit CatalogManager(const BufferInstance&);
    ~CatalogManager();
    void createTable(const string& tableName, const string& pk, 
                const vector<Attribute> &attrs);
    void dropTable(const string&);

    void createIndex(const string& indexName, const string& tableName, 
                const string& key);
    void dropIndex(const string&);

    bool checkPredicates(const string& tableName, 
                const vector<Predicate>& predicates);

    bool hasSchema(const string& tableName);

    bool hasIndex(const string& tableName,
                 const string& attribute);

    SchemaInstance getSchema(const string& tableName);
    
};


}
#endif