#ifndef SQL_ERROR_HPP
#define SQL_ERROR_HPP
#include "Errors/BaseLogger.hpp"

class SQLError: public BaseError {
public: 
    explicit SQLError(const string &what_str)
        : BaseError("SQL Error: " + what_str) {}
};

#endif