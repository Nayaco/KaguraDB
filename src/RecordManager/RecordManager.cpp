#include "RecordManager/RecordManager.hpp"
#include "Errors/RuntimeError/RuntimeError.hpp"
namespace RM {

void init() {
    for (auto& schema: CM::schemas) {
        if(!hasTable(schema.first)) {
            throw RuntimeError(REType::notfound, "Record Init: table not exists");
        }
    }
}

void exit() { }

static bool satisfy(SchemaInstance schema, const Record& record, const Predicate& predicate) {
    for (auto i = 0; i < record.size(); ++i) {
        if(predicate.attrName == schema->attrs[i].name) {
            switch (predicate.op) {
                case OpType::EQ:    return record[i] == predicate.value;
                case OpType::GT:    return record[i] >  predicate.value;
                case OpType::GTEQ:  return record[i] >= predicate.value;
                case OpType::LT:    return record[i] <  predicate.value;
                case OpType::LTEQ:  return record[i] <= predicate.value;
            }
        }
    }
    throw SQLError("Record Error: no '" + predicate.attrName + "' in table '" + schema->tableName + "'");
}

bool hasTable(const string& tableName) {
    return CS::FS::exists(FSpec::genTableName(tableName).c_str());
}

void createTable(const string& tableName) {
    if (hasTable(tableName)) {
        throw RuntimeError(REType::notfound, "table exists");
    }
    CS::createBlockSet(FSpec::genTableName(tableName), FSpec::FileType::TABLE);
}

void dropTable(const string& tableName) {
    if (!hasTable(tableName)) {
        throw RuntimeError(REType::notfound, "table not exists");
    }
    CS::deleteBlockSet(FSpec::genTableName(tableName));
}

int insertRecord(const string& tableName, 
    const Record& record) {
    if(!hasTable(tableName)) 
        throw RuntimeError(REType::notfound, "table not exists");
    auto filename = FSpec::genTableName(tableName);
    int tOfs;
    int tailOfs;
    auto schema = CM::getSchema(tableName);
    int perblkNum = recordBinarySize(schema);
    // table header begin 
    FSpec::Meta::TableHeader tHeader{ };
    auto blk_0 = CS::getBlock(CS::makeUID(filename, 0));
    blk_0->read(reinterpret_cast<char*>(&tHeader), 0, sizeof(tHeader));
    tOfs = tHeader.blockNum;
    tHeader.blockNum += perblkNum;
    tHeader.recordNum += 1;
    if (tHeader.startOffset == 0) tHeader.startOffset = tOfs; 
    blk_0->write(reinterpret_cast<char*>(&tHeader), 0, sizeof(tHeader));
    // table header end
    
    // record begin
    if(record.size() != schema->attrs.size())
        throw SQLError("Record Error: field number not matches to schema");
    auto innerOffset = 0;
    auto blk = CS::setBlock(CS::makeUID(filename, tOfs));
    RecordBlockHeader rblkHeader { };
    rblkHeader.nOfs = 0;
    rblkHeader.pOfs = 0;
    blk->write(reinterpret_cast<char*>(&rblkHeader), innerOffset, 8);
    innerOffset += 8;
    for(auto i = 0; i < schema->attrs.size(); ++i) {
        if(record[i].type != schema->attrs[i].type)
            throw SQLError("Record Error: field type not matches to schema");
        if(record[i].type == ValueType::CHAR && record[i].size() > schema->attrs[i].size())
            throw SQLError("Record Error: "+ string(record[i].val()) + 
                " is too long for char(" + std::to_string(schema->attrs[i].size()) +")");
        if(innerOffset + record[i].size() > BLOCK_SIZE) {
            innerOffset = 0;
            tOfs += 1;
            blk = CS::setBlock(CS::makeUID(filename, tOfs));
        }
        blk->write(record[i].val(), innerOffset, record[i].size());
        innerOffset += schema->attrs[i].size();
    }
    // record end
}

int deleteAllRecord(const string& tableName) {
    if(!hasTable(tableName)) 
        throw RuntimeError(REType::notfound, "table not exists");
    auto filename = FSpec::genTableName(tableName);
    FSpec::Meta::TableHeader tHeader{ };
    auto blk_0 = CS::getBlock(CS::makeUID(filename, 0));
    blk_0->read(reinterpret_cast<char*>(&tHeader), 0, sizeof(tHeader));
    tHeader.blockNum = 1;
    auto delNum = tHeader.recordNum;
    tHeader.recordNum = 0;
    if (tHeader.startOffset != 0) tHeader.startOffset = 0; 
    blk_0->write(reinterpret_cast<char*>(&tHeader), 0, sizeof(tHeader));
    return delNum;
}

int deleteRecord(SchemaInstance schema, 
    const Predicates& predicates){
    if(!hasTable(schema->tableName)) 
        throw RuntimeError(REType::notfound, "table not exists");
    auto filename = FSpec::genTableName(schema->tableName);
    int tOfs;
    int recordNum;
    int delNum = 0;
    int perblkNum = recordBinarySize(schema);
    
    // table header begin 
    FSpec::Meta::TableHeader tHeader{ };
    auto blk_0 = CS::getBlock(CS::makeUID(filename, 0));
    blk_0->read(reinterpret_cast<char*>(&tHeader), 0, sizeof(tHeader));
    tOfs = tHeader.startOffset;
    recordNum = tHeader.recordNum;
    // table header end
    
    // record begin
    for(auto iOfs = 0; iOfs < recordNum;) {
        auto innerOffset = 0;
        Record record;
        
        // blk header
        auto blk_k = CS::getBlock(CS::makeUID(filename, tOfs));
        RecordBlockHeader rblkHeader { };
        blk_k->read(reinterpret_cast<char*>(&rblkHeader), innerOffset, 8);
        innerOffset += 8;
        if(rblkHeader.nOfs & DELETED_MARK) {
            tOfs += perblkNum;
            continue;
        }
        // blk header end
        auto blk = CS::getBlock(CS::makeUID(filename, tOfs));
        for(auto i = 0; i < schema->attrs.size(); ++i) {
            Value value(schema->attrs[i]);
            if(innerOffset + schema->attrs[i].size() > BLOCK_SIZE) {
                innerOffset = 0;
                tOfs += 1;
                blk = CS::getBlock(CS::makeUID(filename, tOfs));
            }
            blk->read(value.val(), innerOffset, value.size());
            innerOffset += value.size();
            record.emplace_back(std::move(value));
        }
        int rdelete = 1;
        for(auto i = 0; i < predicates.size(); ++i) {
            if(!(rdelete = satisfy(schema, record, predicates[i]))) 
                break;
        }
        if(rdelete) {
            delNum++;
            rblkHeader.nOfs |= DELETED_MARK;
            blk_k->write(reinterpret_cast<char*>(&rblkHeader), 0, 8);
        }
        tOfs += perblkNum;
        iOfs++;
    }
    // record end
    tHeader.recordNum -= delNum;
    if(tHeader.recordNum == 0) tHeader.startOffset = 0;
    blk_0->write(reinterpret_cast<char*>(&tHeader), 0, sizeof(tHeader));
    return delNum;
}

int deleteRecord(const string& tableName, 
    const vector<int>& offsets) {
    return 0;
}

Records selectRecords(SchemaInstance schema,
    const Predicates& predicates) {
    if(!hasTable(schema->tableName)) 
        throw RuntimeError(REType::notfound, "table not exists");
    auto filename = FSpec::genTableName(schema->tableName);
    int tOfs;
    int recordNum;
    int perblkNum = recordBinarySize(schema);
    Records records { };

    // table header begin 
    FSpec::Meta::TableHeader tHeader{ };
    auto blk_0 = CS::getBlock(CS::makeUID(filename, 0));
    blk_0->read(reinterpret_cast<char*>(&tHeader), 0, sizeof(tHeader));
    tOfs = tHeader.startOffset;
    recordNum = tHeader.recordNum;
    // table header end
    
    // record begin
    for(auto iOfs = 0; iOfs < recordNum;) {
        auto innerOffset = 0;
        Record record;
        
        // blk header
        auto blk_k = CS::getBlock(CS::makeUID(filename, tOfs));
        RecordBlockHeader rblkHeader { };
        blk_k->read(reinterpret_cast<char*>(&rblkHeader), innerOffset, 8);
        innerOffset += 8;
        if(rblkHeader.nOfs & DELETED_MARK) {
            tOfs += perblkNum;
            continue;
        }
        // blk header end
        auto blk = CS::getBlock(CS::makeUID(filename, tOfs));
        for(auto i = 0; i < schema->attrs.size(); ++i) {
            Value value(schema->attrs[i]);
            if(innerOffset + schema->attrs[i].size() > BLOCK_SIZE) {
                innerOffset = 0;
                tOfs += 1;
                blk = CS::getBlock(CS::makeUID(filename, tOfs));
            }
            blk->read(value.val(), innerOffset, value.size());
            innerOffset += value.size();
            record.emplace_back(std::move(value));
        }
        int rselect = 1;
        for(auto i = 0; i < predicates.size(); ++i) {
            if(!(rselect = satisfy(schema, record, predicates[i]))) 
                break;
        }
        if(rselect) {
            records.emplace_back(record);
        }
        tOfs += perblkNum;
        iOfs++;
    }
    // record end
    return records;
}
Records selectRecordsOfOffset(SchemaInstance schema, 
    const Predicates& predicates, 
    const vector<int>& offsets) {
    return Records{};
}

Records project(const Records& records, 
    SchemaInstance schema, 
    const vector<string>& fields) {
    if(fields.empty()) return records;
    Records newRecords { };
    vector<int> fieldPosition;
    for(auto& field: fields) {
        int fieldExists = 0;
        for(auto i = 0; i < schema->attrs.size(); ++i){
            if (schema->attrs[i].name == field) {
                fieldExists = 1;
                fieldPosition.push_back(i);
                
            }
        }
        if(!fieldExists) {
            throw SQLError("Projection Error: field " + field + " not exist");
        }
    }
    for(auto i = 0; i < records.size(); ++i){
        Record newRecord{ };
        for(auto j = 0; j < fieldPosition.size(); j++) {
            newRecord.emplace_back((records[i])[fieldPosition[j]]);
        }
        newRecords.emplace_back(newRecord);
    }
    return newRecords;
}

}