#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <sstream>
#include <limits>
#include "rbtree/rbtree.cpp"

std::vector<std::tuple<std::string,int,int>> tokenize(const char *filename);

int main(int argc, char **argv) {
    if (argc < 2)       exit(1);
    const char *fin  = argv[1];
    std::string fout;
    
    if (argc >= 3)  fout = argv[2];
    else            fout = "out.txt";

    std::ofstream ofile(fout, std::ios::trunc);

    if (!ofile.is_open() or ofile.fail()) {
        std::cerr << "could not open the file " << fin << '\n';
        exit(1);
    }

    auto operations = tokenize(fin);
    RBTree tree;

    for (const auto& op : operations) {
        auto inst = std::get<0>(op);
        auto v1   = std::get<1>(op);
        auto v2   = std::get<2>(op);

        if (inst      == "INC") {
            tree.insert(v1);
            //tree.printtree();
        }
        else if (inst == "REM")
            tree.remove(v1);
        else if (inst == "SUC") {
            int suc = tree.successor(v1, v2);
            ofile << inst << ' ' << v1 << ' ' << v2 << '\n';
            if (suc == std::numeric_limits<int>::lowest())
                ofile << "-\n";
            else if (suc == std::numeric_limits<int>::max())
                ofile << "∞\n";
            else
                ofile << suc << '\n';
        }
        else if (inst == "IMP") {
            ofile << inst << ' ' << v2 << '\n';
            ofile << tree.inorder(v2) << '\n';
        }
    }
}


std::vector<std::tuple<std::string,int,int>> tokenize(const char *filename) {
    std::vector<std::tuple<std::string,int,int>> ops;

    std::ifstream infile(filename);
    
    if (!infile.is_open() or infile.fail()) {
        std::cerr << "could not open the file " << filename << '\n';
        exit(1);
    }
    
    std::string line, word, operation;
    std::stringstream ss;

    int value = 0, version = -1;

    while (std::getline(infile, line)) {
        ss.str("");
        ss.clear();
        ss << line;

        ss >> operation;

        if (operation == "INC" or operation == "REM" or operation == "SUC")
            ss >> value;
        
        if (operation == "SUC" or operation == "IMP")
            ss >> version;
        
        ops.push_back(std::make_tuple(operation, value, version));
    }
    return ops;
}

