#include <cstddef>
#include <stdexcept>

class Darray {
    int *arr;
    size_t n;
    int default_value;
public:

    Darray(size_t n, int default_value) : n {n}, default_value{default_value} {
        arr = new int[n];
        for (size_t i = 0; i < n; ++i)  arr[i] = default_value;
    }

    void dupCapacity() {
        int *new_arr = new int[2 * n];
        for (size_t i = 0; i < n; ++i)  new_arr[i] = arr[i];
        n *= 2;
        for (size_t i = n / 2; i < n; ++i)  arr[i] = default_value;
        delete[] arr;
        arr = new_arr;
    }

    int& operator[](size_t index) {
        if (index >= n) {
            throw std::out_of_range("Index out of range");
        }
        return arr[index];
    }

    const int& operator[](size_t index) const {
        if (index >= n) {
            throw std::out_of_range("Index out of range");
        }
        return arr[index];
    }

    ~Darray() { delete[] arr; }
};

