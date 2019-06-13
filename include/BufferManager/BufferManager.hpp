#ifndef BM_BUFFER_MANAGER_HPP
#define BM_BUFFER_MANAGER_HPP
#include "Common.hpp"
#include "BaseSpec/Files.hpp"
#include "BufferManager/FileService.hpp"
#include "BufferManager/BlockSpec.hpp"
#include "BufferManager/Block.hpp"

namespace CS {
using namespace FSpec;
static const int CACHE_SIZE = 1024;

void init();
void exit();
    
void createBlockSet(const string& setName, const FileType type);
void deleteBlockSet(const string& setName);
void synchronize();
void maintain();
BlockInstance getBlock(const BlockUID&);
BlockInstance setBlock(const BlockUID& blockUID,
            const char* _src, const size_t _offset, const size_t _size);
BlockInstance setBlock(const BlockUID& blockUID);
}
#endif