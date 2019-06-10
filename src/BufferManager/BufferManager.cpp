#include "BufferManager/BufferManager.hpp"
namespace CS {

BufferManager::BufferManager(const FileServiceInstance& fsInstance)
    : fsInstance(fsInstance), blockCnt(0) { 
    cache.clear(); 
    fileTable.clear();
}

BufferManager::~BufferManager() {
    synchronize();
}

void BufferManager::createBlockSet(const string& setName, const FileType type){
    maintain();
    int fid; bool exists;
    std::tie(fid, exists) = fsInstance->createOrOpenFile(setName.c_str());
    // using namespace std;
    // cout<<fid<<endl;
    fileTable[setName] = fid;
    auto blkInstance = std::make_shared<Block>(makeUID(fid, 0), fsInstance);
    if(exists) { 
        blkInstance->sync();
        cache.emplace_back(blkInstance);
        blockCnt++;
        return;
    }
    switch (type)
    {
    case FileType::CATALOG: {
        Meta::CatalogHeader header;
        header.filetype = static_cast<uint32_t>(type);
        header.blockNum = 1;
        header.blockOffset = BLOCK_SIZE;
        header.indexOffset = 0;
        header.tableOffset = 0;
        header.rootOffset = 0;
        header.startOffset = 0;
        blkInstance->write(reinterpret_cast<const char*>(&header), 0 , sizeof(header));
        break;
    }
    case FileType::TABLE: {
        Meta::TableHeader header;
        header.filetype = static_cast<uint32_t>(type);
        header.blockNum = 1;
        header.blockOffset = BLOCK_SIZE;
        header.recordNum = 0;
        header.rootOffset = 0;
        header.startOffset = 0;
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
    blkInstance->sync();
    cache.emplace_back(blkInstance);
    blockCnt++;
}

void BufferManager::deleteBlockSet(const string& setName) {
    auto fid = fileTable[setName];
    for(auto iter = cache.begin(); iter != cache.end(); ++iter) {
        auto blk = *iter;
        if(blk->getFid() == fid) {
            blk->setDel();
        }
    }
    synchronize();
    fsInstance->unlink(fid, setName.c_str());
}

void BufferManager::synchronize() {
    for(auto iter = cache.rbegin(); iter != cache.rend(); ++iter) {
        auto blk = *iter;
        if(blk->isDirty()) {
            blk->sync();
        }
        if(blk->isDel()) {
            cache.erase(std::next(iter).base());
        }
    }
}

void BufferManager::maintain() {
    while (cache.size() >= CACHE_SIZE) {
        for(auto iter = cache.rbegin(); iter != cache.rend(); ++iter) {
            auto blk = *iter;
            if(blk->isCold()) {
                continue;
            }
            if(blk->isDirty()) {
                blk->sync();
                cache.erase(std::next(iter).base());
            }
            break;
        }
    }
}

BlockInstance BufferManager::getBlock(const BufferUID& bufferUID) {
    auto filename = std::get<0>(bufferUID);
    bool exists;
    if(fileTable.find(filename) == fileTable.end()) {
        std::tie(fileTable[filename], exists) = fsInstance->createOrOpenFile(filename.c_str());
    }
    auto fid = fileTable[std::get<0>(bufferUID)];
    auto ofs = std::get<1>(bufferUID);
    for(auto iter = cache.begin(); iter != cache.end(); ++iter) {
        auto blk = *iter;
        if(blk->getFid() == fid && blk->getOffset() == ofs) {
            cache.erase(iter);
            cache.push_front(blk);
            return blk;
        }
    }
    maintain();
    auto blkInstance = std::make_shared<Block>(makeUID(fid, ofs), fsInstance);
    blkInstance->sync();
    cache.push_front(blkInstance);
    return blkInstance;
}

}