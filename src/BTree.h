#ifndef BTREE_H
#define BTREE_H
#include <functional>
#include <stack>

// N is the order of btree
template<typename K, typename V, int N = 3, typename Comp = std::less<K>>
class BTree {
    struct BTreeNode {
        K keys[N];
        V values[N];
        int size;
        BTreeNode* children[N + 1];
        BTreeNode* parent;
        int parent_index;

        BTreeNode();

        int insert(K k, V v, const Comp& comp);
        bool isFull();
        bool isLeaf();
    };
    BTreeNode *root = nullptr;

    Comp comp;
    void balance(BTreeNode *node);
    struct BTreeIterator {
        BTreeNode *btree;
        int current_index;

        bool operator==(const BTreeIterator& other) {
            return this->btree == other.btree;
        }

        bool operator!=(const BTreeIterator& other) {
            return !(*this == other);
        }

        void operator++() {
            if (btree == nullptr) {
                return;
            }
            current_index++;
            if (current_index <= btree->size && btree->children[current_index]) {
                while (current_index <= btree->size && btree->children[current_index]) {
                    btree = btree->children[current_index];
                    current_index = 0;
                }
            } else {
                while (btree && current_index >= btree->size) {
                    current_index = btree->parent_index;
                    btree = btree->parent;
                }
            }
        }

        const std::pair<const K&, const V&> operator*() {
            return { btree->keys[current_index], btree->values[current_index] };
        }
    };
public:
    void insert(K k, V v);

    typedef BTreeIterator const_iterator_type;

    const_iterator_type begin() {
        BTreeIterator iter;
        auto top = root;
        while (top->children[0]) {
            top = top->children[0];
        }
        iter.btree = top;
        iter.current_index = 0;
        return iter;
    }
    const_iterator_type end() {
        BTreeIterator iter;
        iter.btree = nullptr;
        return iter;
    }
    
};

#include "BTree.cpp"
#endif
