#ifndef FS_HPP
#define FS_HPP

#include "Common.hpp"

namespace CS {
namespace FS {

using FilePool = vector<FILE*>;
using FileBlockNum = vector<int>; 

void init();
void exit();

void createFile(const char* filename);
tuple<int, bool> createOrOpenFile(const char* filename);
int openFile(const char* filename);
void unlink(int fid, const char* filename);
bool existsF(const char* filename);
bool exists(int fid);
size_t allocBlock(int fid);
void writeBlock(int fid, size_t bOffset, char* data);
void readBlock(char* dest, int fid, size_t bOffset);
FILE* getFile(int fid);
int getBlockCnt(int fid);

}
}

#endif