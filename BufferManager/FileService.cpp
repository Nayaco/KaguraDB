#include "BufferManager/FileService.hpp"

namespace CS {
namespace FS {
static char zeroBuffer[65536] = { 0 };

void FileService::createFile(const char* filename) {
    FILE* fp = fopen(filename, "wb");
    fpCheck(fp, "create", filename);   
    fclose(fp);
}

int FileService::openFile(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    fpCheck(fp, "open", filename);
    filePool.emplace_back(fp);
    fseek(fp, 0, SEEK_END);
    fileBlockCnt.push_back((int)ftell(fp) / BLOCK_SIZE);
    return (int)(filePool.size() - 1);
}

int FileService::createOrOpenFile(const char* filename) {
    FILE* fp = fopen(filename, "rb+");
    fpCheck(fp, "create or open", filename);
    filePool.emplace_back(fp);
    fseek(fp, 0, SEEK_END);
    fileBlockCnt.push_back((int)ftell(fp) / BLOCK_SIZE);
    return (int)(filePool.size() - 1);
}

size_t FileService::allocBlock(int fid) {
    fidCheck(fid);
    FILE* fp = filePool[fid];
    fseek(fp, 0, SEEK_END);
    size_t offset = (int)ftell(fp);
    fwrite(zeroBuffer, BLOCK_SIZE, 1, fp);
    fflush(fp);
    fileBlockCnt[fid]++; 
    return offset / BLOCK_SIZE;
}

void FileService::writeBlock(int fid, size_t offset, char* data) {
    fidCheck(fid);
    FILE *fp = filePool[fid];
    fseek(fp, offset * BLOCK_SIZE, SEEK_SET);
    fwrite(data, BLOCK_SIZE, 1, fp);
    fflush(fp);
}

char* FileService::readBlock(int fid, size_t offset) {
    fidCheck(fid);
    FILE *fp = filePool[fid];
    fseek(fp, offset * BLOCK_SIZE, SEEK_SET);
    char* data = new char[BLOCK_SIZE];
    fread(data, BLOCK_SIZE, 1, fp);
    return data;
}

FileService::~FileService() {
    for(auto& fp: filePool) {
        fclose(fp);
    }
}

}
}