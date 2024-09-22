#include "pma.hpp"
#include <cmath>
#include <iostream>

float PMA::log2(int x) {
  float r = 0;
  while (x >>= 1)
    ++r;
  return r;
}

float PMA::segmentSize() { return log2(arr.capacity()); }

int PMA::search(int value) {
  int low = 0;
  int high = arr.capacity();
  int mid = 0;

  while (low <= high) {
    mid = low + (high - low) / 2;

    while (arr[mid] == gap and mid > low)
      --mid;

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

int PMA::scan(int begin_leaf, int end_leaf) {
    int seg_size = segmentSize();
    int total = 0;
    for (int i = begin_leaf * seg_size, end = end_leaf * seg_size; i < end; ++i) {
        if (arr[i] != gap)   ++total;
    }
    return total;
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

  while (pos < arr.size() and arr[pos] != gap and arr[pos] < value) {
    pos++;
  }

  if (pos < arr.size() and arr[pos] == value)
    return;

  arr.emplace(arr.begin() + pos, value); // ordered insert
    
  int seg_size = seg_size;
  // i e j são índices das folhas, inicialmente é apenas uma folha(onde ocorreu a inserção) então j = i + 1
  // O intervalo de elementos no array será arr[i * segmentSize() ... j * segmentSize() - 1]
  int i = pos / seg_size; // starts in leaf
  int j = i + /* segmentSize() - */ 1;

  //  total number of nodes is N = 2L – 1, where L is the number of leaves
  int h = log2((2 * arr.size() / seg_size) - 1);
  float depth = h;

  float density = (float) scan(i, j) / seg_size;

  return;
  
  //TODO: verificar subida
  // If the D is out of thresholds, need rebalance
  //if (density < 0.25) {
  int num_segs = arr.capacity() / seg_size;
  int node = i;
  do {
    if (node % 2){  // right child, then left scan
      density += (scan(i - j - i, i) / (float) seg_size);
      i -= (j - i);
    }
    else{  // left child, then right scan
      density += (scan(j, j + j - i) / (float) seg_size);
      j += (j - i);
    }
    
    if (depth == 0)     break;
    
    --depth;
    num_segs = num_segs / (j - i);
    node = num_segs / j;
  } while (1.0 / 2.0 - (1.0 / 4.0 * depth / h) > density and density >  3.0 / 4.0 + (1.0 / 4.0 * depth / h));

  if (depth != 0)
    rebalance(i, j);
//}

}

void PMA::rebalance(int begin_leaf, int end_leaf) {
    int num_elements = 0;
    int number_gaps = 0;
    int seg_size = segmentSize();
    int max_elements = (end_leaf - begin_leaf) * seg_size;
    
    // TODO: duplicar array(quando necessário...), algo assim:
    //arr.reserve(arr.capacity() * 2);
    //begin_leaf *= 2;
    //end_leaf *= 2;

    int *elements = new int[max_elements];
    for (int i = begin_leaf * seg_size, end = end_leaf * seg_size; i < end; ++i) {
        if (arr[i] == gap)   ++number_gaps;
        else {
            elements[num_elements++] = arr[i];
            arr[i] = gap;
        }
    }
    
    if (num_elements == 1) {
        arr[number_gaps / 2] = elements[0];
    }
    else {
        // minimum space between elements
        int m = (max_elements - num_elements) / (num_elements - 1);
        // extra spaces to distribute
        int extra_spaces = (max_elements - num_elements) % (num_elements - 1);
        
        for (int i = 0, index = begin_leaf * seg_size; i < num_elements; ++i) {
            arr[index] = elements[i];
            index += m + 1;
            if (i < extra_spaces)   ++index;
        }
    }

    delete[] elements;
}


void PMA::print_debug() {
  std::cout << "[ ";
  for (int i = 0; i < arr.capacity(); ++i) {
    if (arr[i] == gap) {
      std::cout << "/ ";
      continue;
    }
    std::cout << arr[i] << ' ';
  }
  std::cout << "]\n";
}
