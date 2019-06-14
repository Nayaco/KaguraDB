#ifndef INDEX_SPEC_HPP
#define INDEX_SPEC_HPP
#include "Common.hpp"
#include "BaseSpec/Types.hpp"
#include "BufferManager/BufferManager.hpp"
namespace IM {

struct nodeBlockStruct {
    uint32_t    del;
    uint32_t    kn;
    uint32_t    leaf;
    uint32_t    parent;
    uint32_t    child[8];
    uint32_t    value[8];
    char        keys[8 * 256];
};

nodeBlockStruct* genLeafNodeBlock(
            int kn, 
            int parent, 
            vector<CS::BlockUID>& vals
            );
nodeBlockStruct* genInnerNodeBlock(
            int kn, 
            int parent, 
            vector<Value>& keys,
            vector<int>& children
            );

}

#endif