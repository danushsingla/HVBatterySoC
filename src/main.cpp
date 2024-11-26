#include "BTree.h"
#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <thread>

using namespace std;

void primitive_test() {}

void fuzzy_tests() {
  std::array<int, 100> array;
  for (int i = 0; i < 100; i++) {
    array[i] = i;
  }

  std::random_device rd;
  std::mt19937 g(rd());

  std::shuffle(array.begin(), array.end(), g);

  BTree<int, int> tree;
  try {

    for (int i = 0; i < 100; i++) {
      tree.insert(array[i], array[i]);
    }
  } catch (std::exception e) {
    std::cout << "err\n";
    return;
  }

  std::sort(array.begin(), array.end());

  int i = 0;
  for (auto [k, v] : tree) {
    assert(k == array[i]);
    i++;
  }
}

void specific_test() {
  std::array<int, 100> array = {
      23, 85, 1,  64, 44, 97, 89, 79, 84, 80, 46, 25, 17, 50, 3,  72, 12,
      2,  96, 56, 22, 39, 92, 55, 67, 70, 68, 24, 61, 66, 99, 11, 82, 45,
      62, 26, 33, 57, 81, 74, 42, 90, 27, 71, 86, 43, 20, 91, 21, 37, 53,
      98, 4,  19, 69, 88, 63, 49, 5,  73, 65, 47, 41, 6,  16, 7,  95, 87,
      29, 34, 9,  60, 35, 8,  13, 40, 28, 0,  93, 48, 52, 32, 58, 78, 38,
      14, 10, 31, 36, 18, 15, 51, 94, 30, 75, 77, 83, 54, 76, 59};

  BTree<int, int> tree;
  for (int i = 0; i < 100; i++) {
    tree.insert(array[i], array[i]);
  }

  int i = 0;
  for (auto [k, v] : tree) {
    // std::cout << "E: " << k << ", A: " << i << "\n";
    assert(k == i);
    i++;
  }
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

  for (auto k : tree) {
    std::cout << "Key: " << k.first << std::endl;
  }

  auto lookup = tree.rangeLookUp(21);
  if (lookup != tree.end()) {
      std::cout << "Lookup: " << (*lookup).first << std::endl;
  } else {
      std::cout << "Out of bounds" << std::endl;
  }
}

int main(void) {
  specific_test();
  funner_test();
  for (int i = 0; i < 10; i++) {
    fuzzy_tests();
  }

  return 0;
}
