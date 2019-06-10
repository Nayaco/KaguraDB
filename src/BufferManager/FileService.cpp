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
    FILE* fp = fopen(filename, "rb+");
    fpCheck(fp, "open", filename);
    filePool.emplace_back(fp);
    fseek(fp, 0, SEEK_END);
    fileBlockCnt.push_back((int)ftell(fp) / BLOCK_SIZE);
    return (int)(filePool.size() - 1);
}

tuple<int, bool> FileService::createOrOpenFile(const char* filename) {
    bool fileExists = existsF(filename);
    if(!fileExists) createFile(filename);
    FILE* fp = fopen(filename, "rb+");
    fpCheck(fp, "create or open", filename);
    filePool.emplace_back(fp);
    fseek(fp, 0, SEEK_END);
    fileBlockCnt.push_back((int)ftell(fp) / BLOCK_SIZE);
    return std::make_tuple((int)(filePool.size() - 1), fileExists);
}

void FileService::unlink(int fid, const char* filename) {
    fclose(filePool[fid]);
    remove(filename);
    filePool[fid] = NULL;
    fileBlockCnt[fid] = 0;
}

bool FileService::existsF(const char* filename) {
    static struct stat statBuffer;
    return (stat(filename, &statBuffer) == 0);
}

bool FileService::exists(int fid) {
    fidCheck(fid);
    return filePool[fid] != NULL;
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

void FileService::writeBlock(int fid, size_t bOffset, char* data) {
    fidCheck(fid);
    FILE *fp = filePool[fid];
    fseek(fp, bOffset * BLOCK_SIZE, SEEK_SET);
    fwrite(data, BLOCK_SIZE, 1, fp);
    fflush(fp);
}

void FileService::readBlock(char* dest, int fid, size_t bOffset) {
    fidCheck(fid);
    FILE *fp = filePool[fid];
    fseek(fp, bOffset * BLOCK_SIZE, SEEK_SET);
    fread(dest, BLOCK_SIZE, 1, fp);
}

FileService::~FileService() {
    for(auto& fp: filePool) {
        if(fp != NULL) fclose(fp);
    }
}

}
}