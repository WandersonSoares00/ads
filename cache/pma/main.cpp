#include "pma.hpp"

int main() {
    PMA pma_arr;
    
    for (int i = 0; i < 10; ++i) {
        pma_arr.insert(i);
        pma_arr.print_debug();
    }
}
