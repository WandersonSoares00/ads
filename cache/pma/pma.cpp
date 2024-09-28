#include "pma.hpp"
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <bit>

int PMA::log2(int x) {
  int r = 0;
  while (x >>= 1)
    ++r;
  return r;
}

int PMA::segmentSize() {
  int seg = log2(arr.capacity());
  return (int) std::bit_ceil(static_cast<unsigned int>(seg));
}

int PMA::numberSegments() { return arr.capacity() / segmentSize(); }

int PMA::insideThreshold(float density, size_t depth, size_t height) {
  float razao;
  if (height == 0) {
    razao = 0;
  } else {
    razao = (float)depth / height;
  }

  bool inside_low  = (density >= 1.0 / 2.0 - (1.0 / 4.0 * razao));
  bool inside_high = (density <= 3.0 / 4.0 + (1.0 / 4.0 * razao));

  if (inside_low and inside_high)   return 0;
  if (inside_low)                   return 1;
  if (inside_high)                  return 2;
  return 3;
}

int PMA::search(int value) {
    int low = 0;
    int high = arr.capacity() - 1;
    
    while (low <= high) {
        int mid = low + (high - low) / 2;

        if (arr[mid] == gap) {
            int left = mid - 1;
            int right = mid + 1;

            while (left >= low && arr[left] == -1) left--;
            while (right <= high && arr[right] == -1) right++;

            if (left < low && right > high) {
                break;
            }

            if (left >= low && (right > high || arr[left] >= value)) {
                mid = left;
            } else {
                mid = right;
            }
        }

        if (arr[mid] == value) {
            return mid;
        } else if (arr[mid] < value) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    
    return low;
}

void PMA::shift(int pos) {
    int i = pos;
    int n = arr.capacity();

    while (i < n and arr[i] != gap) ++i;

    if (i == n and arr[i-1] != gap) {
        n = 2 * n;
        arr.reserve(n);
        while (i < n and arr[i] != gap) ++i;
    }

    for (int j = i; j > pos; --j) {
        arr[j] = arr[j - 1];
    }
    arr[pos] = gap;
}

int PMA::scan(int begin_leaf, int end_leaf) {
  int seg_size = segmentSize();
  int total = 0;

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

  if (pos != arr.capacity()) {
    if (arr[pos] == value)  return;
    if (arr[pos] != gap)    shift(pos);
  }
  else {
    arr.reserve(arr.capacity() * 2);
  }

  arr[pos] = value;
    
  if (arr.capacity() == 1) {
    return;
  }

  int seg_size = segmentSize();
  int begin_leaf = pos / seg_size; // starts in leaf
  int end_leaf = begin_leaf + 1;
  
  //  total number of nodes is N = 2L – 1, where L is the number of leaves
  int height = log2((2 * numberSegments()) - 1);
  int depth = height;
  
  float density = (float)scan(begin_leaf, end_leaf) / seg_size;

  int node = begin_leaf;
  do {
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

  } while (depth > 0 and !insideThreshold(density, depth, height));

  //  If the D is out of thresholds, need rebalance
  if (insideThreshold(density, depth, height) != 0) {
    rebalance(begin_leaf, end_leaf, depth, density);
  }
}

void PMA::rebalance(int begin_leaf, int end_leaf, int depth, int density) {
  int num_elements = 0;
  int number_gaps = 0;

  if (depth == 0 and insideThreshold(density, 0, 0) == 1) {
    int num_segs = numberSegments();
    arr.reserve(2 * arr.capacity());
    begin_leaf = 0;
    if (num_segs < numberSegments())
      end_leaf *= 2;
  }

  int seg_size = segmentSize();
  int max_elements = (end_leaf - begin_leaf) * seg_size;
  // __AUTO_GENERATED_PRINT_VAR_START__
  //fprintf(stderr, "PMA::rebalance max_elements: %d | seg_size: %d (%d, %d) \n",max_elements, seg_size, begin_leaf, end_leaf);

  int *elements = new int[max_elements];
  for (int i = begin_leaf * seg_size, end = end_leaf * seg_size; i < end; ++i) {
    if (arr[i] == gap)
      ++number_gaps;
    else {
      elements[num_elements++] = arr[i];
      arr[i] = gap;
    }
  }

  seg_size = segmentSize();

  if (num_elements == 1) {
    arr[(begin_leaf * seg_size) + (number_gaps / 2)] = elements[0];
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

  delete[] elements;
}

void PMA::print_debug(bool print_gap) {
  std::cout << "[ ";
  for (int i = 0; i < arr.capacity(); ++i) {
    if (arr[i] == gap) {
      if (print_gap)    std::cout << "/ ";
      continue;
    }
    std::cout << arr[i] << ' ';
  }
  std::cout << "]\n";
}

