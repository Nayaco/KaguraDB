#ifndef BM_BUFFER_MANAGER_HPP
#define BM_BUFFER_MANAGER_HPP
#include "Common.hpp"
#include "BaseSpec/Files.hpp"
#include "BufferManager/FileService.hpp"
#include "BufferManager/BlockSpec.hpp"
#include "BufferManager/Block.hpp"

namespace CS {
using namespace FSpec;
using FS::FileServiceInstance;
using BufferUID = tuple<string, int>;
inline BufferUID makeUID(string setName, int offset) {
    return std::make_tuple(setName, offset);
}

class BufferManager { // LRU
    static const int CACHE_SIZE = 1024;
    int blockCnt;
    FileServiceInstance fsInstance;
    list<BlockInstance> cache;
    map<string, int> fileTable;
public:
    BufferManager(const BufferManager&) = delete;
    BufferManager& operator= (const BufferManager&) = delete;

    explicit BufferManager(const FileServiceInstance&);
    ~BufferManager();
    
    void createBlockSet(const string& setName, const FileType type);
    void deleteBlockSet(const string& setName);
    void synchronize();
    void maintain();
    BlockInstance getBlock(const BufferUID&);
};

using BufferInstance = BufferManager*;

}
#endif