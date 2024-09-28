#include "../pma.hpp"
#include <vector>
#include <cstdio>

void test_search(PMA &pma_arr, int value, int expected)
{
  int result = pma_arr.search(value);
  if (result == expected)
  {
    printf("search(%d): correto, retornou %d\n", value, result);
  }
  else
  {
    printf("search(%d): esperava %d - veio %d\n", value, expected, result);
  }
}

int main()
{
  std::vector<int> vec = {0, -1, -1, 1, 3};
  PMA pma_arr(vec);

  test_search(pma_arr, 2, 4);  // Teste original
  test_search(pma_arr, -2, 0); // Valor menor que todos
  test_search(pma_arr, 5, 5);  // Valor maior que todos
  test_search(pma_arr, 0, 0);  // Igual a um elemento existente
  test_search(pma_arr, 1, 3);  // Igual a um elemento existente

  return 0;
}
