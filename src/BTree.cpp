#include <cassert>
#include <iostream>
#include <stdexcept>

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
    if (!(current_node && current_node->isLeaf() && !current_node->isFull())) {
        throw std::runtime_error("Bizarre btree behavior");
    }
    for (int i = current_node->size + 1; i < N + 1; i++) {
        current_node->children[i] = nullptr;
    }
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
        node->children[i] = nullptr;
        right_side->size++;
    }
    right_side->children[node->size - median - 1] = node->children[node->size];
    node->children[node->size] = nullptr;
    if (right_side->children[node->size - median - 1]) {
        right_side->children[node->size - median - 1]->parent_index = node->size - median - 1;
        right_side->children[node->size - median - 1]->parent = right_side;
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
    // assert(node->parent->children[slot] == node);
    node->parent->children[slot + 1] = right_side;
    // node->parent_index = slot;
    right_side->parent_index = slot + 1;
    right_side->parent = node->parent;
    // recursion moment
    if (node->parent->isFull()) {
        balance(node->parent);
    }
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
    for (int i = 0; i < size+1; i++) {
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
        if (children[i]) {
            children[i]->parent_index = i+1;
        }
        if (children[i+1]) {
            children[i+1]->parent_index = i;
        }
        std::swap(children[i+1], children[i]);

        std::swap(keys[i-1], keys[i]);
        std::swap(values[i-1], values[i]);
        --i;
    }
    keys[i] = k;
    values[i] = v;
    ++size;
    return i;
}

