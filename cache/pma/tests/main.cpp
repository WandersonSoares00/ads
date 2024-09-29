#include "pma.hpp"
#include <random>

int main() {
  PMA pma_arr;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(1, 100);

  for (int i = 0; i < 10; ++i) {
    int v = distrib(gen);
    printf("%d - insert %d:\n", i+1, v);
    pma_arr.insert(v);
    pma_arr.print_debug();
  }
}
