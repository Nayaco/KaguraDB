#ifndef LEXER_HPP
#define LEXER_HPP
#include "Parser/Token.hpp"
#include "Parser/ParseException.hpp"
#include "Errors/InterpreterError/ParserError.hpp"
#include <string>
#include <vector>
#include <istream>
namespace Syntax {

using namespace std;

class Lexer {
private:
    Token nextToken();
    bool  nextLine();
    void  skipChars(int);
    bool  skipSpaces();
    
    istream& is;
    string  curLine;
    string::iterator p;
    int nl, nc;

public:
    explicit Lexer(istream&);
    std::vector<Token> tokenlize();
};

}

#endif 