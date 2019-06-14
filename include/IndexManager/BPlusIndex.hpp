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
    string filename;
    int kn;
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
    int   knMax;
    string filename;
    int   nodeNum;
    Attribute pk;
    BPTree(const string& indexName, const Attribute& pk, const string& filename);
    ~BPTree();
    void recursiveDel(Node* node);
    Pos find(const Key&);
    void insert(const Key&, const Pos& val);
    void remove(const Key&);
};

}
#endif
