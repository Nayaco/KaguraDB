#ifndef BM_BLOCK_HPP
#define BM_BLOCK_HPP

#include "Common.hpp"
#include "BufferManager/FileService.hpp"
#include "BufferManager/BlockSpec.hpp"
namespace CS {

using std::tuple;
using std::shared_ptr;
using FS::FileServiceInstance;
using BlockUID = tuple<int, size_t>; // <fid, offset>
inline BlockUID makeUID(const int fid, const size_t offset) {
    if(fid < 0 || offset < 0)
        throw BufferError("Block Error: Illegal fid or offset");
    return std::make_tuple(fid, offset);
}

class Block {
private:
    int fid;
    size_t offset;
    BlockState state;
    FileServiceInstance fsInstance;
    char blockCache[BLOCK_SIZE];
public:
    Block(const Block&) = delete;
    Block operator=(const Block&) = delete;
    explicit Block(const BlockUID&, const FileServiceInstance&);
    const int getFid() const { return fid; }
    const size_t getOffset() const { return offset; }
    bool isClean() const { return state == BlockState::CLEAN; }
    bool isDirty() const { return state == BlockState::DIRTY; }
    bool isCold() const { return state == BlockState::COLD; }
    void setClean() { state = BlockState::CLEAN; }
    void setDirty() { state = BlockState::DIRTY; }
    void setCold() { state = BlockState::COLD; }
    void read(const char* dest);
    void write(const char* src);
    void sync();
};

using BlockInstance = shared_ptr<Block>;

}
#endif