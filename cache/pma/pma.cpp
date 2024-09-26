#include "pma.hpp"
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <iostream>

int PMA::log2(int x) {
  int r = 0;
  while (x >>= 1)
    ++r;
  return r;
}

int PMA::segmentSize() {
  int seg = log2(arr.capacity());
  if (seg == 1)
    return 1;
  if (seg & 1)
    return seg - 1;
  return seg;
}

int PMA::numberSegments() { return arr.capacity() / segmentSize(); }

bool PMA::insideThreshold(float density, size_t depth, size_t height) {
  float razao;
  if (height == 0) {
    razao = 0;
  } else {
    razao = (float)depth / height;
  }
  printf("<><><> low(%.2f) <= density(%.2f)  <= high(%.2f)\n",
         1.0 / 2.0 - (1.0 / 4.0 * razao), density,
         3.0 / 4.0 + (1.0 / 4.0 * razao));

  return (density >= 1.0 / 2.0 - (1.0 / 4.0 * razao) and
          density <= 3.0 / 4.0 + (1.0 / 4.0 * razao));
}

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

  // __AUTO_GENERATED_PRINT_VAR_START__
  printf("\t\tPMA::scan: (%d, %d)  Seg_size: %d\n", begin_leaf, end_leaf,
         seg_size);

  // __AUTO_GENERATED_PRINT_VAR_END__
  for (int i = begin_leaf * seg_size, end = end_leaf * seg_size; i < end; ++i) {
    if (arr[i] != gap)
      ++total;
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
  int pos = search(value);

  while (pos < arr.capacity() and arr[pos] != gap and arr[pos] < value) {
    pos++;
  }

  arr.emplace(arr.begin() + pos, value); // ordered insert
  print_debug();

  int seg_size = segmentSize();
  // i e j são índices das folhas, inicialmente é apenas uma folha(onde ocorreu
  // a inserção) então j = i + 1. O intervalo de elementos no array será arr[i *
  // segmentSize() ... j * segmentSize() - 1]
  int begin_leaf = pos / seg_size; // starts in leaf
  int end_leaf = begin_leaf + 1;

  //  total number of nodes is N = 2L – 1, where L is the number of leaves
  int height = log2((2 * arr.capacity() / numberSegments()) - 1);
  int depth = height;

  float density = (float)scan(begin_leaf, end_leaf) / seg_size;

  int node = begin_leaf;
  while (depth > 0 and not insideThreshold(density, depth, height)) {
    if (node % 2) { // right child, then left scan
      density += (scan(begin_leaf - (end_leaf - begin_leaf), begin_leaf) /
                  (float)(seg_size * (end_leaf - begin_leaf)));
      begin_leaf -= (end_leaf - begin_leaf);
    } else { // left child, then right scan
      density += (scan(end_leaf, end_leaf + (end_leaf - begin_leaf)) /
                  (float)(seg_size * (end_leaf - begin_leaf)));
      end_leaf += (end_leaf - begin_leaf);
    }

    density /= 2.0;

    node = node >> 1;
    depth--;
  };

  printf("<><><> depth %d (%d, %d)\n", depth, begin_leaf, end_leaf);

  //  If the D is out of thresholds, need rebalance
  if (not insideThreshold(density, depth, height)) {
    printf("<><><> OUT OF THRESHOLD\n");
    return;

    // ATE AQUI TA TUDO OK !!!!! @wanderson
    rebalance(begin_leaf, end_leaf, depth);
  }
}

void PMA::rebalance(int begin_leaf, int end_leaf, int depth) {
  int num_elements = 0;
  int number_gaps = 0;
  int seg_size = segmentSize();

  int max_elements = (end_leaf - begin_leaf) * seg_size;
  // __AUTO_GENERATED_PRINT_VAR_START__
  printf("PMA::rebalance max_elements: %d | seg_size: %d (%d, %d) \n",
         max_elements, seg_size, begin_leaf, end_leaf);
  int *elements = new int[max_elements];
  for (int i = begin_leaf * seg_size, end = end_leaf * seg_size; i < end; ++i) {
    if (arr[i] == gap)
      ++number_gaps;
    else {
      elements[num_elements++] = arr[i];
      // arr[i] = gap;
    }
  }
  // print Elements
  std::cout << "Save Elements: \n[ ";
  for (int i = 0; i < num_elements; ++i) {
    std::cout << elements[i] << ' ';
  }
  std::cout << "]\n";

  if (depth == 0) {
    int num_segs = numberSegments();
    arr.reserve(2 * arr.capacity());
    begin_leaf = 0;
    if (num_segs < numberSegments())
      end_leaf *= 2;
    printf("---------- double -----------\n");
  }
  seg_size = segmentSize();

  printf("gaps: %d | elements: %d | max_elements: %d | seg_size: %d \n",
         number_gaps, num_elements, max_elements, seg_size);

  if (num_elements == 1) {
    arr[number_gaps / 2] = elements[0];
  } else {
    // minimum space between elements
    int m = (max_elements - num_elements) / (num_elements - 1);
    // extra spaces to distribute
    int extra_spaces = (max_elements - num_elements) % (num_elements - 1);

    for (int i = 0, index = begin_leaf * seg_size; i < num_elements; ++i) {
      arr[index] = elements[i];
      index += m + 1;
      if (i < extra_spaces)
        ++index;
    }
  }

  print_debug();
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
