#include <cstddef>
#include <stdexcept>
#include <vector>

class Darray {
    int *arr;
    size_t n;
    int default_value;
public:
    Darray(int default_value) : n {0}, default_value{default_value} {
        arr = nullptr;
    }

    Darray(size_t n, int default_value) : n {n}, default_value{default_value} {
        arr = new int[n];
        for (size_t i = 0; i < n; ++i)  arr[i] = default_value;
    }

    Darray(std::vector<int> &arr_cp, int default_value) : default_value{default_value} {
        n = arr_cp.capacity();
        arr = new int[n];
        for (size_t i = 0; i < n; ++i)  arr[i] = arr_cp[i];
    }

    void reserve(size_t new_size) {
        int *new_arr = new int[new_size];
        for (size_t i = 0; i < n and i < new_size; ++i)  new_arr[i] = arr[i];
        for (size_t i = n; i < new_size; ++i)  new_arr[i] = default_value;
        n = new_size;
        delete[] arr;
        arr = new_arr;
    }
    
    size_t capacity() {
        return n;
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

