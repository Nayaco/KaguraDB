#ifndef AST_HPP
#define AST_HPP
#include "Common.hpp"
#include "API.hpp"
namespace Syntax {

namespace AST {

class Statement {
    Statement() = default;
    ~Statement() = default;
    virtual void call() const = 0;
};

class CreateTableStatement: public Statement {
    string name;
    string pk;
    vector<Attribute> attibutes;
public:
    CreateTableStatement() = default;
    ~CreateTableStatement() = default;
    void setTableName(const string& _tableName);
    void setpk(const string& _pk);
    void addAttribute(const Attribute& _attribute);
    void call() const override;
};

class DropTableStatement: public Statement {
    string name;
public:
    DropTableStatement() = default;
    ~DropTableStatement() = default;
    void setTableName(const string& _tableName);
    void call() const override;
};

class CreateIndexStatement: public Statement {
    string name;
    string tableName;
    string attrName;
public:
    CreateIndexStatement() = default;
    ~CreateIndexStatement() = default;
    void setIndexName(const string& _indexName);
    void setTableName(const string& _tableName);
    void setAttrName(const string& _attrName);
    void call() const override;
};

class DropIndexStatement: public Statement {
    string name;
public:
    DropIndexStatement() = default;
    ~DropIndexStatement() = default;
    void setIndexName(const string& _indexName);
    void call() const override;
};

class SelectStatement: public Statement {
    string tableName;
    vector<string> attributes;
    Predicates predicates;
public:
    SelectStatement() = default;
    ~SelectStatement() = default;
    void setTableName(const string& _tableName);
    void addAttribute(const Attribute& _attribute);
    void addPredicate(const Predicate& _predicate);
    void call() const override;
};

class InsertStatement: public Statement {
    string tableName;
    Record record;
public:
    InsertStatement() = default;
    ~InsertStatement() = default;
    void setTableName(const string& _tableName);
    void addValue(const Value& _value);
    void call() const override;
};

class DeleteStatement: public Statement {
    string tableName;
    Predicates predicates;
public:
    DeleteStatement() = default;
    ~DeleteStatement() = default;
    void addPredicate(const Attribute& attribute);
    void call() const override;
};

class ExecfileStatement:public Statement {
    string filename;
public:
    ExecfileState() = default;
    ~ExecfileState() = default;
    void setFilename(const string& filename);
    void call() const override;
};

class QuitStatement:public Statement {
    string filename;
public:
    QuitStatement() = default;
    ~QuitStatement() = default;
    void setFilename(const string& filename);
    void call() const override;
};

}

}

#endif 