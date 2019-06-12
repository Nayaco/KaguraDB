#ifndef BM_BUFFER_MANAGER_HPP
#define BM_BUFFER_MANAGER_HPP
#include "Common.hpp"
#include "BaseSpec/Files.hpp"
#include "BufferManager/FileService.hpp"
#include "BufferManager/BlockSpec.hpp"
#include "BufferManager/Block.hpp"

namespace CS {
using namespace FSpec;
using BufferUID = tuple<string, int>;
static const int CACHE_SIZE = 1024;

inline BufferUID makeUID(string setName, int offset) {
    return std::make_tuple(setName, offset);
}

void init();
void exit();
    
void createBlockSet(const string& setName, const FileType type);
void deleteBlockSet(const string& setName);
void synchronize();
void maintain();
BlockInstance getBlock(const BufferUID&);


}
#endif