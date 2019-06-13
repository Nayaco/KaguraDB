#ifndef BM_BLOCK_HPP
#define BM_BLOCK_HPP

#include "Common.hpp"
#include "BufferManager/FileService.hpp"
#include "BufferManager/BlockSpec.hpp"
namespace CS {


class Block {
private:
    string filename;
    size_t offset;
    BlockState state;
    char blockCache[BLOCK_SIZE];
public:
    Block(const Block&) = delete;
    Block operator=(const Block&) = delete;
    
    explicit Block(const BlockUID&);
    ~Block() = default;
    const string getFilename() const { return filename; }
    const size_t getOffset() const { return offset; }
    bool isClean() const { return state == BlockState::CLEAN; }
    bool isDirty() const { return state == BlockState::DIRTY; }
    bool isCold() const { return state == BlockState::COLD; }
    bool isDel() const { return state == BlockState::DELETED; }
    void setClean() { state = BlockState::CLEAN; }
    void setDirty() { state = BlockState::DIRTY; }
    void setCold() { state = BlockState::COLD; }
    void setDel() { state = BlockState::DELETED; }
    void read(char *_dest, size_t _offset, size_t _size);
    void write(const char* _src, size_t _offset, size_t _size);
    void sync(bool syncDisk);
    void createFile();
    void clear();
};

using BlockInstance = shared_ptr<Block>;

}
#endif