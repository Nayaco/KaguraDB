#include "BufferManager/Block.hpp"

namespace CS {

Block::Block(const BlockUID& blockUID, const FileServiceInstance& fsInstance)
    : fid(std::get<0>(blockUID)), offset(std::get<1>(blockUID)), 
    state(BlockState::INIT), fsInstance(fsInstance) {
    memset(blockCache, 0, BLOCK_SIZE);
}

void Block::read(char* dest, size_t offset, size_t size) {
    if(size + offset > BLOCK_SIZE) 
        throw SQLError("Block Error: out of block bound");
    memcpy((void*)dest, (void*)(blockCache + offset), size);
}

void Block::write(const char* src, size_t offset, size_t size) {
    if(size + offset > BLOCK_SIZE) 
        throw SQLError("Block Error: out of block bound"); 
    memcpy((void*)(blockCache + offset), (void*)src, size);
    setDirty();
}

void Block::sync() {
    if (isDirty()) {
        fsInstance->writeBlock(fid, offset, blockCache);
        setClean();
    }
}

void Block::clear() {
    setDirty();
    memset(blockCache, 0, BLOCK_SIZE);
} 

}