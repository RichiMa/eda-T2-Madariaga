#include "poscode.hpp"
#include "utils.hpp"

#include <chrono>
#include <cstring>
#include <iostream>
#include <string>

static bool is_sorted(const Poscode* A, std::size_t n){
    for (std::size_t i = 1; i < n; ++i)
        if (A[i-1].getData() > A[i].getData()) return false;
    return true;
}

int main(int argc, char** argv){
    std::string algo = "radix";
    std::string file = "../codes_500K.txt"; // desde build/, subir un nivel
    std::size_t n = 500000;
    int toprint = 10;

    for (int i = 1; i < argc; ++i){
        if (!std::strncmp(argv[i], "--algo=", 8))       algo = std::string(argv[i] + 8);
        else if (!std::strncmp(argv[i], "--file=", 8))   file = std::string(argv[i] + 8);
        else if (!std::strncmp(argv[i], "--n=", 5))      n = static_cast<std::size_t>(std::stoull(argv[i] + 4));
        else if (!std::strncmp(argv[i], "--print=", 9))  toprint = std::stoi(argv[i] + 9);
        else {
            std::cout << "Uso: " << argv[0]
                      << " --algo=radix|merge|quick --file=PATH --n=N [--print=K]\n";
            return 0;
        }
    }

    Poscode* data = readCodes(file, n);
    if (!data) return 1;

    auto t0 = std::chrono::high_resolution_clock::now();
    if (algo == "radix")      radix_sort(data, n);
    else if (algo == "merge") merge_sort(data, n);
    else if (algo == "quick") quick_sort(data, n);
    else {
        std::cerr << "Algoritmo no reconocido: " << algo << "\n";
        deleteCodes(data);
        return 1;
    }
    auto t1 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> dt = t1 - t0;
    std::cout << "Algo=" << algo << " n=" << n << " time=" << dt.count() << " s\n";
    std::cout << "Sorted? " << (is_sorted(data, n) ? "YES" : "NO") << "\n";

    for (int i = 0; i < toprint && i < static_cast<int>(n); ++i)
        std::cout << data[i].getData() << "\n";

    deleteCodes(data);
    return 0;
}
