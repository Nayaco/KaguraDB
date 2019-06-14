#include "IndexManager/IndexSpec.hpp"

namespace IM {
nodeBlockStruct* genLeafNodeBlock(int kn, int parent, vector<CS::BlockUID>& vals) {
    nodeBlockStruct* blk = new nodeBlockStruct{ };
    blk->del    = 0;
    blk->kn     = static_cast<uint32_t>(kn);
    blk->leaf   = 1;
    blk->parent = parent;
    for(auto i = 0; i < kn; ++i)
        blk->value[i] = static_cast<uint32_t>(std::get<1>(vals[i]));
    return blk;
}

nodeBlockStruct* genInnerNodeBlock(int kn, int parent, vector<Value>& keys,vector<int>& children) {
    auto write = [](char* src, char* dest, size_t offset, size_t size){
        memcpy(dest + offset, src, size);
    };
    nodeBlockStruct* blk = new nodeBlockStruct{ };
    blk->del    = 0;
    blk->kn     = static_cast<uint32_t>(kn);
    blk->leaf   = 0;
    blk->parent = parent;
    for(auto i = 0; i < kn; ++i)
        blk->child[i] = static_cast<uint32_t>(children[i]);
    for(auto i = 0; i < kn - 1; ++i)
        write(keys[i].val(), blk->keys, i * 256, keys[i].size());
    return blk;
}

}