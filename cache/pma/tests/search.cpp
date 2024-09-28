#include "../pma.hpp"
#include <vector>

int main() {
  std::vector<int> vec = { 0, -1, -1, 1 };
  PMA pma_arr(vec);
  
  int r = pma_arr.search(2);
  if(r == 4) {
    printf("correto\n");
  }
  else
      printf("esperava %d - veio %d\n", 4, r);
}
