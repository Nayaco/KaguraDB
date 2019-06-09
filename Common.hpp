#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstdint>
#include <cmath>

#include <vector>
#include <string>
#include <tuple>
#include <map>
#include <list>

#include <sstream>
#include <fstream>
#include <iostream>

#include <cstdlib>
#include <cstring>
#include <memory>

#include <cassert>

#include "Errors/SQLError/SQLError.hpp"
#include "Errors/BufferError/BufferError.hpp"
#include "Errors/InterpreterError/ParserError.hpp"
#include "Errors/RuntimeError/RuntimeError.hpp"
#include "Errors/CatalogError/CatalogError.hpp"

constexpr size_t BLOCK_SIZE = 4096;
#endif