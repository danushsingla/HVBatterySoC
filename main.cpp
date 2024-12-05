#include <iostream>
#include <vector>
#include <string>
#include "CSVReader.h"
#include "CSVReader.cpp"
#include <fstream>
#include "RedBlackTree.hpp"
using namespace std;

int main()
{
  CSVReader reader("./full_project_data.csv");
  reader.nextStringLine();
  int i = 0;
  cout << "Before while loop" << endl;
  std::vector<std::vector<double>> raw_lines;
  while (reader.ready()) {
    auto line = reader.nextDataLine();
    //cout << "Reading line: " << line.size() << " elements." << endl;
    if(line.size() == 0) break;
    if (!line.empty()) {
      raw_lines.push_back(line);
    }
  }
  cout << "Creating tree" << endl;

  RedBlackTree rb_tree;

  int count = 0;
  for (auto line : raw_lines) {
    cout << count << endl;
    if(count >= 10) break;
    count++;
      rb_tree.insert(
          std::vector<double>(line.begin() + 1, line.begin() + 1 + 96),
          line[line.size() - 1], line[0]);
    }
    cout << "Printing tree" << endl;
    rb_tree.print();
    return 0;
}