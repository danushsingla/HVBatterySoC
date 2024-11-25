#ifndef BTREE_H
#define BTREE_H
#include <functional>

// N is the order of btree
template<typename K, typename V, int N = 3, typename Comp = std::less<K>>
class BTree {
    struct BTreeNode {
        K keys[N];
        V values[N];
        int size;
        BTreeNode* children[N + 1];
        BTreeNode* parent;

        BTreeNode();

        int insert(K k, V v, const Comp& comp);
        bool isFull();
        bool isLeaf();
    };
    BTreeNode *root = nullptr;

    Comp comp;
    void balance(BTreeNode *node);
public:
    void insert(K k, V v);
};

#include "BTree.cpp"
#endif
