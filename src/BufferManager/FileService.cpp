#include "BufferManager/FileService.hpp"

namespace CS {
namespace FS {

static char zeroBuffer[65536] = { 0 };

void createFile(const char* filename) {
    FILE* fp = fopen(filename, "wb");
    fclose(fp);
}

void unlink(const char* filename) {
    remove(filename);
}

bool exists(const char* filename) {
    static struct stat statBuffer;
    return (stat(filename, &statBuffer) == 0);
}

void writeBlock(const char* filename, size_t bOffset, char* data) {
    FILE *fp = fopen(filename, "rb+");
    fseek(fp, bOffset * BLOCK_SIZE, SEEK_SET);
    fwrite(data, BLOCK_SIZE, 1, fp);
    fflush(fp);
    fclose(fp);
}

void readBlock(char* dest, const char* filename, size_t bOffset) {
    FILE *fp = fopen(filename, "rb");
    fseek(fp, bOffset * BLOCK_SIZE, SEEK_SET);
    fread(dest, BLOCK_SIZE, 1, fp);
    fclose(fp);
}

void clearBlock(const char* filename, size_t bOffset) {
    FILE *fp = fopen(filename, "rb+");
    fseek(fp, bOffset * BLOCK_SIZE, SEEK_SET);
    fwrite(zeroBuffer, BLOCK_SIZE, 1, fp);
    fflush(fp);
    fclose(fp);
}

}
}