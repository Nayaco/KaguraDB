#ifndef BM_BLOCKSPEC_HPP
#define BM_BLOCKSPEC_HPP
namespace CS {
#include "Common.hpp"

enum class BlockState: int {
    CLEAN, DIRTY, COLD
};

}

#endif