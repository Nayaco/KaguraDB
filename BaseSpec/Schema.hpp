#ifndef SCHEMA_HPP
#define SCHEMA_HPP
#include <string>
#include <vector>
#include "BaseSpec/Types.hpp"
#include "BaseSpec/Op.hpp"
using std::string;
using std::vector;

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

using Record = std::vector<Value>;

#endif