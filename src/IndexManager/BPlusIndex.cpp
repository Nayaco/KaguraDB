#include "IndexManager/BPlusIndex.hpp"

namespace IM {
Node::Node(const Pos& pos, const Attribute& attr)
    : self(pos), attr(attr) {
    kn = 0;
    k.clear();
    parent = -1;
    child.clear();
    value.clear();
    isLeaf = false;
}

Node::~Node() { }

Node* Node::getChild(int n) {
    Node* _child = new Node(CS::makeUID(std::get<0>(self), child[n]), attr);
    _child->sync(false);
    return _child;
}

Node* Node::getParent() {
    Node* par = new Node(CS::makeUID(std::get<0>(self), parent), attr);
    par->sync(false);
    return par;
}

bool Node::isRoot() {
    return parent == 0;
}

void Node::sync(bool toDisk) {
    if(toDisk) {
        nodeBlockStruct* blk;
        if(isLeaf) blk = genLeafNodeBlock(kn, parent, value);
        if(!isLeaf) blk = genInnerNodeBlock(kn, parent, k, child);
        CS::setBlock(self, reinterpret_cast<char*>(blk), 0, sizeof(nodeBlockStruct));
        delete blk;
    } else {
        nodeBlockStruct* blk = new nodeBlockStruct{ };
        auto cacheBlk = CS::getBlock(self);
        cacheBlk->read(reinterpret_cast<char*>(blk), 0, sizeof(nodeBlockStruct));
        kn = blk->kn;
        parent = blk->parent;
        isLeaf = static_cast<bool>(blk->leaf);
        if(!isLeaf) {
            for (auto i = 0; i < kn - 1; ++i){
                Value _value(attr);
                memcpy(_value.val(), blk->keys + i * 256, _value.size());
            }
            for (auto i = 0; i < kn; ++i) {
                child.push_back(blk->child[i]);
            }
        } else {
            for (auto i = 0; i < kn; ++i) {
                child.push_back(blk->child[i]);
            }
        }
        delete blk;
    }
}

// BPTree::BPTree(const string& indexName);

}