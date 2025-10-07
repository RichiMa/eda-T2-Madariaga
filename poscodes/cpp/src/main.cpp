#include "poscode.hpp"
#include "utils.hpp"

#include <chrono>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>

static bool is_sorted(const Poscode* A, size_t n){
    for (size_t i = 1; i < n; ++i)
        if (A[i-1].getData() > A[i].getData()) return false;
    return true;
}

static void usage(const char* prog){
    std::cout << "Uso: " << prog
              << " --algo=radix|merge|quick --file=PATH --n=N [--print=K]\n";
}

int main(int argc, char** argv){
    std::string algo = "radix";
    std::string file = "../../codes_500K.txt"; 
    size_t n = 500000;
    int toprint = 10;

    auto take_value = [&](const std::string& key, int& i, std::string& out)->bool{
        std::string arg(argv[i]);
        if (arg.compare(0, key.size(), key) != 0) return false;
        if (arg.size() > key.size() && arg[key.size()] == '=') {
            out = arg.substr(key.size() + 1);
            return true;
        }
        if (i + 1 < argc) { out = argv[++i]; return true; }
        return false;
    };

    for (int i = 1; i < argc; ++i){
        std::string val;
        if (take_value("--algo",  i, val)) { algo   = val; continue; }
        if (take_value("--file",  i, val)) { file   = val; continue; }
        if (take_value("--n",     i, val)) { n      = static_cast<size_t>(std::stoull(val)); continue; }
        if (take_value("--print", i, val)) { toprint = std::stoi(val); continue; }

        std::cerr << "Argumento no reconocido: " << argv[i] << "\n";
        usage(argv[0]);
        return 1;
    }

    Poscode* data = readCodes(file, n);
    if (!data){
        std::cerr << "Error leyendo archivo: " << file << "\n";
        return 1;
    }

    auto t0 = std::chrono::high_resolution_clock::now();
    if      (algo == "radix") radix_sort(data, n);
    else if (algo == "merge") merge_sort(data, n);
    else if (algo == "quick") quick_sort(data, n);
    else {
        std::cerr << "Algoritmo no reconocido: " << algo << "\n";
        deleteCodes(data);
        return 1;
    }
    auto t1 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> dt = t1 - t0;
    std::cout.setf(std::ios::fixed); std::cout.precision(6);
    std::cout << "Algo=" << algo << " n=" << n << " time=" << dt.count() << " s\n";
    std::cout << "Sorted? " << (is_sorted(data, n) ? "YES" : "NO") << "\n";

    for (int i = 0; i < toprint && i < static_cast<int>(n); ++i)
        std::cout << data[i].getData() << "\n";

    deleteCodes(data);
    return 0;
}
