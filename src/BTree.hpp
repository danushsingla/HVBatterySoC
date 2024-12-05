#ifndef BTREE_H
#define BTREE_H
#include <functional>
#include <stack>
#include <stdexcept>

#ifdef ENABLE_ASSERTS
#include <cassert>
#else
#define assert(v) (void)0;
#endif

// N is the order of btree
template <typename K, typename V, int N = 3, typename Comp = std::less<K>>
class BTree {
  static_assert(N >= 3, "N must be at least 3");
  struct BTreeNode {
    K keys[N];
    V values[N];
    int size;
    BTreeNode *children[N + 1];
    BTreeNode *parent;
    int parent_index;

    BTreeNode() {
      for (int i = 0; i < N; i++) {
        this->children[i] = nullptr;
      }
      this->size = 0;
      this->parent = nullptr;
    }

    // Inserts inside the BTree node block
    // Basically just adds key/value to the internal array where it should be
    // Method assumes there's room for the new key/value
    int insert(K k, V v, const Comp &comp) {
      assert(!isFull());
      int i = size;
      while (i > 0 && comp(k, this->keys[i - 1])) {
        // idk if this is right yet
        if (children[i]) {
          children[i]->parent_index = i + 1;
        }
        if (children[i + 1]) {
          children[i + 1]->parent_index = i;
        }
        std::swap(children[i + 1], children[i]);

        std::swap(keys[i - 1], keys[i]);
        std::swap(values[i - 1], values[i]);
        --i;
      }
      keys[i] = k;
      values[i] = v;
      ++size;
      return i;
    }

    // Is the node's array full
    bool isFull() { return size == N; };
    // A node is a leaf if it doesn't have any children
    bool isLeaf() {
      for (int i = 0; i < size + 1; i++) {
        if (children[i] != nullptr) {
          return false;
        }
      }
      return true;
    }

    ~BTreeNode() {
      for (int i = 0; i <= size; i++) {
        if (children[i]) {
          delete children[i];
        }
      }
    }
  };
  BTreeNode *root = nullptr;
  size_t _size = 0;

  // Comparison function
  Comp comp;
  // Checks if a node needs to be split after an insertion and does the split
  void balance(BTreeNode *node) {
    if (node == nullptr) {
      return;
    }
    if (!node->isFull()) {
      return;
    }

    int median = node->size / 2;

    // split around median
    // move the values above the median to a new node
    auto right_side = new BTreeNode();
    for (int i = median + 1; i < node->size; i++) {
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
      right_side->children[node->size - median - 1]->parent_index =
          node->size - median - 1;
      right_side->children[node->size - median - 1]->parent = right_side;
    }

    // Divide's this node size by two to make this node the left side node
    node->size /= 2;

    // we have to create a new node to be the root and propagate too
    if (node->parent == nullptr) {
      node->parent = new BTreeNode();
      node->parent->children[0] = node;
      node->parent_index = 0;
      this->root = node->parent;
    }

    // propagate median up
    int slot =
        node->parent->insert(node->keys[median], node->values[median], comp);
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

  struct BTreeIterator {
    BTreeNode *btree;
    int current_index;

    bool operator==(const BTreeIterator &other) {
      return this->btree == other.btree;
    }

    bool operator!=(const BTreeIterator &other) { return !(*this == other); }

    // Go to the next node in the tree
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

    const std::pair<const K &, const V &> operator*() {
      return {btree->keys[current_index], btree->values[current_index]};
    }
  };

public:

  // Returns the number of elements in the tree
  size_t size() {
      return this->_size;
  }

  void insert(K k, V v) {
    _size++;
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

  using const_iterator_type = BTreeIterator;

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
  const_iterator_type rangeLookUp(const K &key) const {
    auto cur = root;
  lol:
    while (cur != nullptr) {
      for (int i = 0; i < cur->size; i++) {
        if (!comp(cur->keys[i], key)) {
          if (cur->children[i]) {
            cur = cur->children[i];
            goto lol;
          } else {
            return {cur, i};
          }
        }
      }
      if (cur->children[cur->size]) {
        cur = cur->children[cur->size];
        goto lol;
      }
      auto target_index = cur->parent_index;
      auto target = cur->parent;

      while (target &&
             (target_index >= (target->parent ? target->parent->size : 0))) {
        if (!comp(cur->keys[target_index], key)) {
          break;
        }
        target_index = target->parent_index;
        target = target->parent;
      }

      if (!target) {
        return this->end();
      }

      return {target, target_index};
    }

    return this->end();
  }

  // Exact lookup
  // Throws an exception of the key is not present
  const V &operator[](const K &key) const {
    auto lookup = rangeLookUp(key);
    if (lookup == this->end() || (*lookup).first != key) {
      throw std::runtime_error("Invalid index");
    }

    return (*lookup).second;
  }

  ~BTree() { delete root; }
};

#endif
