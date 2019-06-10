#ifndef FS_HPP
#define FS_HPP

#include "Common.hpp"

namespace CS {
namespace FS {

using FilePool = std::vector<FILE*>;
using FileBlockNum = std::vector<int>; 

class FileService {
private:
    FilePool filePool;
    FileBlockNum fileBlockCnt;
    inline void fidCheck(int fid) {
        if (fid < filePool.size() && fid >= 0) 
            throw BufferError("Fileservice Error: Fid out of bound");
    }
    inline void fpCheck(FILE *fp, const string& method, const char* filename) {
        if ( fp == NULL ) 
            throw BufferError("Fileservice Error: Fail to " + method + " file " + string(filename));
    }

public: 
    FileService() { };
    FileService(const FileService &) = delete;
    FileService operator =(const FileService &) = delete;
    void createFile(const char* filename);
    int createOrOpenFile(const char* filename);
    int openFile(const char* filename);
    FILE* getFile(int fid) { 
        fidCheck(fid);
        return filePool[fid];
    }
    size_t allocBlock(int fid);
    void writeBlock(int fid, size_t bOffset, char* data);
    char* readBlock(int fid, size_t bOffset);
    int getBlockCnt(int fid) { 
        fidCheck(fid);
        return fileBlockCnt[fid];
    }
    ~FileService();
};
using FileServiceInstance = shared_ptr<FileService>;
}
}

#endif