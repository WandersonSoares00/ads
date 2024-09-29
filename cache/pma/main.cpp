#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <sstream>
#include <limits>
#include "pma.hpp"

std::vector<std::tuple<std::string,int>> tokenize(const char *filename);

int main(int argc, char **argv) {
    if (argc < 2)       exit(1);
    const char *fin  = argv[1];
    std::string fout;
    
    if (argc >= 3)  fout = argv[2];
    else            fout = "out.txt";

    std::ofstream ofile(fout, std::ios::trunc);

    if (!ofile.is_open() or ofile.fail()) {
        std::cerr << "could not open the file" << fin << '\n';
        exit(1);
    }

    auto operations = tokenize(fin);
    PMA pma_arr;

    for (const auto& op : operations) {
        auto inst = std::get<0>(op);
        auto v1   = std::get<1>(op);

        if (inst      == "INC") {
            pma_arr.insert(v1);
        }
        else if (inst == "REM")
            pma_arr.remove(v1);
        else if (inst == "SUC") {
            int suc = pma_arr.successor(v1);
            ofile << suc << '\n';
        }
        else if (inst == "IMP") {
            ofile << pma_arr.print() << '\n';
        }
    }
}


std::vector<std::tuple<std::string,int>> tokenize(const char *filename) {
    std::vector<std::tuple<std::string,int>> ops;

    std::ifstream infile(filename);
    
    if (!infile.is_open() or infile.fail()) {
        std::cerr << "could not open the file" << filename << '\n';
        exit(1);
    }
    
    std::string line, word, operation;
    std::stringstream ss;

    int value = 0;

    while (std::getline(infile, line)) {
        ss.str("");
        ss.clear();
        ss << line;

        ss >> operation;

        if (operation == "INC" or operation == "REM" or operation == "SUC")
            ss >> value;
        
        ops.push_back(std::make_tuple(operation, value));
    }
    return ops;
}

