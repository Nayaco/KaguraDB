#ifndef AST_HPP
#define AST_HPP
#include "Common.hpp"
#include "API/API.hpp"
namespace Syntax {

namespace AST {

enum class ASTException:int { SIGQUIT = 0, SIGEXEC = 1, SIGSEL = 2 };
class Statement {
public:
    Statement() = default;
    virtual ~Statement() = default;
    virtual void call() const = 0;
};

class CreateTableStatement: public Statement {
    string name;
    string pk;
    vector<Attribute> attributes;
public:
    void setTableName(const string& _tableName);
    void setpk(const string& _pk);
    void addAttribute(const Attribute& _attribute);
    void call() const override;
};

class DropTableStatement: public Statement {
    string name;
public:
    void setTableName(const string& _tableName);
    void call() const override;
};

class CreateIndexStatement: public Statement {
    string name;
    string tableName;
    string attrName;
public:
    void setIndexName(const string& _indexName);
    void setTableName(const string& _tableName);
    void setAttrName(const string& _attrName);
    void call() const override;
};

class DropIndexStatement: public Statement {
    string name;
public:
    void setIndexName(const string& _indexName);
    void call() const override;
};

class SelectStatement: public Statement {
    string tableName;
    vector<string> attributes;
    Predicates predicates;
    stringstream ss;
public:
    void setTableName(const string& _tableName);
    void addAttribute(const string& _attribute);
    void addPredicate(const Predicate& _predicate);
    void call() const override;
    string callT();
};

class InsertStatement: public Statement {
    string tableName;
    Record record;
public:
    void setTableName(const string& _tableName);
    void addValue(const Value& _value);
    void call() const override;
};

class DeleteStatement: public Statement {
    string tableName;
    Predicates predicates;
public:
    void setTableName(const string& _tableName);
    void addPredicate(const Predicate& _predicate);
    void call() const override;
};

class ExecfileStatement:public Statement {
    string filename;
    stringstream ss;
public:
    void setFilename(const string& _filename);
    void call() const override;
    string callT();
};

class QuitStatement:public Statement {
public:
    void call() const override;
};

}

}

#endif 