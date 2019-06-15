#include "CatalogManager/CatalogManager.hpp"

namespace CM {
unordered_map<string, SchemaInstance> schemas;
unordered_map<string, IndexInstance> indices;
unordered_map<string, IndexInstance> f_aIndices;
unordered_map<string, int> schemaOffsets;
unordered_map<string, int> indexOffsets;
unordered_map<string, int> f_aIndexOffsets;
unordered_map<string, string> schemaToIndex;

static FSpec::Meta::CatalogHeader cHeader;
static int tailSchema;
static int tailIndex;

static string genIdxName(const string& tableName, const string& attrName) {
    return tableName + "@" + attrName;
}

void init() {
    schemas.clear();
    indices.clear();
    f_aIndices.clear();
    schemaOffsets.clear();
    indexOffsets.clear();
    f_aIndexOffsets.clear();
    schemaToIndex.clear();

    auto catalogName = FSpec::genCatalogName();
    CS::createBlockSet(catalogName, FileType::CATALOG);
    auto blk_0 = CS::getBlock(makeUID(catalogName, 0));
    blk_0->read(reinterpret_cast<char*>(&cHeader), 0, sizeof(cHeader));
    // start table
    int tOfs = cHeader.tableOffset;
    int nOfs = 0;
    BlockStruct::AttributeBlock attributeblk { };
    BlockStruct::ShemaBlock schemablk { };
    tailSchema = 0;
    while(tOfs != 0) {
        tailSchema = tOfs; // wheel change the tail  
        auto blk = CS::getBlock(makeUID(catalogName, tOfs)); // initialized
        auto schemaInstance = std::make_shared<Schema>();
        auto innerOffset = 0; // inner block offset
        blk->read(reinterpret_cast<char*>(&schemablk),
            innerOffset, SCHEMA_BLOCK_SIZE);
        innerOffset += SCHEMA_BLOCK_SIZE;
        nOfs = schemablk.nextOfs;
        if (nOfs & DELETED_MARK) {
            tOfs = static_cast<int>(nOfs & DELETED_MASK);
        }else {
            schemaInstance->tableName = string(schemablk.schemaName);
            schemaInstance->primaryKey = string(schemablk.pk);
            //get attrs begin
            for(auto i = 0; i < schemablk.attrNum; ++i) { 
                Attribute attribute;
                blk->read(reinterpret_cast<char*>(&attributeblk), 
                    innerOffset, ATTRIBUTE_BLOCK_SIZE);
                innerOffset += ATTRIBUTE_BLOCK_SIZE;
                attribute.name = string(attributeblk.name);
                std::tie(attribute.type, attribute.charLength, attribute.unique) =  
                    decodeProperties(attributeblk.properties);
                schemaInstance->attrs.push_back(attribute);
            }
            //get attrs end
            schemas[schemaInstance->tableName] = schemaInstance;
            schemaOffsets[schemaInstance->tableName] = tOfs;
            tOfs = nOfs;
        }
    }
    // end table
    // start index 
    tOfs = cHeader.indexOffset;
    nOfs = 0;
    BlockStruct::IndexBlock indexblk{ };
    while(tOfs != 0) {
        tailIndex = tOfs; // wheel change the tail  
        auto blk = CS::getBlock(makeUID(catalogName, tOfs)); // initialized
        auto indexInstance = std::make_shared<Index>();
        auto innerOffset = 0; // inner block offset
        blk->read(reinterpret_cast<char*>(&indexblk),
            innerOffset, INDEX_BLOCK_SIZE);
        innerOffset += INDEX_BLOCK_SIZE;
        nOfs = indexblk.nextOfs;
        if (nOfs & DELETED_MARK) {
            tOfs = static_cast<int>(nOfs & DELETED_MASK);
        }else {
            indexInstance->indexName = string(indexblk.indexName);
            indexInstance->tableName = string(indexblk.schemaName);
            indexInstance->attrName = string(indexblk.key);
            indices[indexInstance->indexName] = indexInstance;
            f_aIndices[genIdxName(indexblk.schemaName, indexblk.key)] = indexInstance;
            indexOffsets[indexInstance->indexName] = tOfs;
            f_aIndexOffsets[genIdxName(indexblk.schemaName, indexblk.key)] = tOfs;
            tOfs = nOfs;
        }
    }
    // end index
}
    
void exit() { }

bool hasSchema(const string& tableName) {
    return schemas.find(tableName) != schemas.end();
}

void createTable(const string& tableName, const string& pk, 
            const vector<Attribute> &attrs) {
    if (hasSchema(tableName)) {
        throw SQLError("Catalog Error: create table failed, schema exists");
    }
    vector<Attribute> _attrs(attrs);
    auto pkFlag = false;
    for(auto attr = _attrs.begin(); attr != _attrs.end(); ++attr) {
        if(attr->name == pk) {
            attr->unique = true;
            pkFlag = true;
            break;
        }
    } 
    if (!pkFlag) {
        throw SQLError("Catalog Error: create table failed, primarykey do not exist");
    }

    auto writestr = [](const char* src, char* dest, int len) { 
        memset(dest, 0, 64);
        memcpy(dest, src, len); 
    };
    auto catalogName = FSpec::genCatalogName();
    uint32_t tOfs = cHeader.blockNum;
    
    if(cHeader.tableOffset == 0) cHeader.tableOffset = tOfs;
    cHeader.blockNum++;
    auto blk_0 = CS::setBlock(makeUID(catalogName, 0), 
        reinterpret_cast<char*>(&cHeader), 0, sizeof(cHeader));
    
    if(tailSchema > 0) {
        auto tailblk = CS::getBlock(makeUID(catalogName, tailSchema));
        BlockStruct::ShemaBlock tailSchemablk;
        tailblk->read(reinterpret_cast<char*>(&tailSchemablk), 0, SCHEMA_BLOCK_SIZE);
        tailSchemablk.nextOfs |= tOfs;
        tailblk->write(reinterpret_cast<char*>(&tailSchemablk), 0, SCHEMA_BLOCK_SIZE);
    }
    tailSchema = tOfs;

    auto innerOffset = 0;
    // initial schemablock meta begin
    BlockStruct::ShemaBlock schemablk;
    schemablk.nextOfs = 0;
    schemablk.attrNum = _attrs.size();
    writestr(tableName.c_str(), schemablk.schemaName, tableName.length());
    writestr(pk.c_str(), schemablk.pk, pk.length());
    auto blk = CS::setBlock(makeUID(catalogName, tOfs),
        reinterpret_cast<char*>(&schemablk), innerOffset, SCHEMA_BLOCK_SIZE);
    innerOffset += SCHEMA_BLOCK_SIZE;
    // initial schemablock meta end
    // initial schemablock attributes begin
    for(auto attr: _attrs) {
        BlockStruct::AttributeBlock attributeblk;
        writestr(attr.name.c_str(), attributeblk.name, attr.name.length());
        attributeblk.properties = encodeProperties(attr);
        blk->write(reinterpret_cast<char*>(&attributeblk), 
            innerOffset, ATTRIBUTE_BLOCK_SIZE);
        innerOffset += ATTRIBUTE_BLOCK_SIZE;
    }
    blk->sync(true);
    // initial schemablock attributes end
    auto schemaInstance = std::make_shared<Schema>();
    schemaInstance->tableName = tableName;
    schemaInstance->primaryKey = pk;
    schemaInstance->attrs = _attrs;
    schemas[tableName] = schemaInstance;
    schemaOffsets[tableName] = tOfs;
}

void dropTable(const string& tableName) {
    if (!hasSchema(tableName)) {
        throw SQLError("Catalog Error: drop table failed, schema do not exist");
    }
    auto catalogName = FSpec::genCatalogName();
    auto tOfs = schemaOffsets[tableName];
    auto blk = CS::getBlock(makeUID(catalogName, tOfs));
    BlockStruct::ShemaBlock schemablk;
    blk->read(reinterpret_cast<char*>(&schemablk), 0, SCHEMA_BLOCK_SIZE);
    schemablk.nextOfs |= DELETED_MARK;
    blk->write(reinterpret_cast<char*>(&schemablk), 0, SCHEMA_BLOCK_SIZE);
    schemas.erase(tableName);
    schemaOffsets.erase(tableName);
    blk->sync(true);
}

SchemaInstance getSchema(const string& tableName) {
    if (!hasSchema(tableName)) {
        throw SQLError("Catalog Error: get schema failed, schema do not exist");
    }
    return schemas[tableName];
}


bool checkPredicates(const string& tableName, 
            const Predicates& predicates) {
    auto schema = getSchema(tableName);
    for(auto i = 0; i < predicates.size(); ++i) {
        for(auto j = 0; j < schema->attrs.size(); ++j) {
            if(schema->attrs[j].name == predicates[i].attrName) {
                break;
            }
            if(j == schema->attrs.size() -1 && 
                schema->attrs[j].name != predicates[i].attrName){
                return false;
            }
        }
    }
    return true;
}

bool hasIndex(const string& tableName,
                const string& attribute) {
    return f_aIndices.find(genIdxName(tableName, attribute)) != f_aIndices.end();
}

bool hasIndex(const string& indexName) {
    return indices.find(indexName) != indices.end();
}

void createIndex(const string& indexName, const string& tableName, 
            const string& key) {
    if (!hasSchema(tableName)) {
        throw SQLError("Catalog Error: create index failed, table not exists");
    }
    if (hasIndex(tableName, key)) {
        throw SQLError("Catalog Error: create index failed, index exists");
    }
    auto schemaInstance = getSchema(tableName);
    vector<Attribute> _attrs(schemaInstance->attrs);
    auto kFlag = false;
    for(auto attr = _attrs.begin(); attr != _attrs.end(); ++attr) {
        if(attr->name == key) {
            kFlag = true;
            break;
        }
    } 
    if (!kFlag) {
        throw SQLError("Catalog Error: create index failed, attribute do not exist");
    }

    auto writestr = [](const char* src, char* dest, int len) { 
        memset(dest, 0, 64);
        memcpy(dest, src, len); 
    };
    auto catalogName = FSpec::genCatalogName();
    uint32_t tOfs = cHeader.blockNum;
    
    if(cHeader.indexOffset == 0) cHeader.indexOffset = tOfs;
    cHeader.blockNum++;
    auto blk_0 = CS::setBlock(makeUID(catalogName, 0), 
        reinterpret_cast<char*>(&cHeader), 0, sizeof(cHeader));
    
    if(tailIndex > 0) {
        auto tailblk = CS::getBlock(makeUID(catalogName, tailIndex));
        BlockStruct::IndexBlock tailIndexblk;
        tailblk->read(reinterpret_cast<char*>(&tailIndexblk), 0, INDEX_BLOCK_SIZE);
        tailIndexblk.nextOfs |= tOfs;
        tailblk->write(reinterpret_cast<char*>(&tailIndexblk), 0, INDEX_BLOCK_SIZE);
    }
    tailIndex = tOfs;

    auto innerOffset = 0;
    // initial schemablock meta begin
    BlockStruct::IndexBlock indexblk;
    indexblk.nextOfs = 0;
    writestr(indexName.c_str(), indexblk.indexName, indexName.length());
    writestr(tableName.c_str(), indexblk.schemaName, tableName.length());
    writestr(key.c_str(), indexblk.key, key.length());
    auto blk = CS::setBlock(makeUID(catalogName, tOfs),
        reinterpret_cast<char*>(&indexblk), innerOffset, INDEX_BLOCK_SIZE);
    innerOffset += INDEX_BLOCK_SIZE;
    // initial schemablock meta end
    
    auto indexInstance = std::make_shared<Index>();
    indexInstance->indexName = indexName;
    indexInstance->tableName = tableName;
    indexInstance->attrName = key;
    indices[indexName] = indexInstance;
    f_aIndices[genIdxName(tableName, key)] = indexInstance;
    indexOffsets[indexName] = tOfs;
    f_aIndexOffsets[genIdxName(tableName, key)] = tOfs;
}

void dropIndex(const string& indexName) {
    if (!hasIndex(indexName)) {
        throw SQLError("Catalog Error: drop index failed, index do not exist");
    }
    auto catalogName = FSpec::genCatalogName();
    auto tOfs = indexOffsets[indexName];
    auto blk = CS::getBlock(makeUID(catalogName, tOfs));
    BlockStruct::IndexBlock indexblk;
    blk->read(reinterpret_cast<char*>(&indexblk), 0, INDEX_BLOCK_SIZE);
    indexblk.nextOfs |= DELETED_MARK;
    blk->write(reinterpret_cast<char*>(&indexblk), 0, INDEX_BLOCK_SIZE);
    auto indexInstance = getIndex(indexName);
    f_aIndices.erase(genIdxName(indexInstance->tableName, indexInstance->attrName));
    f_aIndexOffsets.erase(genIdxName(indexInstance->tableName, indexInstance->attrName));
    indices.erase(indexName);
    indexOffsets.erase(indexName);
    blk->sync(true);
}

IndexInstance getIndex(const string& indexName) {
    if(!hasIndex(indexName)) {
        throw SQLError("Catalog Error: get index failed, index do not exist");
    }
    return indices[indexName];
}
IndexInstance getIndex(const string& tableName,
                const string& attribute) {
    if(!hasIndex(tableName, attribute)) {
        throw SQLError("Catalog Error: get index failed, index do not exist");
    }
    return f_aIndices[genIdxName(tableName, attribute)];
}


}