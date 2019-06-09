#include "Parser/Lexer.hpp"
#include <istream>
#include <algorithm>
namespace Syntax {

Lexer::Lexer(istream& is)
    :is(is), nl(0), nc(0)
{ nextLine(); }

bool Lexer::nextLine() {
    if (!getline(is, curLine)) return false;
    curLine += "\n";
    nl++;
    nc = 1;
    p = curLine.begin();
    return true;
}

void Lexer::skipChars(int n) {
    while (n--) {
        ++nc; ++p;
    }
}

bool Lexer::skipSpaces() {
    while (1) {
        if (p == curLine.end() ) {
            if (!nextLine()) return false;
        } else if (isspace(*p)) {
            skipChars(1);
        } else {
            break;
        }
    }
    return true;
}

Token Lexer::nextToken() {
    if (p == curLine.end() || !skipSpaces()) {
        throw ParseException::eof;
    }
    int cnl = nl, cnc = nc;
    if (*p == '(') { 
        skipChars(1);
        return Token(Symbol::LPAR, cnl, cnc);
    } else if (*p == ')') {
        skipChars(1);
        return Token(Symbol::RPAR, cnl, cnc);
    } else if (*p == ';') {
        skipChars(1);
        return Token(Symbol::SEMI, cnl, cnc);
    } else if (*p == ',') {
        skipChars(1);
        return Token(Symbol::COMMA, cnl, cnc);
    } else if (*p == '*') {
        skipChars(1);
        return Token(Symbol::ASTER, cnl, cnc);
    } else if (*p == '<') {
        skipChars(1);
        if(p!= curLine.end()) {
            if (*p == '=') {
                skipChars(1);
                return Token(Symbol::LTEQ, cnl, cnc);
            } else if(*p  == '>') {
                skipChars(1);
                return Token(Symbol::NE, cnl, cnc);
            }
        }
        return Token(Symbol::LT, cnl, cnc);
    } else if (*p == '>') {
        skipChars(1);
        if(p!= curLine.end()) {
            if (*p == '=') {
                skipChars(1);
                return Token(Symbol::GTEQ, cnl, cnc);
            }
        }
        return Token(Symbol::GT, cnl, cnc);
    } else if (isalpha(*p) || *p == '_'){
        int tokenLen = 1;
        auto q = p + 1;
        while (q != curLine.end()) {
            if (isalnum(*q) || *q == '_') {
                tokenLen++;
                q++;
            } else {
                break;
            }
        }
        const string str(p, q);
        string str_temp = str;
        for_each(str_temp.begin(), str_temp.end(), [](char& c) {
            c = isalpha(c) ? ::tolower(c) : c;
        });
        skipChars(tokenLen);
        if(tokenLen >= 64) {
            throw ParserError(
                "identifier more than 64 characters", 
                cnl, cnc);
        }
        if (str_temp == "quit") {
            return Token(Keyword::QUIT, cnl, cnc);
        } else if (str_temp == "execfile") {
            return Token(Keyword::EXECFILE, cnl, cnc);
        } else if (str_temp == "and") {
            return Token(Keyword::AND, cnl, cnc);
        } else if (str_temp == "or") {
            return Token(Keyword::OR, cnl, cnc);
        } else if (str_temp == "on") {
            return Token(Keyword::ON, cnl, cnc);
        } else if (str_temp == "from") {
            return Token(Keyword::FROM, cnl, cnc);
        } else if (str_temp == "into") {
            return Token(Keyword::INTO, cnl, cnc);
        } else if (str_temp == "where") {
            return Token(Keyword::WHERE, cnl, cnc);
        } else if (str_temp == "create") {
            return Token(Keyword::CREATE, cnl, cnc);
        } else if (str_temp == "select") {
            return Token(Keyword::SELECT, cnl, cnc);
        } else if (str_temp == "insert") {
            return Token(Keyword::INSERT, cnl, cnc);
        } else if (str_temp == "delete") {
            return Token(Keyword::DELETE, cnl, cnc);
        } else if (str_temp == "drop") {
            return Token(Keyword::DROP, cnl, cnc);
        } else if (str_temp == "table") {
            return Token(Keyword::TABLE, cnl, cnc);
        } else if (str_temp == "index") {
            return Token(Keyword::INDEX, cnl, cnc);
        } else if (str_temp == "key") {
            return Token(Keyword::KEY, cnl, cnc);
        } else if (str_temp == "char") {
            return Token(Keyword::CHAR, cnl, cnc);
        } else if (str_temp == "int") {
            return Token(Keyword::INT, cnl, cnc);
        } else if (str_temp == "float") {
            return Token(Keyword::FLOAT, cnl, cnc);
        } else if (str_temp == "unique") {
            return Token(Keyword::UNIQUE, cnl, cnc);
        } else if (str_temp == "values") {
            return Token(Keyword::VALUES, cnl, cnc);
        } else {
            return Token(str, TokenType::identifier, cnl, cnc);
        }
    } else if (isdigit(*p) || *p == '-') {
        int tokenLen = 1;
        auto q = p + 1;
        auto getNum = [&](){
            while (q != curLine.end()) {
                if (isdigit(*q)) {
                    tokenLen++;
                    q++;
                } else {
                    break;
                }
            }
        };
        getNum();
        if (q != curLine.end() && *q == '.') {
            getNum();
            skipChars(tokenLen);
            return Token(stof(string(p, q)), cnl, cnc);
        }
        skipChars(tokenLen);
        return Token(stoi(string(p, q)), cnl, cnc);
    } else if (*p == '\'' || *p == '\"') {
        int tokenLen = 1;
        auto q = p + 1;
        while (q != curLine.end()) {
            if ((*p == '\'' && *q != '\'') || (*p == '\"' && *q != '\"')) {
                tokenLen++;
                q++;
            } else {
                break;
            }
        }
        if ((*p == '\'' && *q != '\'') || (*p == '\"' && *q != '\"')) {
            throw ParserError("String type syntax error", nl, nc + tokenLen);
        }
        q++;
        tokenLen++;
        string str(p + 1, q - 1);
        skipChars(tokenLen);
        return Token(str, TokenType::string, cnl, cnc);
    } else {
        throw ParserError("Unknow token error", nl, nc);
    }
}

vector<Token> Lexer::tokenlize() {
    vector<Token> tokenList {};
    try {
        while(1) {
            tokenList.push_back(nextToken());
        } 
    } catch (ParseException &e) {
        if(e == ParseException::eof) {
            return tokenList;
        } else {
            throw ParserError("Unknow interruption", -1, -1);
        }
    }
}

}