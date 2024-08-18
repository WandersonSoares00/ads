#include "pma.hpp"
#include <iostream>

int PMA::log2(int x) {
    int r = 0;
    while (x >>= 1)  ++r;
    return r;
}

int PMA::segmentSize() {
    return log2(arr.size());
}

int PMA::search(int value) {
    int low  = 0;
    int high = arr.size();
    int mid = 0;
    
    while (low <= high) {
        mid = low + (high - low) / 2;

        while (arr[mid] == gap and mid >= 0) --mid;

        if (arr[mid] == value or arr[mid] == gap) {
            break;
        }

        if (value > arr[mid])
            low = mid + 1;
        else
            high = mid - 1;
    }

    return mid;
}

int PMA::density(int begin_leaf, int end_leaf) {
    int seg_size = segmentSize();
    int total = 0;
    for (int i = begin_leaf * seg_size, end = (end_leaf * seg_size) + seg_size; i < end; ++i) {
        if (arr[i] != gap)   ++total;
    }
    return total / (end_leaf - begin_leaf + 1) * seg_size;
}

/*
Insert(x, y, z):
    insert x into the same chunk as y (or z) using naıve insertion -- scan
    node ← leaf node representing chunk
    if density(node) ∈/ thresholds(node):
        while density(node) ∈/ thresholds(node):
            node ← parent(node)
        rebalance node by evenly redistributing all elements in the interval
 * */

void PMA::insert(int value) {
    if (arr.empty()) {
        arr.emplace_back(value);
        return;
    }

    int pos = search(value);
    
    arr.emplace(arr.begin() + pos, value); // ordered insert
    
    int i = pos / segmentSize(); // starts in leaf
    int j = i + 1;

    //  total number of nodes is N = 2L – 1, where L is the number of leaves
    int h = log2((2 * arr.size() / segmentSize()) - 1);
    
    int d = density(i, i);

    if (1 / 2 - (1 / 4 * d / h) <= d and d <=  3 / 4 + (1 / 4 * d / h)) {
        do {
            if (i % 2){  // right child, then left scan
                i += (j - i); 
                d += density(j - i, i);
            }
            else{  // left child, then right scan
                j += (j - i); 
                d += density(j - i, j);
            }
        } while (1 / 2 - (1 / 4 * d / h) <= d and d <=  3 / 4 + (1 / 4 * d / h));
        
        rebalance(i, j);
    }
}

void PMA::rebalance(int begin_leaf, int end_leaf) {
    return;
}

void PMA::print_debug() {
    std::cout << "[ ";
    for (auto e : arr)
        std::cout << e << ' ';
    std::cout << "]\n";
}


