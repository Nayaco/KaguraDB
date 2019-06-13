#ifndef FS_HPP
#define FS_HPP

#include "Common.hpp"

namespace CS {
namespace FS {

void createFile(const char* filename);

void unlink(const char* filename);

bool exists(const char* filename);

void writeBlock(const char* filename, size_t bOffset, char* data);

void readBlock(char* dest, const char* filename, size_t bOffset);

void clearBlock(const char* filename, size_t bOffset);

}
}

#endif