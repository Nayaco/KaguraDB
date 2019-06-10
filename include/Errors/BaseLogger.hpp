#ifndef BASE_ERROR_HPP
#define BASE_ERROR_HPP
#include <string>
#include <sstream>
#include <stdexcept>
using std::string;

class BaseError: public std::runtime_error {
    public:
        BaseError(const string &what_str)
            :std::runtime_error("\033[31m[Error]\033[0m:" + what_str){};
};

class BaseWarning: public std::runtime_error {
    public:
        BaseWarning(const string &what_str)
            :std::runtime_error("\033[33m[Warning]\033[0m:" + what_str){};
};

class BaseInfo {
    private:
        string info;
    public:
        BaseInfo(const string &what_str)
            :info("\033[32m[Info]\033[0m:" + what_str){}
};

#endif 