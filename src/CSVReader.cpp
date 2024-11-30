#include "CSVReader.h"
#include <fstream>
#include <string>

CSVReader::CSVReader(const std::string &file_path) : file(file_path) {}

bool CSVReader::ready() { return file.good() && !file.eof(); }

void CSVReader::skip() {
  std::string buf;
  std::getline(file, buf);
}

std::vector<double> CSVReader::nextDataLine() {
  std::string line;
  std::getline(file, line);

  std::vector<double> outputs;
  while (!line.empty()) {
    int end = 0;
    while (end < line.size() && line[end] != ',') {
      ++end;
    }
    std::string datum = line.substr(0, end);
    if (end < line.size()) {
      end++;
    }
    line.erase(0, end);

    outputs.push_back(std::stof(datum));
  }
  return outputs;
}

std::vector<std::string> CSVReader::nextStringLine() {
  std::string line;
  std::getline(file, line);

  std::vector<std::string> outputs;
  while (!line.empty()) {
    int end = 0;
    while (end < line.size() && line[end] != ',') {
      ++end;
    }
    std::string datum = line.substr(0, end);
    if (end < line.size()) {
      end++;
    }
    line.erase(0, end);

    outputs.push_back(datum);
  }
  return outputs;
}
