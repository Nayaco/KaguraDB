#include "BufferManager/BufferManager.hpp"
namespace CS {

static int blockCnt;
static list<BlockInstance> cache;
static map<string, int> fileTable;
static char zeroBuffer[65536] = { 0 };

void init() { 
    blockCnt = 0;
    cache.clear(); 
    fileTable.clear();
}

void exit() {
    for(auto iter = cache.begin(); iter != cache.end(); ++iter) {
        if((*iter)->isDirty()) {
            (*iter)->sync(true);
        }
    }
}

void createBlockSet(const string& setName, const FileType type){
    maintain();
    auto blkInstance = std::make_shared<Block>(makeUID(setName, 0));
    if(FS::exists(setName.c_str())) {
        blkInstance->sync(false);
        cache.emplace_back(blkInstance);
        blockCnt++;
        return;
    }
    FS::createFile(setName.c_str());
    switch (type)
    {
    case FileType::CATALOG: {
        Meta::CatalogHeader header;
        header.filetype = static_cast<uint32_t>(type);
        header.blockNum = 1;
        header.blockOffset = BLOCK_SIZE;
        header.indexOffset = 0;
        header.tableOffset = 0;
        header.startOffset = 0;
        blkInstance->write(reinterpret_cast<const char*>(&header), 0 , sizeof(header));
        break;
    }
    case FileType::TABLE: {
        Meta::TableHeader header;
        header.filetype = static_cast<uint32_t>(type);
        header.blockNum = 1;
        header.startOffset = 0;
        header.blockOffset = BLOCK_SIZE;
        header.recordNum = 0;
        blkInstance->write(reinterpret_cast<const char*>(&header), 0 , sizeof(header));
        break;
    }
    case FileType::INDEX: {
        Meta::IndexHeader header;
        header.filetype = static_cast<uint32_t>(type);
        header.blockNum = 1;
        header.blockOffset = BLOCK_SIZE;
        header.rootOffset = 0;
        blkInstance->write(reinterpret_cast<const char*>(&header), 0 , sizeof(header));
        break;
    }
    }
    blkInstance->sync(true);
    cache.emplace_back(blkInstance);
    blockCnt++;
}

void deleteBlockSet(const string& setName) {
    for(auto iter = cache.begin(); iter != cache.end(); ++iter) {
        auto blk = *iter;
        if(blk->getFilename() == setName) {
            blk->setDel();
        }
    }
    synchronize();
    FS::unlink(setName.c_str());
}

void synchronize() {
    for(auto iter = cache.begin(); iter != cache.end(); ++iter) {
        auto blk = *iter;
        if(blk->isDirty()) {
            blk->sync(true);
        }
        if(blk->isDel()) {
            cache.erase(iter);
        }
    }
}

void maintain() {
    while (cache.size() >= CACHE_SIZE) {
        for(auto iter = cache.rbegin(); iter != cache.rend(); ++iter) {
            auto blk = *iter;
            if(blk->isCold()) {
                continue;
            }
            if(blk->isDirty()) {
                blk->sync(true);
                cache.erase(std::next(iter).base());
            }
            break;
        }
    }
}

BlockInstance getBlock(const BlockUID& blockUID) {
    auto filename = std::get<0>(blockUID);
    auto ofs = std::get<1>(blockUID);
    for(auto iter = cache.begin(); iter != cache.end(); ++iter) {
        auto blk = *iter;
        if(blk->getFilename() == filename && blk->getOffset() == ofs) {
            cache.erase(iter);
            cache.push_front(blk);
            return blk;
        }
    }
    maintain();
    auto blkInstance = std::make_shared<Block>(blockUID);
    blkInstance->sync(false);
    cache.push_front(blkInstance);
    return blkInstance;
}

BlockInstance setBlock(const BlockUID& blockUID,
            const char* _src, const size_t _offset, const size_t _size){
    auto filename = std::get<0>(blockUID);
    auto ofs = std::get<1>(blockUID);
    for(auto iter = cache.begin(); iter != cache.end(); ++iter) {
        auto blk = *iter;
        if(blk->getFilename() == filename && blk->getOffset() == ofs) {
            blk->write(_src, _offset, _size);
            blk->sync(true);
            return blk;
        }
    }
    maintain();
    auto blkInstance = std::make_shared<Block>(blockUID);
    blkInstance->write(_src, _offset, _size);
    blkInstance->sync(true);
    cache.push_front(blkInstance);
    return blkInstance;
}

BlockInstance setBlock(const BlockUID& blockUID){
    auto filename = std::get<0>(blockUID);
    auto ofs = std::get<1>(blockUID);
    for(auto iter = cache.begin(); iter != cache.end(); ++iter) {
        auto blk = *iter;
        if(blk->getFilename() == filename && blk->getOffset() == ofs) {
            blk->sync(true);
            return blk;
        }
    }
    maintain();
    auto blkInstance = std::make_shared<Block>(blockUID);
    blkInstance->write(zeroBuffer, 0, BLOCK_SIZE);
    blkInstance->sync(true);
    cache.push_front(blkInstance);
    return blkInstance;
}

}