#ifndef SCHEMA_HPP
#define SCHEMA_HPP
#include "Common.hpp"
#include "BaseSpec/Types.hpp"
#include "BaseSpec/Op.hpp"

struct Predicate {
    string attrName;
    OpType op;
    Value  value;
};

struct Schema {
    string tableName;
    string primaryKey;
    vector<Attribute> attrs;
};

struct Index {
    string indexName;
    string tableName;
    string attrName;
};

using Record = vector<Value>;
using SchemaInstance = shared_ptr<Schema>;
using Records = vector<Record>;
using Predicates = vector<Predicate>;

#endif