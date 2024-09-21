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

float PMA::density(int begin_leaf, int end_leaf) {
  float total = 0;

  for (int i = begin_leaf, end = end_leaf; i <= end; ++i) {
    if (arr[i] != gap)
      ++total;
  }
  auto val = total / float((end_leaf - begin_leaf + 1));
  return val;
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
    arr.emplace(arr.begin(), gap);
    return;
  }

  int pos = search(value);

  while (pos < arr.size() and arr[pos] != gap and arr[pos] < value) {
    pos++;
  }

  if (pos < arr.size() and arr[pos] == value)
    return;

  arr.emplace(arr.begin() + pos, value); // ordered insert

  int i = pos / segmentSize(); // starts in leaf
  int j = i + segmentSize() - 1;

  //  total number of nodes is N = 2L – 1, where L is the number of leaves
  int h = log2((2 * arr.size() / segmentSize()) - 1);

  float d = density(i, j);

  return;

  // If the D is out of thresholds, need rebalance
  if (1.0 / 2.0 - (1.0 / 4.0 * d / h) > d or
      d > 3.0 / 4.0 + (1.0 / 4.0 * d / h)) {
    do {
      if (i % 2) { // right child, then left scan
        i += (j - i);
        d += density(j - i, i);
      } else { // left child, then right scan
        j += (j - i);
        d += density(j - i, j);
      }
    } while (1.0 / 2.0 - (1.0 / 4.0 * d / h) <= d and
             d <= 3.0 / 4.0 + (1.0 / 4.0 * d / h));

    rebalance(i, j);
  }
}

void PMA::rebalance(int begin_leaf, int end_leaf) { return; }

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
