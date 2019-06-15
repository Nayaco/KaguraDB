#ifndef REHL_HPP
#define REHL_HPP
#include "Common.hpp"
#include "Parser/Parser.hpp"
namespace App {
using namespace Syntax;
class REHL {
    REHL() = default;
    ~REHL() = default;
    string line;
    stringstream ss;
    bool onRun;
public:
    static REHL &rehl() {
        static REHL single;
        return single;
    }
    void run();
};
}
#endif