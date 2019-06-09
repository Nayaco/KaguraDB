#ifndef BASE_ERROR_HPP
#define BASE_ERROR_HPP
#include <sstream>
#include <stdexcept>
class BaseError: public std::runtime_error {
    public:
        BaseError(const std::string &what)
            :std::runtime_error("\033[31m[Error]\033[0m:" + what){};
};

class BaseWarning: public std::runtime_error {
    public:
        BaseWarning(const std::string &what)
            :std::runtime_error("\033[33m[Warning]\033[0m:" + what){};
};

class BaseInfo {
    private:
        std::string info;
    public:
        BaseInfo(const std::string &what)
            :info("\033[32m[Info]\033[0m:" + what){}
};

#endif 