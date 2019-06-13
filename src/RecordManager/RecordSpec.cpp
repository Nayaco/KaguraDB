#include "RecordManager/RecordSpec.hpp"
namespace RM {

size_t recordBinarySize(const SchemaInstance schema) {
    size_t binSize = 8;
    size_t blkSize = 1;
    for(auto i = 0; i < schema->attrs.size(); ++i) {
        binSize += schema->attrs[i].size();
        if(binSize > BLOCK_SIZE) {
            binSize = schema->attrs[i].size();
            blkSize++;
        }
    }
    return blkSize;
}

}