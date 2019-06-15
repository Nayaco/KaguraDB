#ifndef PARSER_HPP
#define PARSER_HPP
#include "Parser/AST.hpp"
#include "Parser/Lexer.hpp"
#include "Parser/ParseException.hpp"
#include "Parser/Token.hpp"
namespace Syntax {
using StatementInstance = shared_ptr<AST::Statement>;
using Statements = std::vector<StatementInstance>;

class Parser  {
    Lexer lexer;
    vector<Token> tokens;
    vector<Token>::iterator current;
    bool check(const Keyword& _keyword);
    bool check(const Symbol& _symbol);
    void expect(const Keyword& _keyword);
    void expect(const Symbol& _symbol);
    string getIdentifier();
    tuple<ValueType, size_t> getType();
    void getSchema(shared_ptr<AST::CreateTableStatement>);
    Predicate getPredicate();
    Value getValue();
    int getInt();
    float getFloat();
    string getString();
    void raise(const string& what_str);
    void skip(int i);
public:
    explicit Parser(istream &is);
    Statements parse();
    StatementInstance parseCreate();
    StatementInstance parseDrop();
    StatementInstance parseCreateTable();
    StatementInstance parseDropTable();
    StatementInstance parseCreateIndex();
    StatementInstance parseDropIndex();
    StatementInstance parseSelect();
    StatementInstance parseInsert();
    StatementInstance parseDelete();
    StatementInstance parseQuit();
    StatementInstance parseExecfile();
};

}

#endif 