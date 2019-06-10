#ifndef TOKEN_HPP
#define TOKEN_HPP
#include <string>
#include <tuple>
namespace Syntax {

using namespace std;

enum class Keyword: int {
    QUIT            = 0,
    EXECFILE        = 1,
    AND             = 2,
    OR              = 3,
    ON              = 4,
    FROM            = 5,
    INTO            = 6,
    WHERE           = 7,
    CREATE          = 8,
    SELECT          = 9,
    INSERT          = 10,
    DELETE          = 11,
    DROP            = 12,
    TABLE           = 13,
    INDEX           = 14,
    KEY             = 15,
    CHAR            = 16,
    INT             = 17,
    FLOAT           = 18,
    UNIQUE          = 19,
    PRIMARY         = 20,
    VALUES          = 21 
};

enum class Symbol: int {
    LPAR            = 0,
    RPAR            = 1,
    SEMI            = 2,
    COMMA           = 3,
    EQ              = 4,
    NE              = 5,
    LT              = 6,
    LTEQ            = 7,
    GT              = 8,
    GTEQ            = 9,
    ASTER           = 10 
};

enum class TokenType {
    keyword,
    identifier,
    symbol,
    integer,
    floating,
    string
};
union TokenValue {
    Keyword keyval;
    string* strval;
    Symbol  symval;
    int     intval;
    float   floatval;
};
class Token {
public:
    Token() = delete;
    Token(const Keyword&, const int l, const int c);
    Token(const Symbol&, const int l, const int c);
    Token(const int, const int l, const int c);
    Token(const float, const int l, const int c);
    Token(const string&, const TokenType, const int l, const int c);
    TokenType typ() const;
    TokenValue val() const;
    tuple<int, int> pos() const;
    ~Token();
private:
    TokenType  type;
    TokenValue value;
    int l, c;
};

const char *mapKeywordToString(const Keyword);

const char *mapSymbolToString(const Symbol);

}

#endif 