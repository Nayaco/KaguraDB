#ifndef BM_BUFFER_MANAGER_HPP
#define BM_BUFFER_MANAGER_HPP
#include "Common.hpp"
#include "BufferManager/FileService.hpp"
#include "BufferManager/BlockSpec.hpp"
#include "BufferManager/Block.hpp"

namespace CS {

using FS::FileServiceInstance;
class BufferManager { // LRU
    int blockCnt = 0;
    FileServiceInstance fsInstance;
    list<BlockInstance> cache;
    map<string, int> fileTable;
    void deleteFile(const string& filename);
    bool fileExists(const string& filename);
    int createFile(const string& filename);
public:
    BufferManager(const BufferManager&) = delete;
    BufferManager& operator= (const BufferManager&) = delete;

    explicit BufferManager(const FileServiceInstance&);
    ~BufferManager();
    
    void createBlockSet(const string& setName);
    void deleteBlockSet(const string& setName);
    void synchronize();
    BlockInstance getBlock(BlockUID);
};

using BufferInstance = shared_ptr<BufferManager>;

}
#endif