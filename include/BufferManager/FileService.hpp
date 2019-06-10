#ifndef FS_HPP
#define FS_HPP

#include "Common.hpp"

namespace CS {
namespace FS {

using FilePool = vector<FILE*>;
using FileBlockNum = vector<int>; 
class FileService {
private:
    FilePool filePool;
    FileBlockNum fileBlockCnt;
    
    inline void fidCheck(int fid) {
        if (fid >= filePool.size() && fid < 0) 
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
    tuple<int, bool> createOrOpenFile(const char* filename);
    int openFile(const char* filename);
    void unlink(int fid, const char* filename);
    bool existsF(const char* filename);
    bool exists(int fid);
    FILE* getFile(int fid) { 
        fidCheck(fid);
        return filePool[fid];
    }
    size_t allocBlock(int fid);
    void writeBlock(int fid, size_t bOffset, char* data);
    void readBlock(char* dest, int fid, size_t bOffset);
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