#include "BTree.h"
#include "CSVReader.h"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
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

template <typename F> uint64_t timeit(F f) {
  auto start = std::chrono::high_resolution_clock::now();
  f();
  auto end = std::chrono::high_resolution_clock::now();

  auto diff = chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  return diff.count();
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

  auto lookup = tree.rangeLookUp(13);
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

  CSVReader reader("data/test_export.csv");
  reader.nextStringLine();
  int i = 0;
  std::vector<std::vector<double>> raw_lines;
  while (reader.ready()) {
    auto line = reader.nextDataLine();
    if (!line.empty()) {
      raw_lines.push_back(line);
    }
  }

  uint64_t avg = 0.0;
  for (int i = 0; i < 100; i++) {
    avg += timeit([&]() {
      BTree<double, std::vector<double>> tree;
      for (auto &line : raw_lines) {
        tree.insert(line[0], line);
        i++;
      }
    });
  }
  avg /= 10;

  std::cout << "Inserted " << i << " lines of data into a BTree of order 3 in "
            << avg << " ns.\n";

  BTree<double, std::vector<double>, 11, std::less<double>> tree2;

  // diff = timeit([&]() {
  //         auto v = tree.rangeLookUp(3.3);
  //         auto value = *v;
  //         assert(value.first >= 3.3);
  //         });
  // std::cout << "Lookup time is in " << diff << " ns for BTree of order 3.\n";

  // diff = timeit([&]() {
  //   i = 0;
  //   for (auto &line : raw_lines) {
  //     tree2.insert(line[0], line);
  //     i++;
  //   }
  // });
  //

  avg = 0.0;
  for (int i = 0; i < 100; i++) {
    avg += timeit([&]() {
      BTree<double, std::vector<double>, 11, std::less<double>> tree;
      for (auto &line : raw_lines) {
        tree.insert(line[0], line);
        i++;
      }
    });
  }
  avg /= 10;

  std::cout << "Inserted " << i << " lines of data into a BTree of order 11 in " << avg << " ns.\n";

  char c;
  std::cin >> c;

  return 0;
}
