#ifndef BM_BLOCKSPEC_HPP
#define BM_BLOCKSPEC_HPP
namespace CS {
#include "Common.hpp"

enum class BlockState: int {
    CLEAN, DIRTY, COLD, INIT, DELETED
};

using BlockUID = tuple<string, size_t>; // <fid, offset>

inline BlockUID makeUID(const string& filename, const size_t offset) {
    if(filename.length() == 0 || offset < 0)
        throw BufferError("Block Error: Illegal filename or offset");
    return std::make_tuple(filename, offset);
}

}
#endif