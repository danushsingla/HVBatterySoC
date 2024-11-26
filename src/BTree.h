#ifndef BTREE_H
#define BTREE_H
#include <functional>
#include <stack>
#include <stdexcept>

// N is the order of btree
template<typename K, typename V, int N = 3, typename Comp = std::less<K>>
class BTree {
    static_assert(N >= 3, "N must be at least 3");
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
        } };
public:
    void insert(K k, V v);

    typedef BTreeIterator const_iterator_type;

    const_iterator_type begin() const {
        BTreeIterator iter;
        auto top = root;
        while (top->children[0]) {
            top = top->children[0];
        }
        iter.btree = top;
        iter.current_index = 0;
        return iter;
    }
    const_iterator_type end() const {
        BTreeIterator iter;
        iter.btree = nullptr;
        return iter;
    }
    
    // Returns smallest node greater than or equal to the key
    // Returns `this->end()` if the key is greater than the maximum
    const_iterator_type rangeLookUp(const K& key) const {
        auto cur = root;
    lol:
        while (cur != nullptr) {
            for (int i = 0; i < cur->size; i++) {
                if (!comp(cur->keys[i], key)) {
                    if (cur->children[i]) {
                        cur = cur->children[i];
                        goto lol;
                    } else {
                        return { cur, i };
                    }
                }
            }
            if (cur->children[cur->size]) {
                cur = cur->children[cur->size];
                goto lol;
            }
            auto target = cur->parent;

            while (target && target->parent_index >= (target->parent ? target->parent->size : 0)) {
                target = target->parent;
            }

            if (!target) {
                return this->end();
            }
            
            return { target, target->parent_index };
        }

        return this->end();
    }

     const V& operator[](const K& key) const {
         auto lookup = rangeLookUp(key);
         if (lookup == this->end() || (*lookup).first != key) {
             throw std::runtime_error("Invalid index");
         }

         return (*lookup).second;
     }
};

#include "BTree.cpp"
#endif
