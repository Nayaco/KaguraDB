#ifndef PARSER_ERROR_HPP
#define PARSER_ERROR_HPP
#include "Errors/BaseLogger.hpp"

class ParserError: public BaseError {
private:
    std::string make_msg(const std::string &what_str, const int l, const int c);
public: 
    explicit ParserError(const std::string &what_str);
    explicit ParserError(const std::string &what_str, const int l, const int c);
};

#endif