#include <iostream>
#include "functions.hpp"

int main(int argc, char** argv) {
    int query;

    std::cout << "0                 - сменить реализацию\n";
    std::cout << "1 {N}             - перевести число в систему счисления\n";
    std::cout << "2 {SIZE} {ARRAY}  - отсортировать массив\n";
    std::cout << "-1                - выход" << std::endl;

    do {
        std::cout << "\033[1m";
        std::cin >> query;
        std::cout << "\033[0m";
        switch(query) {
            case (0):
                std::cout << "Библиотека была подключена на этапе линковки" << std::endl;
                break;
            case (1):
                {
                    long num;
                    std::cin >> num;
                    char* result = translation(num);
                    std::cout << "\033[36m" << result << "\033[0m" << std::endl;
                    delete [] result;
                }
                break;
            case (2):
                {
                    unsigned long size;
                    std::cin >> size;
                    int *array = new int[size];
                    for (int i = 0; i < size; ++i) {
                        std::cin >> array[i];
                    }
                    int* result = sort(array, size);
                    for (int i = 0; i < size; ++i) {
                        std::cout << "\033[36m" << result[i] << ' ';
                    }
                    std::cout << "\033[0m" << std::endl;
                    delete [] array;
                    delete [] result;
                }
                break;
        }
    } while (query != -1);
}