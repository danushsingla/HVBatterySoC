#define ENABLE_ASSERTS
#include "BTree.hpp"
#include "CSVReader.h"
#include "RedBlackTree.hpp"
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

constexpr double THEORETICAL_CAPACITY = 2.52e7;


template <typename F> uint64_t timeit(F f) {
  auto start = std::chrono::high_resolution_clock::now();
  f();
  auto end = std::chrono::high_resolution_clock::now();

  auto diff = chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  return diff.count();
}


template <const int N>
double changeInSOC(const BTree<double, std::vector<double>, N> &iter,
                   double start = 0, double end = 100000) {
  double soc = 0.0;
  double before = 0.0;
  int i = 0;

  auto iterator = iter.begin();
  if (start != 0) {
    auto time = timeit([&]() { iterator = iter.rangeLookUp(start); });
    std::cout << "Time to find initial BTree node: " << time << "ns\n";
  }
  auto calculation_time = timeit([&]() {
    for (; iterator != iter.end(); ++iterator) {
      auto &pair = *iterator;
      double now = pair.first;
      if (now >= end) {
        break;
      }
      double current = pair.second[pair.second.size() - 1];
      double voltage = 0.0;
      for (int i = 1; i < pair.second.size() - 1; i++) {
        voltage += pair.second[i];
      }
      // -1.0 factor is because a negative current reading is charging, a
      // positive current reading is discharge
      soc += -1.0 * (now - before) * current * voltage;
      before = now;
      i += 1;
    }
  });
  std::cout << "BTree SOC calculation algorithm time: " << calculation_time
            << "ns\n";
  return soc / THEORETICAL_CAPACITY;
}

double rbtreesoc(RedBlackTree &tree, double start = 0, double end = 100000) {
  int i = 0;
  double soc = 0.0;
  double before = 0.0;
  auto iter = tree.begin();
  if (start != 0) {
    auto time = timeit([&]() { iter = tree.search_range(start); });
    std::cout << "Time to find initial Red-Black Tree node: " << time << "ns\n";
  }
  auto calculation_time = timeit([&]() {
    for (; iter != tree.end(); ++iter) {
      auto &node = *iter;
      double now = node.time;
      if (now >= end) {
        break;
      }
      double current = node.current;
      double voltage = 0.0;
      for (int i = 0; i < node.voltages.size(); i++) {
        voltage += node.voltages[i];
      }
      // std::cout << "Voltage: " << voltage << " ";
      // std::cout << "Current: " << current << "\n";

      // -1.0 factor is because a negative current reading is charging, a
      // positive current reading is discharge
      soc += -1.0 * (now - before) * current * voltage;
      before = now;
      i++;
    }
  });
  std::cout << "Red-Black Tree SOC calculation algorithm time: " << calculation_time
            << "ns\n";
  return soc / THEORETICAL_CAPACITY;
}

int main(void) {
  std::string path;
  std::cout << "Enter path of battery data csv: ";
  std::cin >> path;
  std::cout << "\n";

  std::string raw_start_time;
  std::cout << "Enter start time (enter \"begin\" to start at beginning): ";
  std::cin >> raw_start_time;
  std::cout << "\n";
  double start = 0.0;
  try {
    start = stof(raw_start_time);
  } catch (std::exception e) {
  }

  std::string raw_end_time;
  std::cout << "Enter end time (enter \"end\" to finish at end): ";
  std::cin >> raw_end_time;
  std::cout << "\n";
  double end = 0.0;
  try {
    end = stof(raw_end_time);
  } catch (std::exception e) {
  }

  CSVReader reader(path);
  reader.nextStringLine();
  int i = 0;
  std::vector<std::vector<double>> raw_lines;
  while (reader.ready()) {
    auto line = reader.nextDataLine();
    if (!line.empty()) {
      raw_lines.push_back(line);
    }
  }

  BTree<double, std::vector<double>> tree;
  auto btree_build_time_ns = timeit([&]() {
    for (auto line : raw_lines) {
      tree.insert(line[0], line);
    }
  });

  double res = changeInSOC(tree, start, end);

  RedBlackTree rb_tree;

  auto rbtree_build_time_ns = timeit([&]() {
    for (auto line : raw_lines) {
      rb_tree.insert(
          std::vector<double>(line.begin() + 1, line.begin() + 1 + 96),
          line[line.size() - 1], line[0]);
    }
  });

  double rbtree_res = rbtreesoc(rb_tree, start, end);

  std::cout << "BTree SOC Change Result: " << res * 100.0 << "%" << "\n";
  std::cout << "BTree Build Time: " << (double)btree_build_time_ns / 1e9 << "s"
            << "\n";
  std::cout << "Red-Black Tree SOC Change Result: " << rbtree_res * 100.0 << "%"
            << "\n";
  std::cout << "Red-Black Build Time: " << (double)rbtree_build_time_ns / 1e9
            << "s" << "\n";

  return 0;
}
