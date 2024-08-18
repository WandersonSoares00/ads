#pragma once

#include <vector>

//namespace PMA {

constexpr int gap = -1;

class PMA {
    
    std::vector<int> arr;

    public:
    int density(int begin_leaf, int end_leaf);
    
    void rebalance(int begin_leaf, int end_leaf);
    
    int log2(int x);

    inline int segmentSize();

    int search(int value);

    void insert(int value);
    
    void print_debug();
    //void remove(int value);
};

//}
