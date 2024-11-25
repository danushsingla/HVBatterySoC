#include "BTree.h"
#include <string>

using namespace std;

void primitive_test() {

}

void funner_test() {
    BTree<int, int> tree;
    tree.insert(8, 8);
    tree.insert(9, 9);
    tree.insert(10, 10);
    tree.insert(11, 11);
    tree.insert(15, 15);
    tree.insert(20, 20);
    tree.insert(17, 17);
}


int main(void) {
    funner_test();

    return 0;
}
