#ifndef RUNTIME_ERROR_HPP
#define RUNTIME_ERROR_HPP
#include "Errors/BaseLogger.hpp"
enum class REType: unsigned int {
    overflow = 100,
    timeout = 200,
    notfound = 400,
    unknow = 500
};
class RuntimeError: public BaseError {
    public:
        explicit RuntimeError(const REType& type,const string& what)
            :BaseError(std::to_string(static_cast<unsigned int>(type)) + ":" + what) {};
};
#endif