#include "functions.hpp"
#include <iostream>

extern "C" {
    char* translation(long x) {
        char* arr = new char[100];
        unsigned long i = 0;
        while (x > 0) {
            arr[i++] = (x % 3) + '0';
            x /= 3;
        }

        const unsigned long size = i + 1;
        char* result = new char[size];
        for (int j = 0; j < size - 1; ++j) {
            result[j] = arr[--i];
        }
        result[size - 1] = '\0';
        delete [] arr;
        return result;
    }
}

extern "C" {
    void _sort(int* arr, int first, int last) {
        int i = first, j = last, pivot = arr[(first + last) / 2];
        do {
            while (arr[i] < pivot)
                i++;
            while (arr[j] > pivot)
                j--;
    
            if(i <= j) {
                if (arr[i] > arr[j]) {
                    int tmp = arr[i];
                    arr[i] = arr[j];
                    arr[j] = tmp;
                }
                i++;
                j--;
            }
        } while (i <= j);
    
        if (i < last)
            _sort(arr, i, last);
        if (first < j)
            _sort(arr, first, j);
    }

    int* sort(int* array, const unsigned long& size) {
        int* result = new int[size];
        for (int i = 0; i < size; ++i) {
            result[i] = array[i];
        }

        _sort(result, 0, size - 1);

        return result;
    }

}
