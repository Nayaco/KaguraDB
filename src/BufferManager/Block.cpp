#include "BufferManager/Block.hpp"

namespace CS {

Block::Block(const BlockUID& blockUID)
    : filename(std::get<0>(blockUID)), offset(std::get<1>(blockUID)), state(BlockState::INIT) {
    memset(blockCache, 0, BLOCK_SIZE);
}

void Block::read(char* _dest, size_t _offset, size_t _size) {
    if(_size + _offset > BLOCK_SIZE) 
        throw SQLError("Block Error: out of block bound");
    memcpy(_dest, (blockCache + _offset), _size);
}

void Block::write(const char* _src, size_t _offset, size_t _size) {
    if(_size + _offset > BLOCK_SIZE) 
        throw SQLError("Block Error: out of block bound"); 
    memcpy((void*)(blockCache + _offset), (void*)_src, _size);
    setDirty();
}

void Block::sync(bool syncDisk) {
    if (!syncDisk)
        FS::readBlock(blockCache, filename.c_str(), offset);
    else
        FS::writeBlock(filename.c_str(), offset, blockCache);
    setClean();
}

void Block::createFile() {
    if (FS::exists(filename.c_str())) {
        throw SQLError("Block Error: file exists");
    }
    FS::createFile(filename.c_str());
}

void Block::clear() {
    setDirty();
    memset(blockCache, 0, BLOCK_SIZE);
} 

}