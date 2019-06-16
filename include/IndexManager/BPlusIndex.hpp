#ifndef BPI_HPP
#define BPI_HPP
#include "Common.hpp"
#include "BaseSpec/Types.hpp"
#include "BufferManager/BlockSpec.hpp"
#include "BufferManager/BufferManager.hpp"
#include "IndexManager/IndexSpec.hpp"

namespace IM {
using Key = Value;
using Pos = CS::BlockUID;
struct Node {
    Attribute attr;
    const static int kn = 5;
    vector<Key> k;
    int parent;
    vector<int> child;
    vector<Pos> value;
    Pos self;
    bool isLeaf;
    Node(const Pos& pos, const Attribute& attr);
    ~Node();
    Node* getChild(int n);
    Node* getParent();
    bool isRoot();
    void sync(bool toDisk);
};

struct BPTree {
    Node* root;
    const static int knMax = 5;
    map<int, Node*> stack;
    string filename;
    int   nodeNum;
    Attribute pk;
    BPTree(const string& _filename, const Attribute& _pk);
    ~BPTree();
    Node* getNode(int ofs);
    Node* getLeftMost(Node*);
    Node* mergeNode(Node* node1, Node* node2);
    pair<Node*, Node*> splitNode(Node* node1);
    void recursiveDel(Node* node);
    Pos find(const Key&);
    void insert(const Key&, const Pos& val);
    void remove(const Key&);
};

enum class BPTException {
    NOTFOUND
};

}
#endif
