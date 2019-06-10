#include "Parser/Token.hpp"
#include <stdexcept>

namespace Syntax {
    
Token::Token(const Keyword& keyword, const int l, const int c)
    :l(l), c(c), type(TokenType::keyword)
{value.keyval = keyword;}

Token::Token(const Symbol& symbol, const int l, const int c)
    :l(l), c(c), type(TokenType::symbol)
{value.symval = symbol;}

Token::Token(const int num, const int l, const int c)
    :l(l), c(c), type(TokenType::integer)
{value.intval = num;}

Token::Token(const float num, const int l, const int c)
    :l(l), c(c), type(TokenType::floating)
{value.floatval = num;}

Token::Token(const string& str, const TokenType type, const int l, const int c)
    :l(l), c(c), type(type)
{
    if(type == TokenType::identifier || type == TokenType::string)
        value.strval = new string(str);
    else
        throw std::logic_error("illegal TokenType");
}

TokenType Token::typ() const { return type; }

TokenValue Token::val() const { return value; }

tuple<int, int> Token::pos() const { return make_tuple(l, c); }

Token::~Token() {
    if(type == TokenType::identifier || type == TokenType::string)
        delete value.strval;
}

static const char *keywords[] = {
    "quit"   , "execfile", "and"  , "or"    , "on"    ,
    "from"   , "into"    , "where", "create", "select",
    "insert" , "delete"  , "drop" , "table" , "index" ,
    "key"    , "char"    , "int"  , "float" , "unique",
    "primary", "values"
};

static const char *symbols[] = {
    "(" , ")", ";" , ",", "=" , 
    "<>", "<", "<=", ">", ">=", 
    "*"
};

const char *mapKeywordToString(const Keyword keyword) {
    return keywords[static_cast<int>(keyword)];
}

const char *mapSymbolToString(const Symbol symbol) {
    return symbols[static_cast<int>(symbol)];
}
    
}