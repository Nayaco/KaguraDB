#include "CatalogManager/CatalogManager.hpp"

namespace CM {
unordered_map<string, SchemaInstance> schemas;
unordered_map<string, Index> indices;
unordered_map<string, int> schemaOffsets;
unordered_map<string, int> indexOffsets;
unordered_map<string, string> schemaToIndex;

static FSpec::Meta::CatalogHeader cHeader;
static int tailSchema;
static int tailIndex;

void init() {
    schemas.clear();
    indices.clear();
    schemaOffsets.clear();
    indexOffsets.clear();
    schemaToIndex.clear();

    auto catalogName = FSpec::genCatalogName();
    CS::createBlockSet(catalogName, FileType::CATALOG);
    auto blk_0 = CS::getBlock(makeUID(catalogName, 0));
    blk_0->read(reinterpret_cast<char*>(&cHeader), 0, sizeof(cHeader));
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


}