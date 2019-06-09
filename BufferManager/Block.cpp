#include "BufferManager/Block.hpp"

namespace CS {

void Block::read(const char* dest) {
    memcpy((void*)dest, (void*)blockCache, BLOCK_SIZE);
}

void Block::write(const char* src) {
    memcpy((void*)blockCache, (void*)src, BLOCK_SIZE);
}

void Block::sync() {
    if (isDirty()) {
        fsInstance->writeBlock(fid, offset, blockCache);
    }
}

}