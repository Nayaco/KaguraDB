#include "Errors/InterpreterError/ParserError.hpp"
std::string ParserError::make_msg(const std::string &what_str, 
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
ParserError::ParserError(const std::string& what_str)
        : BaseError(what_str){}
ParserError::ParserError(const std::string& what_str, const int l, const int c)
        : BaseError(make_msg(what_str, l, c)){}