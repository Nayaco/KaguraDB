#include "Parser/Parser.hpp"

namespace Syntax {
Parser::Parser(istream &is)
    : lexer(is) {
    tokens = lexer.tokenlize();
    current = tokens.begin();       
}

Statements Parser::parse() {
    Statements statements;
    while (current != tokens.end()) {
        auto &token = *current;
        if (token.typ() == TokenType::keyword) {
            auto keyword = token.val().keyval;
            if (keyword == Keyword::CREATE) {
                statements.push_back(parseCreate());
            } else if (keyword == Keyword::DROP) {
                statements.push_back(parseDrop());
            } else if (keyword == Keyword::SELECT) {
                statements.push_back(parseSelect());
            } else if (keyword == Keyword::INSERT) {
                statements.push_back(parseInsert());
            } else if (keyword == Keyword::DELETE) {
                statements.push_back(parseDelete());
            } else if (keyword == Keyword::QUIT) {
                statements.push_back(parseQuit());
            } else if (keyword == Keyword::EXECFILE) {
                statements.push_back(parseExecfile());
            } else {
                raise("unknown statement");
            }
        } else {
            raise("unknown statement");
        }
    }
    return statements;
}

bool Parser::check(const Keyword& _keyword) {
    return current != tokens.end() && 
        current->typ() == TokenType::keyword &&
        current->val().keyval == _keyword;
}
bool Parser::check(const Symbol& _symbol) {
    return current != tokens.end() && 
        current->typ() == TokenType::symbol &&
        current->val().symval == _symbol;
    
}
void Parser::expect(const Keyword& _keyword) {
    if (check(_keyword)) {
        skip(1);
    } else {
        stringstream ss;
        ss << "expect \'" 
        << mapKeywordToString(_keyword) 
        << "\' but get "
        << (current == tokens.end() ? "EOF" : current->toString()); 
        raise(ss.str());
    }
}
void Parser::expect(const Symbol& _symbol) {
    if (check(_symbol)) {
        skip(1);
    } else {
        stringstream ss;
        ss << "expect \'" 
        << mapSymbolToString(_symbol) 
        << "\' but get "
        << (current == tokens.end() ? "EOF" : current->toString()); 
        raise(ss.str());
    }
}
string Parser::getIdentifier() {
    if (current != tokens.end() &&
        current->typ() == TokenType::identifier){
        auto val = current->val().strval;
        skip(1);
        return val;
    }
    else 
        raise("expect identifier");   
}
tuple<ValueType, size_t> Parser::getType() {
    if (check(Keyword::INT)) {
        skip(1);
        return std::make_pair(ValueType::INT, 0);
    } else if (check(Keyword::FLOAT)) {
        skip(1);
        return std::make_pair(ValueType::FLOAT, 0);
    } else if (check(Keyword::CHAR)) {
        skip(1);
        expect(Symbol::LPAR);
        int size = getInt();
        if (size < 1 || size > 255) {
            --current;
            throw ParserError("size of chars should be in [1, 255]", 
                std::get<0>(current->pos()),
                std::get<1>(current->pos()));
        }
        expect(Symbol::RPAR);
        return std::make_pair(ValueType::CHAR, static_cast<size_t>(size));
    } else {
        raise("expect 'int', 'float', or 'char(..)'");
    }
}

void Parser::getSchema(shared_ptr<AST::CreateTableStatement> statement) {
    if(check(Keyword::PRIMARY)) {
        skip(1);
        expect(Keyword::KEY);
        expect(Symbol::LPAR);
        string pk = getIdentifier();
        expect(Symbol::RPAR);
        statement->setpk(pk);
    } else {
        Attribute attr;
        attr.name = getIdentifier();
        std::tie(attr.type, attr.charLength) = getType();
        skip(attr.unique = check(Keyword::UNIQUE));
        statement->addAttribute(attr);
    }
}
Predicate Parser::getPredicate() {
    Predicate predicate;
    predicate.attrName = getIdentifier();
    if (check(Symbol::EQ)) {
        predicate.op = OpType::EQ;
    } else if (check(Symbol::NE)) {
        predicate.op = OpType::NE;
    } else if (check(Symbol::LT)) {
        predicate.op = OpType::LT;
    } else if (check(Symbol::LTEQ)) {
        predicate.op = OpType::LTEQ;
    } else if (check(Symbol::GT)) {
        predicate.op = OpType::GT;
    } else if (check(Symbol::GTEQ)) {
        predicate.op = OpType::GTEQ;
    } else {
        raise(
            "expect '=', '<>', '<', '<=', '>','>='");
    }
    skip(1);
    predicate.value = getValue();
    return predicate;
}
Value Parser::getValue() {
    if (current != tokens.end()) {
        Value value;
        if (current->typ() == TokenType::integer) {
            value.type = ValueType::INT;
            value.intval = current->val().intval;
            skip(1);
            return value;
        } else if (current->typ() == TokenType::floating) {
            value.type = ValueType::FLOAT;
            value.floatval = current->val().floatval;
            skip(1);
            return value;
        } else if (current->typ() == TokenType::string) {
            auto str = current->val().strval;
            skip(1);
            if (str.length() >= 1 && str.length() <= 255) {
                value.type = ValueType::CHAR;
                value.charLength = str.length();
                std::memset(value.charval, 0, 256);
                std::strcpy(value.charval, str.c_str());
                return value;
            } else {
                raise("sring too long or too short");
            }
        }
    }
    raise("expect value of int/float/string");
}
int Parser::getInt() {
    if (current != tokens.end() &&
        current->typ() == TokenType::integer) {
        auto val = current->val().intval;
        skip(1);
        return val;
    } else 
        raise("expect integer");
}
float Parser::getFloat() {
    if (current != tokens.end() &&
        current->typ() == TokenType::floating) {
        auto val = current->val().floatval;
        skip(1);
        return val;
    }else if (current != tokens.end() &&
        current->typ() == TokenType::integer){
        auto val = current->val().intval;
        skip(1);
        return val;
    }else
        raise("expect float");
}
string Parser::getString() {
    if (current != tokens.end() &&
        current->typ() == TokenType::string) {
        auto val = current->val().strval;
        skip(1);
        return val;
    }else 
        raise("expect string");
}
void Parser::raise(const string& what_str) {
    if (current == tokens.end()) {
        throw ParserError(what_str, -1, -1);
    } else {
        int  l, c;
        std::tie(l, c) = current->pos();
        throw ParserError(what_str, l, c);
    }
}
void Parser::skip(int i) {
    current += i;
}

StatementInstance Parser::parseCreate(){
    skip(1);
    auto& token = *current;
    if(token.typ() == TokenType::keyword) {
        auto keyword  = token.val().keyval;
        if(keyword == Keyword::TABLE) {
            return parseCreateTable();
        } else if(keyword == Keyword::INDEX) {
            return parseCreateIndex();
        }
    }
    raise("parse create failed");
}
StatementInstance Parser::parseDrop(){
    skip(1);
    auto& token = *current;
    if(token.typ() == TokenType::keyword) {
        auto keyword  = token.val().keyval;
        if(keyword == Keyword::TABLE) {
            return parseDropTable();
        } else if(keyword == Keyword::INDEX) {
            return parseDropIndex();
        }
    }
    raise("parse create failed");
}
StatementInstance Parser::parseCreateTable(){
    skip(1);
    auto statement = std::make_shared<AST::CreateTableStatement>();
    statement->setTableName(getIdentifier());
    expect(Symbol::LPAR);
    do {
        if(check(Symbol::COMMA)) skip(1);
        getSchema(statement);
    }while(check(Symbol::COMMA));
    expect(Symbol::RPAR);
    expect(Symbol::SEMI);
    return statement;
}
StatementInstance Parser::parseDropTable(){
    skip(1);
    auto statement = std::make_shared<AST::DropTableStatement>();
    statement->setTableName(getIdentifier());
    expect(Symbol::SEMI);
    return statement;
}
StatementInstance Parser::parseCreateIndex(){
    skip(1);
    auto statement = std::make_shared<AST::CreateIndexStatement>();
    statement->setIndexName(getIdentifier());
    expect(Keyword::ON);
    statement->setTableName(getIdentifier());
    expect(Symbol::LPAR);
    statement->setAttrName(getIdentifier());
    expect(Symbol::RPAR);
    expect(Symbol::SEMI);
    return statement;
}
StatementInstance Parser::parseDropIndex(){
    skip(1);
    auto statement = std::make_shared<AST::DropIndexStatement>();
    statement->setIndexName(getIdentifier());
    expect(Symbol::SEMI);
    return statement;
}
StatementInstance Parser::parseSelect(){
    skip(1);
    auto statement = std::make_shared<AST::SelectStatement>();
    if (check(Symbol::ASTER)) {
        skip(1);
    } else {
        do{
            if(check(Symbol::COMMA)) skip(1);
            statement->addAttribute(getIdentifier());
        } while(check(Symbol::COMMA));
    }
    expect(Keyword::FROM);
    statement->setTableName(getIdentifier());
    
    if (check(Keyword::WHERE)) {
        skip(1);
        do {
            if(check(Keyword::AND)) skip(1);
            statement->addPredicate(getPredicate());
        } while(check(Keyword::AND));
    }
    expect(Symbol::SEMI);
    return statement;
}
StatementInstance Parser::parseInsert(){
    skip(1);
    expect(Keyword::INTO);
    auto statement = std::make_shared<AST::InsertStatement>();
    statement->setTableName(getIdentifier());
    expect(Keyword::VALUES);
    expect(Symbol::LPAR);
    do {
        if(check(Symbol::COMMA)) skip(1);
        statement->addValue(getValue());
    } while(check(Symbol::COMMA));
    expect(Symbol::RPAR);
    expect(Symbol::SEMI);
    return statement;
}
StatementInstance Parser::parseDelete(){
    skip(1);
    auto statement = std::make_shared<AST::DeleteStatement>();
    expect(Keyword::FROM);
    statement->setTableName(getIdentifier());
    if (check(Keyword::WHERE)) {
        skip(1);
        do {
            if(check(Keyword::AND)) skip(1);
            statement->addPredicate(getPredicate());
        } while(check(Keyword::AND));
    }
    expect(Symbol::SEMI);
    return statement;
}
StatementInstance Parser::parseQuit(){
    skip(1);
    auto statement = std::make_shared<AST::QuitStatement>();
    expect(Symbol::SEMI);
    return statement;
}
StatementInstance Parser::parseExecfile(){
    skip(1);
    auto statement = std::make_shared<AST::ExecfileStatement>();
    statement->setFilename(getString());
    expect(Symbol::SEMI);
    return statement;
}
    




}