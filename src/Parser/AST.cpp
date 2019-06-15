#include "Parser/AST.hpp"
#include "API/API.hpp"

namespace Syntax {
namespace AST {

void CreateTableStatement::setTableName(const string& _tableName) { name = _tableName; }
void CreateTableStatement::setpk(const string& _pk) { pk = _pk; }
void CreateTableStatement::addAttribute(const Attribute& _attribute) { attributes.emplace_back(_attribute); }
void CreateTableStatement::call() const { API::createTable(name, pk, attributes); }

void DropTableStatement::setTableName(const string& _tableName) { name = _tableName; }
void DropTableStatement::call() const { API::dropTable(name); }

void SelectStatement::setTableName(const string& _tableName) { tableName = _tableName; }
void SelectStatement::addAttribute(const string& _attribute) { attributes.emplace_back(_attribute); }
void SelectStatement::addPredicate(const Predicate& _predicate) { predicates.emplace_back(_predicate); }
string SelectStatement::callT() {
    auto res = API::select(attributes, tableName, predicates);
    auto& schema = std::get<0>(res);
    auto& records = std::get<1>(res);
    auto attrNames(attributes);
    if (attrNames.empty()) {
        std::transform(schema->attrs.begin(), schema->attrs.end(),
                        std::back_inserter(attrNames),
                        [](const Attribute &attribute) -> std::string {
                            return attribute.name;
                        });
    }
    int num = attrNames.size();
    vector<size_t> widths(num, std::numeric_limits<size_t>::min());
    for (int i = 0; i < num; i++)
        widths[i] = std::max(widths[i], attrNames[i].size());
    for (auto &record : records) {
        for (int i = 0; i < num; i++) {
            widths[i] = std::max(widths[i], record[i].toString().size());
        }
    }
    auto outputEdge = [&]() {
        ss << "+";
        for (int i = 0; i < num; i++) {
            ss << std::string(widths[i] + 2, '-');
            ss << "+";
        }
        ss << std::endl;
    };
    outputEdge();
    ss << std::left;
    ss << "|";
    for (int i = 0; i < num; i++) {
        ss << " " << std::setw(widths[i]) << attrNames[i] << " |";
    }
    ss << std::endl;
    outputEdge();
    ss << std::right;
    for (auto &record : records) {
        int size = record.size();
        ss << "|";
        for (int i = 0; i < num; i++) {
            ss << " " << std::setw(widths[i]) << record[i].toString()
                        << " |";
        }
        ss << std::endl;
    }
    outputEdge();
    return ss.str();
}
void SelectStatement::call() const {
    throw ASTException::SIGSEL;
}

void InsertStatement::setTableName(const string& _tableName) { tableName = _tableName; }
void InsertStatement::addValue(const Value& _value) { record.emplace_back(_value); }
void InsertStatement::call() const {
    API::insert(tableName, record);
}

void DeleteStatement::setTableName(const string& _tableName) { tableName = _tableName; }
void DeleteStatement::addPredicate(const Predicate& _predicate) { predicates.emplace_back(_predicate); }
void DeleteStatement::call() const {
    API::deleteRecord(tableName, predicates);
}

void ExecfileStatement::setFilename(const string& _filename) {filename = _filename;}
void ExecfileStatement::call() const {
    throw ASTException::SIGEXEC;
}

string ExecfileStatement::callT() {
    std::ifstream fs(filename.c_str(), std::ios::in);
    string buf;
    while(getline(fs, buf)) {
        ss << buf << std::endl;
    }
    return ss.str();
}

void QuitStatement::call() const {
    throw ASTException::SIGQUIT;
}

void CreateIndexStatement::setIndexName(const string& _indexName) { name = _indexName; }
void CreateIndexStatement::setTableName(const string& _tableName) { tableName = _tableName; }
void CreateIndexStatement::setAttrName(const string& _attrName) { attrName = _attrName; }
void CreateIndexStatement::call() const { 
    API::createIndex(name, tableName, attrName);
}

void DropIndexStatement::setIndexName(const string& _indexName) { name = _indexName; }
void DropIndexStatement::call() const {
    API::dropIndex(name);
};

}
}