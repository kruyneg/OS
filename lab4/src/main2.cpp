#include <iostream>
#include <dlfcn.h>
#include <functional>

template<class T> std::function<T> make_func(void* ptr) {
    return *reinterpret_cast<T*>(ptr);
}

int main(int argc, char** argv) {
    short current_lib = 1;
    void* handle = dlopen("./libsimple.so", RTLD_LAZY);
    
    char* error;
    if ((error = dlerror()) != NULL) {
        std::cerr << error << std::endl;
        return -1;
    }

    auto translation = make_func<char* (long)>(dlsym(handle, "translation"));
    if ((error = dlerror()) != NULL) {
        std::cerr << error << std::endl;
        return -1;
    }
    auto sort = make_func<int* (int*, const unsigned long&)>(dlsym(handle, "sort"));
    if ((error = dlerror()) != NULL) {
        std::cerr << error << std::endl;
        return -1;
    }
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
                if (current_lib == 2) {
                    dlclose(handle);
                    handle = dlopen("./libsimple.so", RTLD_LAZY);
                    current_lib = 1;
                    std::cout << "Перевод в двоичную и сортировака пузырьком\n";
                }
                else {
                    dlclose(handle);
                    handle = dlopen("./libdifficult.so", RTLD_LAZY);
                    current_lib = 2;
                    std::cout << "Перевод в троичную и сортировка Хоара\n";
                }
                translation = make_func<char* (long)>(dlsym(handle, "translation"));
                sort = make_func<int* (int*, const unsigned long&)>(dlsym(handle, "sort"));
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

    dlclose(handle);
}