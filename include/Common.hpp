#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstdint>
#include <cmath>

#include <vector>
#include <string>
#include <tuple>
#include <map>
#include <unordered_map>
#include <list>

#include <sstream>
#include <fstream>
#include <iostream>

#include <sys/stat.h>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <chrono>

#include <cassert>

#include "Errors/SQLError/SQLError.hpp"
#include "Errors/BufferError/BufferError.hpp"
#include "Errors/InterpreterError/ParserError.hpp"
#include "Errors/RuntimeError/RuntimeError.hpp"
#include "Errors/CatalogError/CatalogError.hpp"

using std::string;
using std::map;
using std::unordered_map;
using std::list;
using std::vector;
using std::shared_ptr;
using std::tuple;
using std::pair;
using std::stringstream;
using Clock = std::chrono::system_clock;

constexpr size_t BLOCK_SIZE = 4096;
#endif