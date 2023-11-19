#include "functions.hpp"
extern "C" {
    char* translation(long x) {
        char* arr = new char[100];
        unsigned long i = 0;
        while (x > 0) {
            arr[i++] = (x % 2) + '0';
            x /= 2;
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
    int* sort(int* array, const unsigned long& size) {
        int* result = new int[size];
        for (int i = 0; i < size; ++i) {
            result[i] = array[i];
        }
        for (int i = 0; i < size; ++i) {
            for (int j = 1; j < size; ++j) {
                if (result[j] < result[j - 1]) {
                    int tmp = result[j];
                    result[j] = result[j - 1];
                    result[j - 1] = tmp;
                }
            }
        }
        return result;
    }
}