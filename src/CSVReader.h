#ifndef CSVREADER_H
#define CSVREADER_H

#include <fstream>
#include <optional>
#include <vector>

class CSVReader {
    std::ifstream file;
public:
    CSVReader(const std::string & file_path);
    bool ready();
    void skip();
    std::vector<double> nextDataLine();
    std::vector<std::string> nextStringLine();
};
#endif
