#include "IndexManager/BPlusIndex.hpp"

namespace IM {
Node::Node(const Pos& pos, const Attribute& attr)
    : self(pos), attr(attr) {
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

bool Node::isRoot() { return parent == 0; }

void Node::sync(bool toDisk) {
    if(toDisk) {
        nodeBlockStruct* blk;
        if(isLeaf) blk = genLeafNodeBlock(kn, parent, k, value);
        if(!isLeaf) blk = genInnerNodeBlock(kn, parent, k, child);
        CS::setBlock(self, reinterpret_cast<char*>(blk), 0, sizeof(nodeBlockStruct));
        delete blk;
    } else {
        nodeBlockStruct* blk = new nodeBlockStruct{ };
        auto cacheBlk = CS::getBlock(self);
        cacheBlk->read(reinterpret_cast<char*>(blk), 0, sizeof(nodeBlockStruct));
        parent = blk->parent;
        isLeaf = static_cast<bool>(blk->leaf);
        if(!isLeaf) {
            for (auto i = 0; i < kn - 1; ++i){
                Value _value(attr);
                memcpy(_value.val(), blk->keys + i * 256, _value.size());
                k.push_back(_value);
            }
            for (auto i = 0; i < kn; ++i) {
                child.push_back(blk->child[i]);
            }
        } else {
            for (auto i = 0; i < kn - 1; ++i){
                Value _value(attr);
                memcpy(_value.val(), blk->keys + i * 256, _value.size());
                k.push_back(_value);
            }
            for (auto i = 0; i < kn; ++i) {
                child.push_back(blk->child[i]);
            }
        }
        delete blk;
    }
}

BPTree::BPTree(const string& _filename, const Attribute& _pk)
    :filename(_filename), pk(_pk) {
    auto blk_0 = CS::getBlock(CS::makeUID(filename, 0));
    FSpec::Meta::IndexHeader iHeader;
    blk_0->read(reinterpret_cast<char*>(&iHeader), 0, sizeof(iHeader));
    root = new Node(CS::makeUID(filename, iHeader.startOffset), pk);
    nodeNum = iHeader.blockNum - 1;
}

Node* BPTree::getNode(int ofs) {
    if(stack.find(ofs) == stack.end()) {
        auto node = new Node(CS::makeUID(filename, ofs), pk);
        stack[ofs] = node;
        return node;
    } else {
        return stack[ofs];
    }
}

Node* BPTree::mergeNode(Node* node1, Node* node2) {
    
    auto blk_0 = CS::getBlock(CS::makeUID(filename, 0));
    FSpec::Meta::IndexHeader iHeader;
    blk_0->read(reinterpret_cast<char*>(&iHeader), 0, sizeof(iHeader));
    iHeader.blockNum++;
    blk_0->write(reinterpret_cast<char*>(&iHeader), 0, sizeof(iHeader));
    
    Node* newNode = new Node(CS::makeUID(filename, 1 + nodeNum++), pk);
    (newNode->k).insert(newNode->k.begin(), node1->k.begin(), node1->k.end());
    (newNode->k).insert(newNode->k.begin(), node2->k.begin(), node2->k.end());
    (newNode->child).insert(newNode->child.begin(), node1->child.begin(), node1->child.end());
    (newNode->child).insert(newNode->child.begin(), node2->child.begin(), node2->child.end());
    newNode->parent = node1->parent;
    newNode->isLeaf = node1->isLeaf;    
    newNode->sync(true);

    return newNode;
}

pair<Node*, Node*> BPTree::splitNode(Node* node1) {
    auto blk_0 = CS::getBlock(CS::makeUID(filename, 0));
    FSpec::Meta::IndexHeader iHeader;
    blk_0->read(reinterpret_cast<char*>(&iHeader), 0, sizeof(iHeader));
    iHeader.blockNum +=2 ;
    blk_0->write(reinterpret_cast<char*>(&iHeader), 0, sizeof(iHeader));
    
    Node* newNode1 = new Node(CS::makeUID(filename, 1 + nodeNum++), pk);
    Node* newNode2 = new Node(CS::makeUID(filename, 1 + nodeNum++), pk);
    (newNode1->k).insert(newNode1->k.begin(), node1->k.begin(), node1->k.begin() + (knMax / 2));
    (newNode1->child).insert(newNode1->child.begin(), node1->child.begin(), node1->child.begin() + (knMax / 2));
    (newNode1->value).insert(newNode1->value.begin(), node1->value.begin(), node1->value.begin() + (knMax / 2));
    (newNode2->k).insert(newNode2->k.begin(), node1->k.begin() + (knMax / 2), node1->k.end());
    (newNode2->child).insert(newNode2->child.begin(), node1->child.begin() + (knMax / 2) + 1, node1->child.end());
    (newNode2->value).insert(newNode2->value.begin(), node1->value.begin() + (knMax / 2) + 1, node1->value.end());
    
}
Pos BPTree::find(const Key& key) {
    Node* current = root;
    int   currentNum;
    while(!current->isLeaf) {
        int nextNum = current->child[current->k.size()];
        for(int i = 0; i < current->k.size(); i++) {
            if(current->k[i] >= key) {
                nextNum = current->child[i];
                break;
            }
        }
        current = getNode(nextNum);
    }
    for(int i = 0; i < current->k.size(); i++) {
        if(key == current->k[i]) return current->value[i];
    }
    throw BPTException::NOTFOUND;
    return current->value[0];
}
void BPTree::insert(const Key& key, const Pos& val) {
    try {
        Pos ex = find(key);
    } catch(BPTException& e) {
        Node* current = root;
        int   currentNum;
        while(!current->isLeaf) {
            int nextNum = current->child[current->k.size()];
            for(int i = 0; i < current->k.size(); i++) {
                if(current->k[i] >= key) {
                    nextNum = current->child[i];
                    break;
                }
            }
            current = getNode(nextNum);
        }
        current->k.push_back(key);
        current->value.push_back(val);
    }
}

void BPTree::remove(const Key& key) {
    try {
        Pos ex = find(key);
        Node* current = root;
        int   currentNum;
        while(!current->isLeaf) {
            int nextNum = current->child[current->k.size()];
            for(int i = 0; i < current->k.size(); i++) {
                if(current->k[i] >= key) {
                    nextNum = current->child[i];
                    break;
                }
            }
            current = getNode(nextNum);
        }
    } catch(BPTException& e) {
       throw SQLError("Index Error: no such key");
    }
}

BPTree::~BPTree() { }

}