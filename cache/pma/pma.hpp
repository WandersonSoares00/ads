#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

template <typename T> struct PMAAllocator {
  using value_type = T;
  T default_value;

  PMAAllocator(T val) : default_value(val) {}

  template <class U> PMAAllocator(const PMAAllocator<U> &) {}

  T *allocate(std::size_t n) {
    if (n <= std::numeric_limits<std::size_t>::max() / sizeof(T)) {
      if (auto ptr = static_cast<T *>(operator new(n * sizeof(T)))) {
        std::fill(ptr, ptr + n, default_value);
        return ptr;
      }
    }
    throw std::bad_alloc();
  }
  void deallocate(T *ptr, std::size_t n) { delete[] ptr; }

  ~PMAAllocator() = default;
};

// namespace PMA {

constexpr int gap = -1;

/**
 * @class PMA
 * @brief Packed Memory Array (PMA) class.
 *
 * The PMA class represents a Packed Memory Array, which is a data structure
 * used for efficient storage and retrieval of values. It provides methods for
 * calculating density, rebalancing, logarithm base 2 calculation, segment size
 * calculation, searching, and insertion. The PMA class does not support value
 * removal.
 */
class PMA {

  std::vector<int, PMAAllocator<int>> arr;

  /**
   * Calculates the logarithm base 2 of the given integer.
   *
   * @param x The integer for which to calculate the logarithm base 2.
   * @return The logarithm base 2 of the given integer.
   */
  int log2(int x);

  /**
   * Calculates the segment size of the PMA.
   *
   * @return The segment size of the PMA.
   */
  inline int segmentSize();

  inline int numberSegments();

  /**
   * Calculates the density of non-gap elements in the specified range of leaf
   * nodes.
   *
   * @param begin_leaf The index of the first leaf node in the range.
   * @param end_leaf The index of the last leaf node in the range.
   * @return The density of non-gap elements in the specified range.
   */
  int scan(int begin_leaf, int end_leaf);

  /**
   * @brief Rebalances the PMA tree within the specified range of leaf nodes.
   *
   * This function is responsible for rebalancing the PMA tree by redistributing
   * the keys and values within the specified range of leaf nodes. The
   * rebalancing process aims to maintain a balanced tree structure, which
   * improves the overall performance of the PMA data structure.
   *
   * @param begin_leaf The index of the first leaf node in the range.
   * @param end_leaf The index of the last leaf node in the range.
   */
  void rebalance(int begin_leaf, int end_leaf, int depth);

  /**
   * @brief Checks if the given density, depth, and height values are within an
   * acceptable threshold.
   *
   * This function returns a boolean indicating whether the provided parameters
   * (density, depth, and height) fall within a predefined threshold or
   * acceptable range of values. It is useful for validating measurements
   * against certain criteria, such as in physical simulations or other areas
   * involving these parameters.
   *
   * @param density The density value (float). Represents the density of a
   * material or object.
   * @param depth The depth value (size_t). Typically represents the depth in
   * units of measurement.
   * @param height The height value (size_t). Represents the height in units of
   * measurement.
   *
   * @return true if the values are within the acceptable threshold; false
   * otherwise.
   */
  inline bool insideThreshold(float density, size_t depth, size_t height);

public:
  PMA() : arr(PMAAllocator<int>(gap)) {
    arr.reserve(2); /* arr[0] = gap; arr[1] = gap; */
  }

  /**
   * @brief Searches for a value in the PMA array.
   *
   * This function searches for a given value in the PMA array and returns the
   * location of the element if found, or the position where the element should
   * be inserted if not found.
   *
   * @param value The value to search for in the PMA array.
   * @return The location of the element if found, or the position where the
   * element should be inserted if not found.
   */
  int search(int value);

  /**
   * Inserts a value into the PMA.
   *
   * @param value The value to be inserted.
   */
  void insert(int value);

  void print_debug();
  // void remove(int value);
};

//}
