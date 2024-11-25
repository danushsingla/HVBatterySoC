#include "BTree.h"
#include <cassert>

template <typename K, typename V, int N, typename Comp>
void BTree<K, V, N, Comp>::insert(K k, V v) {
    if (!root) {
        root = new BTreeNode();
        root->insert(k, v, comp);
        return;
    }
    BTreeNode *current_node = root;
    while (current_node != nullptr && !current_node->isLeaf()) {
        int i = 0;
        // trolling
        for (; i < current_node->size; i++) {
            if (comp(k, current_node->keys[i])) {
                break;
            }
        }
        current_node = current_node->children[i];
    }
    assert(current_node->isLeaf() && !current_node->isFull());
    current_node->insert(k, v, comp);

    // balance
    balance(current_node);
}

template <typename K, typename V, int N, typename Comp>
void BTree<K, V, N, Comp>::balance(BTreeNode *node) {
    if (node == nullptr) {
        return;
    }
    if (!node->isFull()) {
        return;
    }

    int median = node->size / 2;

    // split around median
    auto right_side = new BTreeNode();
    for (int i = median+1; i < node->size; i++) {
        right_side->keys[i - median - 1] = node->keys[i];
        right_side->values[i - median - 1] = node->values[i];
        right_side->children[i - median - 1] = node->children[i];
        if (right_side->children[i - median - 1]) {
            right_side->children[i - median - 1]->parent_index = i - median - 1;
            right_side->children[i - median - 1]->parent = right_side;
        }
        right_side->size++;
    }
    right_side->children[node->size - median - 1] = node->children[node->size];
    if (right_side->children[node->size - median - 1]) {
        right_side->children[node->size - median - 1]->parent_index = node->size - median - 1;
    }

    node->size /= 2;
    
    
    // we have to create a new node to be the root and propagate too
    if (node->parent == nullptr) {
        node->parent = new BTreeNode();
        node->parent->children[0] = node;
        node->parent_index = 0;
        this->root = node->parent;
    }
    
    // propagate median up
    int slot = node->parent->insert(node->keys[median], node->values[median], comp);
    node->parent->children[slot + 1] = right_side;
    right_side->parent_index = slot + 1;
    // recursion moment
    if (node->parent->isFull()) {
        balance(node->parent);
    }
    right_side->parent = node->parent;
}

template <typename K, typename V, int N, typename Comp>
BTree<K, V, N, Comp>::BTreeNode::BTreeNode() {
    for (int i = 0; i < N; i++) {
        this->children[i] = nullptr;
    }
    this->size = 0;
    this->parent = nullptr;
}

template <typename K, typename V, int N, typename Comp>
bool BTree<K, V, N, Comp>::BTreeNode::isFull() {
    return size == N;
}

template <typename K, typename V, int N, typename Comp>
bool BTree<K, V, N, Comp>::BTreeNode::isLeaf() {
    for (int i = 0; i < N + 1; i++) {
        if (children[i] != nullptr) {
            return false;
        }
    }
    return true;
}

template <typename K, typename V, int N, typename Comp>
int BTree<K, V, N, Comp>::BTreeNode::insert(K k, V v, const Comp& comp) {
    assert(!isFull());
    int i = size;
    while (i > 0 && comp(k, this->keys[i-1])) {
        // idk if this is right yet
        if (children[i-1]) {
            children[i-1]->parent_index++;
        }
        if (children[i]) {
            children[i]->parent_index--;
        }
        std::swap(children[i-1], children[i]);

        std::swap(keys[i-1], keys[i]);
        std::swap(values[i-1], values[i]);
        --i;
    }
    keys[i] = k;
    values[i] = v;
    ++size;
    return i;
}

