#ifndef BM_BUFFER_MANAGER_HPP
#define BM_BUFFER_MANAGER_HPP
#include "Common.hpp"
#include "BufferManager/FileService.hpp"
#include "BufferManager/BlockSpec.hpp"
#include "BufferManager/Block.hpp"

namespace CS {

using FS::FileServiceInstance;
using std::list;
class BufferManager {
    int blockCnt = 0;
    FileServiceInstance fsInstance;
    
}   

}
#endif