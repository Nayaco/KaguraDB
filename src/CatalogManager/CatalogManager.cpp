#include "CatalogManager/CatalogManager.hpp"

namespace CM {
static unordered_map<string, int> schemaOffsets;
static unordered_map<string, int> indexOffsets;
static unordered_map<string, string> schemaToIndex;

FSpec::Meta::CatalogHeader cHeader;
int tailSchema;
int tailIndex;

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
    
    FSpec::BlockStruct::AttributeBlock* attributeblk = new FSpec::BlockStruct::AttributeBlock { };
    FSpec::BlockStruct::ShemaBlock* schemablk = new FSpec::BlockStruct::ShemaBlock { };
    while(tOfs != 0) {
        tailSchema = tOfs;
        auto blk = CS::getBlock(makeUID(catalogName, tOfs));
        auto schemaInstance = std::make_shared<Schema>();
        auto innerOffset = 0;
        blk->read(reinterpret_cast<char*>(schemablk),
            innerOffset, SCHEMA_BLOCK_SIZE);
        
        innerOffset += SCHEMA_BLOCK_SIZE;
        
        if (schemablk->nextOfs & DELETED_MARK) {
            tOfs = static_cast<int>(schemablk->nextOfs & DELETED_MASK);
            continue;
        } 
        schemaInstance->tableName = string(schemablk->schemaName);
        schemaInstance->primaryKey = string(schemablk->pk);
        for(auto i = 0; i < schemablk->attrNum; ++i) {
            Attribute attribute;
            blk->read(reinterpret_cast<char*>(attributeblk), 
                innerOffset, ATTRIBUTE_BLOCK_SIZE);
            innerOffset += ATTRIBUTE_BLOCK_SIZE;
            attribute.name = string(attributeblk->name);
            std::tie(attribute.type, attribute.charLength, attribute.unique) =  
                decodeProperties(attributeblk->properties);
            schemaInstance->attrs.push_back(attribute);
        }
        schemas[schemaInstance->tableName] = schemaInstance;
        schemaOffsets[schemaInstance->tableName] = tOfs;
        tOfs = static_cast<int>(schemablk->nextOfs);
        tOfs = 0;
    }
    delete schemablk;
    delete attributeblk;
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
    auto flag = false;
    for(auto attr = _attrs.begin(); attr != _attrs.end(); ++attr) {
        if(attr->name == pk) {
            attr->unique = true;
            flag = true;
            break;
        }
    } 
    if (!flag) {
        throw SQLError("Catalog Error: create table failed, primarykey do not exist");
    }

    auto writestr = [](const char* src, char* dest) { memcpy(dest, src, 64); };
    auto catalogName = FSpec::genCatalogName();
    uint32_t tOfs = cHeader.blockNum;
    auto tailblk = CS::getBlock(makeUID(catalogName, tailSchema));
    auto blk = CS::getBlock(makeUID(catalogName, tOfs));
    
    FSpec::BlockStruct::ShemaBlock tailSchemablk;
    FSpec::BlockStruct::ShemaBlock schemablk;
    tailblk->read(reinterpret_cast<char*>(&tailSchemablk), 0, SCHEMA_BLOCK_SIZE);
    tailSchemablk.nextOfs |= tOfs;
    schemablk.nextOfs = 0;
    schemablk.attrNum = _attrs.size();
    writestr(tableName.c_str(), schemablk.schemaName);
    writestr(pk.c_str(), schemablk.pk);
    auto innerOffset = 0;
    blk->write(reinterpret_cast<char*>(&schemablk), innerOffset, SCHEMA_BLOCK_SIZE);
    innerOffset += SCHEMA_BLOCK_SIZE;
    for(auto attr: _attrs) {
        FSpec::BlockStruct::AttributeBlock attributeblk;
        writestr(attr.name.c_str(), attributeblk.name);
        attributeblk.properties = encodeProperties(attr);
        blk->write(reinterpret_cast<char*>(&attributeblk), 
            innerOffset, ATTRIBUTE_BLOCK_SIZE);
        innerOffset += ATTRIBUTE_BLOCK_SIZE;
    }
    
    auto schemaInstance = std::make_shared<Schema>();
    schemaInstance->tableName = tableName;
    schemaInstance->primaryKey = pk;
    schemaInstance->attrs = _attrs;
    schemas[tableName] = schemaInstance;
    schemaOffsets[tableName] = tOfs;

    tailblk->write(reinterpret_cast<char*>(&tailSchemablk), 0, SCHEMA_BLOCK_SIZE);
    
    auto blk_0 = CS::getBlock(makeUID(catalogName, 0));
    cHeader.blockNum += 1;
    if(cHeader.tableOffset == 0) cHeader.tableOffset = tOfs;
    blk_0->write(reinterpret_cast<char*>(&cHeader), 0, sizeof(cHeader));
}

void dropTable(const string& tableName) {
    if (hasSchema(tableName)) {
        throw SQLError("Catalog Error: create table failed, schema exists");
    }
    auto catalogName = FSpec::genCatalogName();
    auto tOfs = schemaOffsets[tableName];
    auto blk = CS::getBlock(makeUID(catalogName, tOfs));
    FSpec::BlockStruct::ShemaBlock schemablk;
    blk->read(reinterpret_cast<char*>(&schemablk), 0, SCHEMA_BLOCK_SIZE);
    schemablk.nextOfs |= DELETED_MARK;
    blk->write(reinterpret_cast<char*>(&schemablk), 0, SCHEMA_BLOCK_SIZE);
    schemas.erase(tableName);
    schemaOffsets.erase(tableName);
}

}