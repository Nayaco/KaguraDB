#ifndef PARSER_ERROR_HPP
#define PARSER_ERROR_HPP
#include "Errors/BaseLogger.hpp"

inline string make_msg(const string &what_str, 
                                const int l, const int c) {
    std::stringstream ss;
    if (l != -1 && c != -1) {
        ss << "At: Line " << l << ", Column" << c << ":\n";
    } else {
        ss << "At the end:\n";
    }
    ss << "ParserError:" << what_str;
    return ss.str();
}

class ParserError: public BaseError {
public: 
    explicit ParserError(const string& what_str)
        : BaseError(what_str){}
    explicit ParserError(const string& what_str, const int l, const int c)
        : BaseError(make_msg(what_str, l, c)){}
};

#endif