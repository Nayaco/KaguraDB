#ifndef BUFFER_ERROR_HPP
#define BUFFER_ERROR_HPP
#include "Errors/BaseLogger.hpp"
class BufferError: public BaseError {
public:
    explicit BufferError(const std::string &what_str)
        : BaseError("Buffer Error: " + what_str) {}
};
#endif